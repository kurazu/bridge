# The following variables should be set in the environment already:
# export SPIDER_MONKEY_PREFIX=/home/kurazu/apps/spider_monkey
# export SPIDER_MONKEY_VERSION=mozjs-

virtualenv:
	virtualenv -p python3.4 venv

build:
	SPIDER_MONKEY_PREFIX=$(SPIDER_MONKEY_PREFIX) SPIDER_MONKEY_VERSION=$(SPIDER_MONKEY_VERSION) python3 setup.py build

install:
	SPIDER_MONKEY_PREFIX=$(SPIDER_MONKEY_PREFIX) SPIDER_MONKEY_VERSION=$(SPIDER_MONKEY_VERSION) python3 setup.py install

test:
	LD_LIBRARY_PATH="$(SPIDER_MONKEY_PREFIX)/lib" python3 -m bridge.tests

clean:
	rm -rf ./build

shell:
	LD_LIBRARY_PATH="$(SPIDER_MONKEY_PREFIX)/lib" python3
