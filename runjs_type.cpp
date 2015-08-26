#include "runjs.hpp"

/* Structure of JSFunc Python objects */
typedef struct {
    // Required header fields
    PyObject_HEAD
    // The pointer to a compiled SpiderMonkey Function object.
    JS::RootedFunction js_func;
} JSFunc;

static void
JSFunc_dealloc(JSFunc* self) {
    self->js_func.set(NULL);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject *
JSFunc_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    JSFunc *self;

    self = (JSFunc *)type->tp_alloc(type, 0);

    return (PyObject *)self;
}

static int
JSFunc_init(JSFunc *self, PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = {
        (char *)"name", (char *)"file_name", (char * )"line_no",
        (char *)"arg_names", (char *)"code", NULL
    };
    const char * function_name;
    const char * file_name;
    int line_no;
    PyObject * arg_names;
    const char * code;
    Py_ssize_t arg_count;

    if (!PyArg_ParseTupleAndKeywords(
        args, kwargs, "ssiOs", kwlist,
        &function_name, &file_name, &line_no, &arg_names, &code
    )) {
        return -1;
    }

    if (!PySequence_Check(arg_names)) {
        PyErr_SetString(PyExc_ValueError, "arg_names param needs to be a list");
        return -1;
    }
    arg_count = PySequence_Size(arg_names);
    const char ** c_arg_names = new const char*[arg_count];
    for (Py_ssize_t i = 0; i < arg_count; i++) {
        PyObject * element = PySequence_GetItem(arg_names, i);
        if (!PyUnicode_Check(element)) {
            PyErr_SetString(PyExc_ValueError, "arg_names must contain str instances");
            return -1;
        }
        c_arg_names[i] = PyUnicode_AsUTF8(element);
    }
    // TODO
    const unsigned nargs = arg_count;

    PyObject* module = PyImport_ImportModule(runjs_module_name);
    if (!module) {
        return -1;
    }

    RunJSModuleState* module_state = (RunJSModuleState*) PyModule_GetState(module);

    JS::RootedFunction compiled_function(module_state->context);
    int return_value = 0;
    try {
        compile_js_func(
            module_state, function_name, file_name, line_no,
            nargs, c_arg_names, code, &compiled_function
        );
        self->js_func = compiled_function;
    } catch (const char * err_msg) {
        return_value = -1;
        PyErr_SetString(PyExc_RuntimeError, err_msg);
    }
    Py_XDECREF(module);
    delete [] c_arg_names;
    c_arg_names = NULL;
    return return_value;
}

static PyObject *
JSFunc_call(PyObject* _self, PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = {(char *)"arguments_json", NULL};
    const char * arguments_json_cstring;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s", kwlist, &arguments_json_cstring)) {
        return NULL;
    }

    JSFunc * self = (JSFunc*) _self;

    PyObject* module = PyImport_ImportModule(runjs_module_name);
    if (!module) {
        return NULL;
    }

    RunJSModuleState* module_state = (RunJSModuleState*) PyModule_GetState(module);

    const char * result_cstring;
    try {
        result_cstring = run_js_func(
            module_state, self->js_func, arguments_json_cstring
        );
    } catch (const char * err_msg) {
        PyErr_SetString(PyExc_RuntimeError, err_msg);
        return NULL;
    }

    PyObject *result = PyUnicode_FromString(result_cstring);
    delete result_cstring;
    return result;
}

/* Methods of JSFunc */
static PyMethodDef JSFunc_methods[] = {
    {NULL}  /* Sentinel */
};

/* Members of JSFunc */
static PyMemberDef JSFunc_members[] = {
    {NULL}  /* Sentinel */
};

/* Definition of the JSFunc type. */
PyTypeObject JSFuncType = {
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
