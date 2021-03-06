# bridge
Prototype Python-JS bridge

## Prerequisites
You need Python3.4 to run the extension.

You need build tools and Python development files
```
sudo apt-get install python3-dev build-essential
```

You need to have Firefox's SpiderMonkey engine (no necessarily installed in your system, can be installed in a custom prefix). The version that is supported is ```FIREFOX_AURORA_38_BASE```:
http://hg.mozilla.org/mozilla-central/rev/98086da94ccd

Read instructions on installing SM at https://developer.mozilla.org/en-US/docs/Mozilla/Projects/SpiderMonkey/Build_Documentation

Quick manual:

```
mkdir -p ~/apps/spider_monkey
wget http://hg.mozilla.org/mozilla-central/archive/98086da94ccd.tar.bz2
tar jxvf 98086da94ccd.tar.bz2
cd mozilla-central-98086da94ccd/js/src
autoconf-2.13
mkdir build_OPT.OBJ
cd build_OPT.OBJ
../configure --prefix=~/apps/spider_monkey
make
make install
```

## Installation
There is a ```Makefile``` provided, that contains the exact commands for building and running the package.

You need to have `virtualenv` package installed on your system.

Create a new virtual env and activate it.
```
make virtualenv
source venv/bin/activate
```

Point system variables to SpiderMonkey location
```
export SPIDER_MONKEY_PREFIX='/home/kurazu/apps/spider_monkey'
export SPIDER_MONKEY_VERSION='mozjs-'
```
Build and install the package
```
make build
make install
```

## Running
Remember that SpiderMoneky needs to be in ```LD_LIBRARY_PATH``` when running Python:
```
make shell
```

To run the tests:
```
make test
```

## Usage
Use the ```js_func``` decorator from ```bridge``` module to convert your JavaScript functions into Python functions:
```
from bridge import js_func

@js_func
def math(str):
    """
    var sqrt = Math.sqrt;
    return (function(arg) {
        return sqrt(arg);
    })(str);
    """
```

The JavaScript code is read from the function's docstring. The function itself doesn't have to have any instructions.

You can use your function normally (provided you only supply JSON-able parameters):

```
assert math(9) == 3
```

Exceptions raised by JavaScript will be converted to ```RuntimeException```s. The error message should point the correct location of the JavaScript code inside your original Python source files.

```
>>> import bridge.tests
>>> bridge.tests.runtime(2, 3)
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
  File "/home/kurazu/workspace/bridge/venv/lib/python3.4/site-packages/bridge-0.1-py3.4-linux-x86_64.egg/bridge/__init__.py", line 38, in js_func_wrapper
    json_response = compiled_function(json_args)
RuntimeError: ReferenceError: c is not defined at /home/kurazu/workspace/bridge/venv/lib/python3.4/site-packages/bridge-0.1-py3.4-linux-x86_64.egg/bridge/tests.py:32
```

where the line 32 in ```tests.py``` was
```
return b + c;
```

See more usage examples in ```tests.py```.

If you need less magic and more flexibility, use ```runjs.JSFunc``` objects directly.

Create a new function:
```
>>> import runjs
>>> func = runjs.JSFunc(
...     name='add', file_name='math.py', line_no=20,
...     arg_names=('a', 'b'), code='return {result: a + b};'
... )
```

The ```runjs.JSFunc``` object can be called, but call expects a single argument, which needs to be a ```str``` object with a JSON serialization of the array of parameters:

```
>>> func('[4, 5]')
'{"result":9}'
```

The result of the function will always be a JSON serialization of JavaScript return value.
