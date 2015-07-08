#include <Python.h>
#include "include/v8.h"
#include "include/libplatform/libplatform.h"

using namespace v8;

void run_in_v8(const char* code) {
  V8::InitializeICU();
  Platform* platform = platform::CreateDefaultPlatform();
  V8::InitializePlatform(platform);
  V8::Initialize();

  // Create a new Isolate and make it the current one.
  Isolate* isolate = Isolate::New();
  {
    Isolate::Scope isolate_scope(isolate);

    // Create a stack-allocated handle scope.
    HandleScope handle_scope(isolate);

    // Create a new context.
    Local<Context> context = Context::New(isolate);

    // Enter the context for compiling and running the hello world script.
    Context::Scope context_scope(context);

    // Create a string containing the JavaScript source code.
    Local<String> source = String::NewFromUtf8(isolate, code);

    // Compile the source code.
    Local<Script> script = Script::Compile(source);

    // Run the script to get the result.
    Local<Value> result = script->Run();

    // Convert the result to an UTF8 string and print it.
    String::Utf8Value utf8(result);
    printf("RESULT: %s\n", *utf8);
  }

  // Dispose the isolate and tear down V8.
  isolate->Dispose();
  V8::Dispose();
  V8::ShutdownPlatform();
  delete platform;
}

extern "C" {

    static PyObject *
    runjs_run(PyObject *self, PyObject *args) {
        const char *code;

        if (!PyArg_ParseTuple(args, "s", &code))
            return NULL;
        run_in_v8(code);
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
    PyInit_runjs(void) {
        return PyModule_Create(&runjsmodule);
    }

}