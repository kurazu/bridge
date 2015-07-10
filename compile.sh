#!/bin/bash
g++ helloworld.cpp -o helloworld -I/home/kurazu/apps/v8 -I/usr/local/include -Wl,--start-group /home/kurazu/apps/v8/out/x64.release/obj.target/{tools/gyp/libv8_{base,libbase,nosnapshot,libplatform},third_party/icu/libicu{uc,i18n,data}}.a -Wl,--end-group -lrt -ldl -pthread -std=c++0x
#g++ helloworld.cpp -o helloworld -I/home/kurazu/apps/v8 -I/usr/local/include /home/kurazu/apps/v8/out/native/obj.target/{tools/gyp/libv8_{base,libbase,nosnapshot,libplatform},third_party/icu/libicu{uc,i18n,data}}.a -lrt -ldl -pthread -std=c++0x
