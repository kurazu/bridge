#include "runjs.hpp"

/* The class of the global object. */
JSClass global_class = {
    "global",
    JSCLASS_GLOBAL_FLAGS
};

void
compile_js_func(
    const RunJSModuleState * module_state,
    const char * function_name, const char * file_name, const int line_no,
    const unsigned arg_count, const char *argnames[],
    const char * code, JS::MutableHandleFunction compiled_function
) {
    bool ok;

    // TODO
    JSAutoRequest ar(module_state->context);
    JSAutoCompartment ac(module_state->context, module_state->global);

    JS::AutoObjectVector empty_scope_chain(module_state->context);
    JS::CompileOptions compile_options(module_state->context);
    compile_options.setFileAndLine(file_name, line_no);
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

    if (!ok) {
        throw "Failed to compile";
    }
}

static JS::HandleValueArray
array_to_vector(
    const RunJSModuleState * module_state,
    const unsigned arg_count, JS::HandleValue array_value
) {

    if (!array_value.isObject()) {
        throw "Expected parameter to be object";
    }

    if (array_value.isNullOrUndefined()) {
        throw "Parameter is null or undefined";
    }

    JS::RootedObject array_object(module_state->context, &array_value.toObject());

    JS::AutoValueVector result(module_state->context);

    for (unsigned i = 0; i < arg_count; i++) {
        JS::RootedValue element(module_state->context);
        std::string key_string = std::to_string(i);
        const char * key_cstring = key_string.c_str();
        bool ok = JS_GetProperty(
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
    JS::HandleFunction js_func,
    const unsigned arg_count, const char * arguments_json_cstring
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
    // TODO arg_count should be take from the array
    JS::HandleValueArray arguments = array_to_vector(module_state, arg_count, parse_result);

    JS::RootedValue result(module_state->context);
    ok = JS_CallFunction(
        module_state->context, JS::NullPtr(), js_func, arguments, &result
    );
    if (!ok) {
        throw "JS function call failed";
    }

    JS::AutoValueVector stringify_args(module_state->context);
    stringify_args.append(result);
    JS::RootedValue stringify_result(module_state->context);
    ok = JS_CallFunctionName(
        module_state->context, json_object, "stringify",
        stringify_args, &stringify_result
    );
    if (!ok) {
        throw "Failed to convert result to JSON";
    }
    if (!stringify_result.isString()) {
        throw "stringify result is not a string";
    }
    if (stringify_result.isNullOrUndefined()) {
        throw "stringify result is null or undefined";
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

    // TODO
    // JS_SetErrorReporter(runtime, reportError);

    // TODO

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
    module_state->global.set(NULL);
    JS_DestroyContext(module_state->context);
    module_state->context = NULL;
    JS_DestroyRuntime(module_state->runtime);
    module_state->runtime = NULL;
    JS_ShutDown();
}
