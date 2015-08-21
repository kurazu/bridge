// This is an example for SpiderMonkey 31.
// For SpiderMonkey 24 or 38, see each comment.

// following code might be needed in some case
// #define __STDC_LIMIT_MACROS
// #include <stdint.h>
#include "jsapi.h"

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

typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
    JSFunction *js_func;
} JSFunc;

static void
JSFunc_dealloc(JSFunc* self)
{
    delete self->js_func;
    self->js_func = nullptr;
    Py_TYPE(self)->tp_free((PyObject*)self);
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

static PyObject *
JSFunc_call(PyObject* _self, PyObject *args, PyObject *kwds)
{
    JSFunc * self = (JSFunc*) _self;
    PyObject *result;
    result = PyUnicode_FromString("");
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

static struct PyModuleDef runjsmodule = {
   PyModuleDef_HEAD_INIT,
   "runjs", /* name of module */
   "Module for executing JavaScript", /* module documentation, may be NULL */
   sizeof(RunJSModuleState), /* size of per-interpreter state of the module,
          or -1 if the module keeps state in global variables. */
   NULL, /* m_methods */
   NULL, /* m_reload */
   NULL, /* m_traverse */
   NULL, /* m_clear */
   NULL, /* m_free */
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
