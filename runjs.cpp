#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <v8.h>

using namespace v8;

void run_in_v8(const char* code) {

  // Get the default Isolate created at startup.
  Isolate* isolate = Isolate::GetCurrent();

  // Create a stack-allocated handle scope.
  HandleScope handle_scope(isolate);

  // Create a new context.
  Handle<Context> context = Context::New(isolate);

  // Enter the context for compiling and running the hello world script.
  Context::Scope context_scope(context);

  // Create a string containing the JavaScript source code.
  Handle<String> source = String::NewFromUtf8(isolate, code);

  // Compile the source code.
  Handle<Script> script = Script::Compile(source);

  // Run the script to get the result.
  Handle<Value> result = script->Run();

  // Convert the result to an UTF8 string and print it.
  String::Utf8Value utf8(result);
  printf("%s\n", *utf8);
  return 0;
}


extern "C" {

#include <Python.h>

static PyObject *
runjs_run(PyObject *self, PyObject *args)
{
    run_in_v8("2 + 3");
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

}
