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

static PyObject *
runjs_run(PyObject *self, PyObject *args)
{
    printf("inside run 1\n");
    int status = run_in_v8("'2' + '3'");
    printf("inside run 2\n");
    return PyLong_FromLong(status);
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
