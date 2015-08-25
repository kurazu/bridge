// This is an example for SpiderMonkey 31.
// For SpiderMonkey 24 or 38, see each comment.

// following code might be needed in some case
// #define __STDC_LIMIT_MACROS
// #include <stdint.h>
#include "jsapi.h"
#include <string>

/* The class of the global object. */
static JSClass global_class = {
    "global",
    JSCLASS_GLOBAL_FLAGS,
    // [SpiderMonkey 38] Following Stubs are removed. Remove those lines.
};

typedef struct {
    JSRuntime * runtime;
    JSContext * context;
    JS::RootedObject global;
} RunJSModuleState;

int run_in_v8(const char* script)
{
    printf("inside run_in_v8 1\n");
    // [SpiderMonkey 24] JS_Init does not exist. Remove this line.
    JS_Init();
    printf("inside run_in_v8 2\n");

    // [SpiderMonkey 38] useHelperThreads parameter is removed.
    JSRuntime *rt = JS_NewRuntime(8L * 1024 * 1024);
    printf("inside run_in_v8 3\n");
    //JSRuntime *rt = JS_NewRuntime(8L * 1024 * 1024, JS_USE_HELPER_THREADS);
    printf("inside run_in_v8 4\n");
    if (!rt) {
        printf("inside run_in_v8 5\n");
        return 1;
    }

    printf("inside run_in_v8 6\n");
    JSContext *cx = JS_NewContext(rt, 8192);
    printf("inside run_in_v8 7\n");
    if (!cx) {
        printf("inside run_in_v8 8\n");
        return 1;
    }

    // [SpiderMonkey 24] hookOption parameter does not exist.
    // JS::RootedObject global(cx, JS_NewGlobalObject(cx, &global_class, nullptr));
    printf("inside run_in_v8 9\n");
    JS::RootedObject global(cx, JS_NewGlobalObject(cx, &global_class, nullptr, JS::FireOnNewGlobalHook));
    printf("inside run_in_v8 10\n");
    if (!global) {
        printf("inside run_in_v8 11\n");
        return 1;
    }

    printf("inside run_in_v8 12\n");
    JS::RootedValue rval(cx);
    printf("inside run_in_v8 13\n");

    {
      JSAutoCompartment ac(cx, global);
      JS_InitStandardClasses(cx, global);

      const char *filename = "noname";
      int lineno = 1;
      // [SpiderMonkey 24] The type of rval parameter is 'jsval *'.
      // bool ok = JS_EvaluateScript(cx, global, script, strlen(script), filename, lineno, rval.address());
      // [SpiderMonkey 38] JS_EvaluateScript is replaced with JS::Evaluate.
      JS::CompileOptions opts(cx);
      opts.setFileAndLine(filename, lineno);
      bool ok = JS::Evaluate(cx, global, opts, script, strlen(script), &rval);
      //bool ok = JS_EvaluateScript(cx, global, script, strlen(script), filename, lineno, &rval);
      if (!ok)
        return 1;
    }

    printf("inside run_in_v8 14\n");
    JSString *str = rval.toString();
    printf("inside run_in_v8 15\n");
    printf("%s\n", JS_EncodeString(cx, str));
    printf("inside run_in_v8 16\n");

    JS_DestroyContext(cx);
    printf("inside run_in_v8 17\n");
    JS_DestroyRuntime(rt);
    printf("inside run_in_v8 18\n");
    JS_ShutDown();
    printf("inside run_in_v8 19\n");
    return 0;
}


extern "C" {

#include <Python.h>
#include <structmember.h>

const char * runjs_module_name = "runjs";

typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
    JS::RootedFunction js_func;
} JSFunc;

static void
JSFunc_dealloc(JSFunc* self)
{
    printf("DEALLOC START\n");
    //delete self->js_func;
    printf("DEALLOC REF\n");
    //self->js_func = nullptr;
    printf("DEALLOC ZERO\n");
    Py_TYPE(self)->tp_free((PyObject*)self);
    printf("DEALLOC DONE\n");
}

static PyObject *
JSFunc_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    JSFunc *self;

    self = (JSFunc *)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->js_func = nullptr;
    }

    return (PyObject *)self;
}

