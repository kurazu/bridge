#if !defined( RUNJS_H )
#define RUNJS_H

#include "jsapi.h"
#include <Python.h>
#include <structmember.h>

/* The class of the global object. */
static JSClass global_class = {
    "global",
    JSCLASS_GLOBAL_FLAGS
};

/* The name of the module */
static const char * runjs_module_name = "runjs";

/* Structure of JSFunc Python objects */
typedef struct {
    // Required header fields
    PyObject_HEAD
    // The pointer to a compiled SpiderMonkey Function object.
    JS::RootedFunction js_func;
} JSFunc;

/* Function to compile JS code for later reuse. */
static JS::RootedFunction compile_js_func(
    RunJSModuleState * module_state,
    const char * file_name, const int line_no,
    const int arg_count, const char *argnames[],
    const char * code
);

/* Function for executing JS function, getting JSON string as input
 * and returning JSON string as output. */
static const char * run_js_func(
    RunJSModuleState * module_state,
    JS::RootedFunction js_func,
    const int arg_count, const char * arguments_json_string
);

/* Called to cleanup JSFunc object */
static void JSFunc_dealloc(JSFunc* self);

/* Called to create a new object of type JSFunc */
static PyObject * JSFunc_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

/* JSFunc __init__ implementation */
static int JSFunc_init(JSFunc *self, PyObject *args, PyObject *kwds);

/* JSFunc __call__ implementation */
static PyObject * JSFunc_call(PyObject* _self, PyObject *args, PyObject *kwds);

/* Methods of JSFunc */
static PyMethodDef JSFunc_methods[] = {
    {NULL}  /* Sentinel */
};

/* Definition of the JSFunc type. */
static PyTypeObject JSFuncType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "runjs.JSFunc",            /* tp_name */
    sizeof(JSFunc),            /* tp_basicsize */
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

/* Members of the module */
static PyMemberDef JSFunc_members[] = {
    {NULL}  /* Sentinel */
};

/* Called when module if unloaded. */
static void runjs_free(void * module_obj);

/* Module-level state structure that will hold SpiderMonkey variables. */
typedef struct {
    JSRuntime * runtime;
    JSContext * context;
    JS::RootedObject global;
} RunJSModuleState;

/* Definition of the module. */
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


#endif // !defined( RUNJS_H )
