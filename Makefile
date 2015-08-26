build:
	python3 setup.py build

run:
	cd /home/kurazu/workspace/bridge/build/lib.linux-x86_64-3.4 && LD_LIBRARY_PATH=.:/home/kurazu/apps/boost/lib/:/home/kurazu/apps/v8/out/x64.release/lib.target LD_DEBUG=all LD_DEBUG_OUTPUT=/tmp/ld.log python3 -c "import runjs"

clean:
	rm -rf ./build

all: clean run


simple:
	g++ helloworld.cpp -o helloworld -Wl,--start-group /home/kurazu/apps/v8/out/native/obj.target/{tools/gyp/libv8_{base,libbase,snapshot,libplatform},third_party/icu/libicu{uc,i18n,data}}.a -Wl,--end-group -lrt -pthread -std=c++0x

dwa:
	g++ helloworld.cpp -o helloworld -I/home/kurazu/apps/v8 -Wl,--start-group /home/kurazu/apps/v8/out/native/obj.target/{tools/gyp/libv8_{base,libbase,nosnapshot,libplatform},third_party/icu/libicu{uc,i18n,data}}.a -Wl,--end-group -lrt -pthread -std=c++0x

runsm: build
	cd /home/kurazu/workspace/bridge/build/lib.linux-x86_64-3.4 && LD_LIBRARY_PATH=.:/home/kurazu/apps/spider_monkey/lib python3 -c "import runjs; f = runjs.JSFunc(); print(f())"

shell:
	cd /home/kurazu/workspace/bridge/build/lib.linux-x86_64-3.4 && LD_LIBRARY_PATH=.:/home/kurazu/apps/spider_monkey/lib python3