static int
JSFunc_init(JSFunc *self, PyObject *args, PyObject *kwds)
{
    printf("INIT 1\n");
    PyObject* module = PyImport_ImportModule(runjs_module_name);
    if (!module) {
        return -1;
    }
    printf("INIT 2\n");
    RunJSModuleState* module_state = (RunJSModuleState*) PyModule_GetState(module);
    printf("INIT 3\n");
    const char * function_name = "add";
    const char * code = "return JSON.stringify(a + b)";
    unsigned nargs = 2;

    bool ok;
    {
      JSAutoRequest ar(module_state->context);
      JSAutoCompartment ac(module_state->context, module_state->global);
      JS_InitStandardClasses(module_state->context, module_state->global);

        JS::AutoObjectVector emptyScopeChain(module_state->context);
        printf("INIT 4\n");
        const char *argnames[2] = {"a", "b"};
        printf("INIT 5\n\n\n\n");
        JS::CompileOptions compile_options(module_state->context);
        printf("INIT 6\n");
        compile_options.setFileAndLine("python", 0);
        printf("INIT 7\n");
        JS::RootedFunction fun(module_state->context);
        printf("INIT 8\n");
        ok = JS::CompileFunction(
            module_state->context,
            emptyScopeChain,
            compile_options,
            function_name,
            nargs,
            argnames,
            code,
            strlen(code),
            &fun
        );
        printf("INIT 9\n");
        if (!ok) {
            printf("INIT 9a\n");
            PyErr_SetString(PyExc_RuntimeError, "Failed to compile");
            Py_XDECREF(module);
            return -1;
        }
        printf("INIT 10\n");
        self->js_func = fun;
        printf("INIT 11\n");
        Py_XDECREF(module);
        printf("INIT 12\n");
    }
    printf("INIT 13\n");

    // PyObject *first=NULL, *last=NULL, *tmp;

    // static char *kwlist[] = {"first", "last", "number", NULL};

    // if (! PyArg_ParseTupleAndKeywords(args, kwds, "|OOi", kwlist,
    //                                   &first, &last,
    //                                   &self->number))
    //     return -1;

    // if (first) {
    //     tmp = self->first;
    //     Py_INCREF(first);
    //     self->first = first;
    //     Py_XDECREF(tmp);
    // }

    // if (last) {
    //     tmp = self->last;
    //     Py_INCREF(last);
    //     self->last = last;
    //     Py_XDECREF(tmp);
    // }

    return 0;
}

static PyMemberDef JSFunc_members[] = {
    {NULL}  /* Sentinel */
};

void reportError(JSContext *cx, const char *message, JSErrorReport *report) {
     fprintf(stderr, "%s:%u:%s\n",
             report->filename ? report->filename : "[no filename]",
             (unsigned int) report->lineno,
             message);
}

static PyObject *
JSFunc_call(PyObject* _self, PyObject *args, PyObject *kwds)
{
    const char * input_arguments = "[2, 3]";
    printf("CALL\n");
    JSFunc * self = (JSFunc*) _self;

    PyObject* module = PyImport_ImportModule(runjs_module_name);
    if (!module) {
        return NULL;
    }
    printf("INIT CALL 1\n");
    RunJSModuleState* module_state = (RunJSModuleState*) PyModule_GetState(module);
    printf("INIT CALL 2\n");
    //JS::AutoValueVector func_args(module_state->context);
    JS::RootedValue rval(module_state->context);
    printf("INIT CALL 3\n");
    JS::Value arguments[2];
    arguments[0] = INT_TO_JSVAL(2);
    arguments[1] = INT_TO_JSVAL(3);


    bool ok;
    JS::RootedValue json(module_state->context);
    printf("CALL 7\n");
    ok = JS_GetProperty(module_state->context, module_state->global, "JSON", &json);
    printf("CALL 8\n");
    if (!ok) {
        printf("CALL 9\n");
        PyErr_SetString(PyExc_RuntimeError, "Get JSON failed");
        return NULL;
    }
    printf("CALL 10\n");
    JS::RootedObject json_object(module_state->context, &json.toObject());
    printf("CALL 10a\n");

    JS::RootedString input_str(module_state->context);
    printf("CALL 10aa\n");
    {
        JSAutoCompartment ac4(module_state->context, module_state->global);
        JSString * z = JS_NewStringCopyZ(module_state->context, input_arguments);
        printf("CALL 10aaa\n");
        input_str = z;
        printf("CALL 10b\n");
    }
    JS::RootedValue input_value(module_state->context, STRING_TO_JSVAL(input_str));
    printf("CALL 10c\n");
    JS::AutoValueVector parse_args(module_state->context);
    printf("CALL 10d\n");
    parse_args.append(input_value);
    printf("CALL 10e\n");
    JS::RootedValue parse_result(module_state->context);
    printf("CALL 10f\n");
    {
        JSAutoCompartment ac3(module_state->context, module_state->global);
        printf("CALL 10g\n");
        ok = JS_CallFunctionName(module_state->context, json_object, "parse", parse_args, &parse_result);
        printf("CALL 10h\n");
    }
    if (!ok) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to parse arguments");
        return NULL;
    }
    printf("CALL 10i\n");

    {
        JSAutoCompartment ac(module_state->context, module_state->global);
        JS::RootedObject parse_object(module_state->context, &parse_result.toObject());
        printf("CALL 10j\n");

        JS::AutoValueVector argv(module_state->context);
        printf("CALL 10k\n");
        for (int i = 0; i < 2; i++) {
            printf("CALL 10k %d\n", i);
            JS::RootedValue argument(module_state->context);
            printf("CALL 10l\n");
            std::string prop_name = std::to_string(i);
            printf("CALL 10m\n");
            const char * prop_name_cstr = prop_name.c_str();
            printf("CALL 10n\n");
            ok = JS_GetProperty(module_state->context, parse_object, prop_name_cstr, &argument);
            printf("CALL 10o\n");
            if (!ok) {
                printf("CALL 10p\n");
                PyErr_SetString(PyExc_RuntimeError, "Failed to get argument");
                return NULL;
            }
            printf("CALL 10q\n");
            argv.append(argument);
            printf("CALL 10r\n");
        }
        printf("CALL 10s\n");
        ok = JS_CallFunction(
            module_state->context, JS::NullPtr(), self->js_func,
            argv, &rval
        );
    }
    printf("CALL 4\n");
    if (!ok) {
        printf("CALL 5\n");
        PyErr_SetString(PyExc_RuntimeError, "Function call failed");
        return NULL;
    }
    printf("CALL 6\n");

    printf("CALL 11\n");
    JS::AutoValueVector stringify_args(module_state->context);
    printf("CALL 12\n");
    stringify_args.append(rval);
    printf("CALL 13\n");
    JS::RootedValue stringify_result(module_state->context);
    printf("CALL 14\n");
    {
        JSAutoCompartment ac2(module_state->context, module_state->global);
        ok = JS_CallFunctionName(module_state->context, json_object, "stringify", stringify_args, &stringify_result);
    }
    printf("CALL 15\n");
    if (!ok) {
        printf("CALL 16\n");
        PyErr_SetString(PyExc_RuntimeError, "Failed to convert to JSON");
        return NULL;
    }
    printf("CALL 17\n");
    JS::RootedString str(module_state->context, stringify_result.toString());
    printf("CALL 18\n");
    char * result_string = JS_EncodeStringToUTF8(module_state->context, str);
    printf("CALL 19\n");
    if (result_string == NULL) {
        printf("CALL 20\n");
        PyErr_SetString(PyExc_RuntimeError, "Failed to convert result to string");
        return NULL;
    }
    printf("CALL 21\n");
    PyObject *result;
    result = PyUnicode_FromString(result_string);
    delete result_string;
    printf("CALL END\n");
    return result;
}

