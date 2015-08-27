#ifndef RUNJS_H
#define RUNJS_H

#include "jsapi.h"
#include <Python.h>
#include <structmember.h>

/*****************************************************************************
 * Spider-monkey related symbols.                                            *
 *****************************************************************************/

typedef struct {
    JSRuntime * runtime;
    JSContext * context;
    JS::RootedObject global;
} RunJSModuleState;

typedef struct {
    bool occured = false;
    const char * message = NULL;
    const char * file_name = NULL;
    unsigned line_no = 0;
    void reset();
    void set(const char * n_message, const char * n_file_name, unsigned n_line_no);
} JSError;

/* Function to compile JS code for later reuse. */
extern void compile_js_func(
    const RunJSModuleState * module_state,
    const char * function_name, const char * file_name, const int line_no,
    const unsigned arg_count, const char *argnames[],
    const char * code, JS::MutableHandleFunction compiled_function
);

/* Function for executing JS function, getting JSON string as input
 * and returning JSON string as output. */
extern const char * run_js_func(
    const RunJSModuleState * module_state,
    JS::HandleFunction js_func, const char * arguments_json_cstring
);

/* Initialize Spider Monkey JS engine and populate given module state struct */
extern void initialize_sm(RunJSModuleState * module_state);

/* Shutdown Spider Monkey JS engine */
extern void shutdown_sm(RunJSModuleState * module_state);

/*****************************************************************************
 * Python related symbols.                                                   *
 *****************************************************************************/

/* The name of the module */
extern const char * runjs_module_name;

/* Definition of the JSFunc type */
extern PyTypeObject JSFuncType;

#endif // !defined( RUNJS_H )
