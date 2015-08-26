#include "runjs.hpp"

/* Definition of the module. */
struct PyModuleDef runjsmodule = {
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

void runjs_free(void * module_obj) {
    PyObject * module = (PyObject*) module_obj;
    RunJSModuleState* module_state = (RunJSModuleState*) PyModule_GetState(module);

    shutdown_sm(module_state);
}

//TODO
// void reportError(JSContext *cx, const char *message, JSErrorReport *report) {
//      fprintf(stderr, "%s:%u:%s\n",
//              report->filename ? report->filename : "[no filename]",
//              (unsigned int) report->lineno,
//              message);
// }

PyMODINIT_FUNC
PyInit_runjs(void)
{
    PyObject* module;

    JSFuncType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&JSFuncType) < 0) {
        return NULL;
    }

    module = PyModule_Create(&runjsmodule);
    if (module == NULL) {
        return NULL;
    }

    RunJSModuleState* module_state = (RunJSModuleState*) PyModule_GetState(module);

    try {
        initialize_sm(module_state);
    catch (const char * err_msg) {
        PyErr_SetString(PyExc_RuntimeError, err_msg);
        return NULL;
    }

    Py_INCREF(&JSFuncType);
    PyModule_AddObject(module, "JSFunc", (PyObject *)&JSFuncType);

    return module;
}
