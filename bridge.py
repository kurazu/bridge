import json
import inspect

import runjs

print(runjs.run("2 + 3"))

"""
function funca(json_params) {
    var params, result;

    params = JSON.parse(json_params);
    result = (function(a, b) {
        return a + b;
    })(params.a, params.b);

    return JSON.stringify(result);
}
"""


def js_func(func):
    content = func.__doc__.strip()
    name = func.__name__
    arg_spec = inspect.getargspec(func)
    assert arg_spec.varargs is None
    assert arg_spec.keywords is None
    arg_names = arg_spec.args

    compiled_function = runjs.compile(name, arg_names, content)

    def js_func_wrapper(*args, **kwargs):
        args = inspect.getcallargs(func, *args, **kwargs)
        json_args = json.dumps(args)
        json_response = compiled_function(json_args)
        return json.loads(json_response)

    return js_func_wrapper


@js_func
def funcb(a, b=2):
    """
    return a + b;
    """
