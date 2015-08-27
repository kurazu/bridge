#include "runjs.hpp"
#include <cstring>
#include <string>

/* The class of the global object. */
JSClass global_class = {
    "global",
    JSCLASS_GLOBAL_FLAGS
};

void
JSError::reset() {
    occured = false;
    if (message != NULL) {
        delete [] message;
    }
    message = NULL;
    if (file_name != NULL) {
        delete [] file_name;
    }
    file_name = NULL;
    line_no = 0;
}

static inline char *
s_cpy(const char * str) {
    const size_t length = std::strlen(str);
    char * result = new char[length + 1];
    std::strcpy(result, str);
    return result;
}

void
JSError::set(const char * n_message, const char * n_file_name, unsigned n_line_no) {
    occured = true;
    message = s_cpy(n_message);
    file_name = s_cpy(n_file_name);
    line_no = n_line_no;
}

inline static JSError *
get_js_error(const RunJSModuleState * module_state) {
    return (JSError *)JS_GetContextPrivate(module_state->context);
}

static void
error_reporter(JSContext *context, const char *message, JSErrorReport *report) {
    JSError * context_data = (JSError *)JS_GetContextPrivate(context);
    context_data->set(message, report->filename, report->lineno);
}

static void
enable_error_reporter(const RunJSModuleState * module_state) {
    JSError * context_data = get_js_error(module_state);
    context_data->reset();
    JS_SetErrorReporter(module_state->runtime, error_reporter);
}

static void
disable_error_reporter(const RunJSModuleState * module_state) {
    JS_SetErrorReporter(module_state->runtime, NULL);
}

void
compile_js_func(
    const RunJSModuleState * module_state,
    const char * function_name, const char * file_name, const int line_no,
    const unsigned arg_count, const char *argnames[],
    const char * code, JS::MutableHandleFunction compiled_function
) {
    bool ok;

    JSAutoCompartment ac(module_state->context, module_state->global);

    JS::AutoObjectVector empty_scope_chain(module_state->context);
    JS::CompileOptions compile_options(module_state->context);
    compile_options.setFileAndLine(file_name, line_no);

    enable_error_reporter(module_state);
    ok = JS::CompileFunction(
        module_state->context,
        empty_scope_chain,
        compile_options,
        function_name,
        arg_count,
        argnames,
        code,
        strlen(code),
        compiled_function
    );
    disable_error_reporter(module_state);
    if (!ok) {
        JSError * error = get_js_error(module_state);
        if (error->occured) {
            throw error;
        } else {
            throw "Failed to compile";
        }
    }
}

static JS::HandleValueArray
array_to_vector(
    const RunJSModuleState * module_state, JS::HandleValue array_value
) {
    bool ok;

    if (!array_value.isObject()) {
        throw "Expected parameter to be object";
    }

    if (array_value.isNullOrUndefined()) {
        throw "Parameter is null or undefined";
    }

    JS::RootedObject array_object(module_state->context, &array_value.toObject());

    JS::AutoValueVector result(module_state->context);

    JS::RootedValue length_value(module_state->context);

    ok = JS_GetProperty(module_state->context, array_object, "length", &length_value);
    if (!ok) {
        throw "Couldn't get length";
    }

    if (!length_value.isNumber()) {
        throw "Length is not a number";
    }

    const int32_t length = length_value.toInt32();

    for (int32_t i = 0; i < length; i++) {
        JS::RootedValue element(module_state->context);
        std::string key_string = std::to_string(i);
        const char * key_cstring = key_string.c_str();
        ok = JS_GetProperty(
            module_state->context, array_object, key_cstring, &element
        );
        if (!ok) {
            throw "Failed to get array element";
        }
        result.append(element);
    }

    return result;
}

