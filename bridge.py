import json
import inspect

import runjs


def js_func(func):
    content = inspect.getdoc(func)
    name = func.__name__
    arg_spec = inspect.getargspec(func)
    assert arg_spec.varargs is None
    assert arg_spec.keywords is None
    arg_names = arg_spec.args
    file_name = inspect.getsourcefile(func)
    source_lines, first_line_no = inspect.getsourcelines(func)
    first_content_line = content.split('\n')[0]
    first_line_no = func.__code__.co_firstlineno
    for idx, source_line in enumerate(source_lines):
        if first_content_line in source_line:
            line_no = first_line_no + idx
            break
    else:
        raise ValueError("Couldn't determine line number")

    compiled_function = runjs.JSFunc(
        name, arg_names, content, file_name, line_no
    )

    def js_func_wrapper(*args, **kwargs):
        args = inspect.getcallargs(func, *args, **kwargs)
        json_args = json.dumps(args)
        json_response = compiled_function(json_args)
        return json.loads(json_response)

    return js_func_wrapper


@js_func
def funcb(a, b=2):
    """
    var sum = a + b;
    return sum;
    """
