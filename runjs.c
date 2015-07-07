#include <Python.h>

static PyObject *
runjs_run(PyObject *self, PyObject *args)
{
    return PyLong_FromLong(5);
}

static PyMethodDef RunjsMethods[] = {
    {"run",  runjs_run, METH_VARARGS, "Execute JavaScript."},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

static struct PyModuleDef runjsmodule = {
   PyModuleDef_HEAD_INIT,
   "runjs", /* name of module */
   "Module for executing JavaScript", /* module documentation, may be NULL */
   -1, /* size of per-interpreter state of the module,
          or -1 if the module keeps state in global variables. */
   RunjsMethods
};

PyMODINIT_FUNC
PyInit_runjs(void)
{
    return PyModule_Create(&runjsmodule);
}