static PyMethodDef JSFunc_methods[] = {
    {NULL}  /* Sentinel */
};

static PyTypeObject JSFuncType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "runjs.JSFunc",            /* tp_name */
    sizeof(JSFunc),/* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)JSFunc_dealloc,/* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_reserved */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    JSFunc_call,               /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
    "JavaScript function",     /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    JSFunc_methods,            /* tp_methods */
    JSFunc_members,            /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)JSFunc_init,     /* tp_init */
    0,                         /* tp_alloc */
    JSFunc_new,                /* tp_new */
};

void runjs_free(void * module_obj) {
    printf("runjs_free %p\n", module_obj);
    PyObject * module = (PyObject*) module_obj;
    RunJSModuleState* module_state = (RunJSModuleState*) PyModule_GetState(module);
    JS_DestroyContext(module_state->context);
    module_state->context = NULL;
    JS_DestroyRuntime(module_state->runtime);
    module_state->runtime = NULL;
    JS_ShutDown();
    printf("runjs_freed\n");
}

static struct PyModuleDef runjsmodule = {
   PyModuleDef_HEAD_INIT,
   runjs_module_name, /* name of module */
   "Module for executing JavaScript", /* module documentation, may be NULL */
   sizeof(RunJSModuleState), /* size of per-interpreter state of the module,
          or -1 if the module keeps state in global variables. */
   NULL, /* m_methods */
   NULL, /* m_reload */
   NULL, /* m_traverse */
   NULL, /* m_clear */
   runjs_free, /* m_free */
};



PyMODINIT_FUNC
PyInit_runjs(void)
{
    PyObject* module;

    JSFuncType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&JSFuncType) < 0) {
        return NULL;
    }

    printf("1\n");
    module = PyModule_Create(&runjsmodule);
    if (module == NULL) {
        return NULL;
    }
    printf("2\n");
    RunJSModuleState* module_state = (RunJSModuleState*) PyModule_GetState(module);
    printf("3\n");
    printf("init mod %p\n", module);
    printf("init state %p\n", module_state);

    printf("0\n");
    JS_Init();
    printf("0.5\n");

    JSRuntime * runtime = JS_NewRuntime(8L * 1024 * 1024);
    printf("4\n");
    if (!runtime) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create JS runtime");
        return NULL;
    }
    printf("5\n");
    module_state->runtime = runtime;
    printf("6\n");

    JSContext * context = JS_NewContext(runtime, 8192);
    if (!context) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create JS context");
        return NULL;
    }
    module_state->context = context;

    JS_SetErrorReporter(runtime, reportError);

    JSAutoRequest ar(context);

    JS::RootedObject global(context, JS_NewGlobalObject(context, &global_class, nullptr, JS::FireOnNewGlobalHook));
    if (!global) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create JS global object");
        return NULL;
    }
    module_state->global = global;
    printf("initialized sm\n");

    Py_INCREF(&JSFuncType);
    PyModule_AddObject(module, "JSFunc", (PyObject *)&JSFuncType);
    return module;
}

}
