import json
import inspect
import functools
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
        name=name, file_name=file_name, line_no=line_no, arg_names=arg_names,
        code=content
    )

    @functools.wraps(func)
    def js_func_wrapper(*args, **kwargs):
        call_args = inspect.getcallargs(func, *args, **kwargs)
        call_args_positional = [
            call_args[arg_name]
            for arg_name in arg_names
        ]
        json_args = json.dumps(call_args_positional)
        json_response = compiled_function(json_args)
        return json.loads(json_response)

    return js_func_wrapper


@js_func
def funcb(a, b=2):
    """
    var sum = a + b;
    return sum;
    """

@js_func
def funcbad(a, b=2):
    """
    return c + b;
    """
