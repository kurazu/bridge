# bridge
Prototype Python-JS bridge

## Prerequisites
You need Python3.4 to run the extension.

You need build tools and Python development files
```
sudo apt-get install python3-dev build-essential
```

You need to have Firefox's SpiderMonkey engine (no necessarily installed in your system, can be installed in a custom prefix). The version that is supported is FIREFOX_AURORA_38_BASE:
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
You need to have `virtualenv` package installed on your system.

Create a new virutal env and activate it.
```
virtualenv -p python3.4 venv
source venv/bin/activate
```

Point system variables to SpiderMonkey location
```
export SPIDER_MONKEY_PREFIX='/home/kurazu/apps/spider_monkey'
export SPIDER_MONKEY_VERSION='mozjs-'
```
Build and install the package
```
python3 setup.py build
python3 setup.py install
```

## Running
Remember that SpiderMoneky needs to be in ```LD_LIBRARY_PATH``` when running Python:
```
LD_LIBRARY_PATH="${SPIDER_MONKEY_PREFIX}/lib:${LD_LIBRARY_PATH}" python3
```

## Usage
Use the ```js_func``` decorator from ```bridge``` module to convert your JavaScript functions into Python functions:
```
from bridge import js_func

@js_func
def mul(a, b=2):
    """
    return a * b;
    """
```

The JavaScript code is read from the function's docstring. The function itself doesn't have to have any instructions.

You can use your function normally (provided you only supply JSON-able parameters):

```
assert mul(3) == 6
```