const char *
run_js_func(
    const RunJSModuleState * module_state,
    JS::HandleFunction js_func, const char * arguments_json_cstring
) {
    bool ok;

    JSAutoCompartment ac(module_state->context, module_state->global);

    JS::RootedValue json(module_state->context);
    ok = JS_GetProperty(module_state->context, module_state->global, "JSON", &json);
    if (!ok) {
        throw "Failed to get JSON literal";
    }
    if (!json.isObject()) {
        throw "JSON is not an object";
    }
    if (json.isNullOrUndefined()) {
        throw "JSON is null or undefined";
    }

    JS::RootedObject json_object(module_state->context, &json.toObject());

    JS::RootedString arguments_json_string(module_state->context);
    JSString * arguments_json_jsstring = JS_NewStringCopyZ(module_state->context, arguments_json_cstring);
    if (arguments_json_jsstring == NULL) {
        throw "Couldn't convert arguments to JSString";
    }
    arguments_json_string = arguments_json_jsstring;
    JS::RootedValue arguments_json_value(module_state->context, STRING_TO_JSVAL(arguments_json_string));

    JS::AutoValueVector parse_args(module_state->context);
    parse_args.append(arguments_json_value);
    JS::RootedValue parse_result(module_state->context);
    ok = JS_CallFunctionName(
        module_state->context, json_object, "parse", parse_args, &parse_result
    );
    if (!ok) {
        throw "Failed to parse arguments JSON";
    }
    JS::HandleValueArray arguments = array_to_vector(module_state, parse_result);

    JS::RootedValue result(module_state->context);
    enable_error_reporter(module_state);
    ok = JS_CallFunction(
        module_state->context, JS::NullPtr(), js_func, arguments, &result
    );
    disable_error_reporter(module_state);
    if (!ok) {
        JSError * error = get_js_error(module_state);
        if (error->occured) {
            throw error;
        } else {
            throw "JS function call failed";
        }
    }
    if (result.isUndefined()) {
        /* Special case - undefined value cannot be serialized as JSON.
         * We will be treating it as null, so it can be serialized. */
        result.setNull();
    }
    JS::AutoValueVector stringify_args(module_state->context);
    stringify_args.append(result);
    JS::RootedValue stringify_result(module_state->context);
    ok = JS_CallFunctionName(
        module_state->context, json_object, "stringify",
        stringify_args, &stringify_result
    );
    if (!ok) {
        throw "Couldn't covert result to JSON - stringify call failed";
    }
    if (stringify_result.isNullOrUndefined()) {
        throw "Couldn't covert result to JSON - stringify result is null or undefined";
    }
    if (!stringify_result.isString()) {
        throw "Couldn't covert result to JSON - stringify result is not a string";
    }
    JS::RootedString stringify_result_string(
        module_state->context, stringify_result.toString()
    );
    const char * result_cstring = JS_EncodeStringToUTF8(
        module_state->context, stringify_result_string
    );
    if (result_cstring == NULL) {
        throw "Failed to convert stringify result to cstring";
    }
    return result_cstring;
}

/* Initialize Spider Monkey JS engine and populate given module state struct */
void
initialize_sm(RunJSModuleState * module_state) {
    JS_Init();
    JSRuntime * runtime = JS_NewRuntime(8L * 1024 * 1024);
    if (!runtime) {
        throw "Failed to create JS runtime";
    }
    module_state->runtime = runtime;

    JSContext * context = JS_NewContext(runtime, 8192);
    if (!context) {
        throw "Failed to create JS context";
    }
    module_state->context = context;

    /* Allocate space for storing information about exceptions. */
    JSError * js_error = new JSError();
    JS_SetContextPrivate(context, (void *)js_error);

    JS::RootedObject global(context, JS_NewGlobalObject(
        context, &global_class, nullptr, JS::FireOnNewGlobalHook
    ));
    if (!global) {
        throw "Failed to create JS global object";
    }
    module_state->global = global;

    JSAutoCompartment ac(module_state->context, module_state->global);
    bool ok = JS_InitStandardClasses(module_state->context, module_state->global);
    if (!ok) {
        throw "Failed to initialize standard JS classes";
    }
}

/* Shutdown Spider Monkey JS engine */
void
shutdown_sm(RunJSModuleState * module_state) {
    JSError * js_error = (JSError *)JS_GetContextPrivate(module_state->context);
    js_error->reset();
    delete js_error;
    JS_SetContextPrivate(module_state->context, NULL);
    module_state->global.set(NULL);
    JS_DestroyContext(module_state->context);
    module_state->context = NULL;
    JS_DestroyRuntime(module_state->runtime);
    module_state->runtime = NULL;
    JS_ShutDown();
}
