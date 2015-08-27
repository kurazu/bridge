build:
	python3 setup.py build

run:
	cd /home/kurazu/workspace/bridge/build/lib.linux-x86_64-3.4 && LD_LIBRARY_PATH=.:/home/kurazu/apps/spider_monkey/lib PYTHONPATH=/home/kurazu/workspace/bridge python3 ../../bridge.py

clean:
	rm -rf ./build

all: clean run


simple:
	g++ helloworld.cpp -o helloworld -Wl,--start-group /home/kurazu/apps/v8/out/native/obj.target/{tools/gyp/libv8_{base,libbase,snapshot,libplatform},third_party/icu/libicu{uc,i18n,data}}.a -Wl,--end-group -lrt -pthread -std=c++0x

dwa:
	g++ helloworld.cpp -o helloworld -I/home/kurazu/apps/v8 -Wl,--start-group /home/kurazu/apps/v8/out/native/obj.target/{tools/gyp/libv8_{base,libbase,nosnapshot,libplatform},third_party/icu/libicu{uc,i18n,data}}.a -Wl,--end-group -lrt -pthread -std=c++0x

runsm:
	cd /home/kurazu/workspace/bridge/build/lib.linux-x86_64-3.4 && LD_LIBRARY_PATH=.:/home/kurazu/apps/spider_monkey/lib python3 -c "import runjs; f = runjs.JSFunc(name='f', file_name='make.py', line_no=3, arg_names=('a', 'b'), code='var sum = a + b; return sum;'); print(f('[2, 3]'))"

runbad:
	cd /home/kurazu/workspace/bridge/build/lib.linux-x86_64-3.4 && LD_LIBRARY_PATH=.:/home/kurazu/apps/spider_monkey/lib python3 -c "import runjs; f = runjs.JSFunc(name='f', file_name='make.py', line_no=3, arg_names=('a', 'b'), code='return c + b;'); print(f('[2, 3]'))"

shell:
	cd /home/kurazu/workspace/bridge/build/lib.linux-x86_64-3.4 && LD_LIBRARY_PATH=.:/home/kurazu/apps/spider_monkey/lib PYTHONPATH=/home/kurazu/workspace/bridge python3

gdb:
	echo "import runjs; f = runjs.JSFunc(); print(f())"
	cd /home/kurazu/workspace/bridge/build/lib.linux-x86_64-3.4 && LD_LIBRARY_PATH=.:/home/kurazu/apps/spider_monkey/lib gdb python3
