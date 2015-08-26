#if !defined( RUNJS_H )
#define RUNJS_H

#include "jsapi.h"
#include <Python.h>
#include <structmember.h>
#include <string>

/*****************************************************************************
 * Spider-monkey related symbols.                                            *
 *****************************************************************************/

static typedef struct {
    JSRuntime * runtime;
    JSContext * context;
    JS::RootedObject global;
} RunJSModuleState;

/* Function to compile JS code for later reuse. */
JS::RootedFunction compile_js_func(
    RunJSModuleState * module_state,
    const char * function_name, const char * file_name, const int line_no,
    const unsigned arg_count, const char *argnames[],
    const char * code
);

/* Function for executing JS function, getting JSON string as input
 * and returning JSON string as output. */
const char * run_js_func(
    RunJSModuleState * module_state,
    JS::RootedFunction js_func,
    const int arg_count, const char * arguments_json_cstring
);

/* Initialize Spider Monkey JS engine and populate given module state struct */
void initialize_sm(RunJSModuleState * module_state);

/* Shutdown Spider Monkey JS engine */
void shutdown_sm(RunJSModuleState * module_state);

/*****************************************************************************
 * Python related symbols.                                                   *
 *****************************************************************************/

PyTypeObject JSFuncType;

#endif // !defined( RUNJS_H )
