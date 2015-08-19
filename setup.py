from distutils.core import setup, Extension


runjs = Extension(
    'runjs',
    sources=['runjs_sm.cpp'],
    include_dirs=[
        '/home/kurazu/apps/spider_monkey/include/mozjs-'
    ],
    # include_dirs=[
    #     '/usr/include',
    #     #'/home/kurazu/apps/v8',
    #     #'/home/kurazu/apps/boost/include'
    # ],
    libraries=['mozjs-', 'm', 'dl'],
    library_dirs=[
        '/home/kurazu/apps/spider_monkey/lib'
    ],
    #     '/usr/local/lib',
    #     '/home/kurazu/apps/v8/out/native/lib.target',
    # ],
    # extra_compile_args=['-std=c++0x'],
    extra_compile_args=['-std=gnu++0x', '-include', '/home/kurazu/apps/spider_monkey/include/mozjs-/js/RequiredDefines.h'],
    # extra_objects=[
    #     '/home/kurazu/tmp/v8/out/x64.release/obj.target/tools/gyp/libv8_base.a',
    #     '/home/kurazu/tmp/v8/out/x64.release/obj.target/tools/gyp/libv8_libbase.a',
    #     '/home/kurazu/tmp/v8/out/x64.release/obj.target/tools/gyp/libv8_nosnapshot.a',
    #     '/home/kurazu/tmp/v8/out/x64.release/obj.target/tools/gyp/libv8_libplatform.a',
    #     '/home/kurazu/tmp/v8/out/x64.release/obj.target/third_party/icu/libicuuc.a',
    #     '/home/kurazu/tmp/v8/out/x64.release/obj.target/third_party/icu/libicui18n.a',
    #     '/home/kurazu/tmp/v8/out/x64.release/obj.target/third_party/icu/libicudata.a'
    # ],
    #extra_link_args=['-Wl,--start-group', '/home/kurazu/tmp/v8/out/x64.release/obj.target/tools/gyp/libv8_base.a', '/home/kurazu/tmp/v8/out/x64.release/obj.target/tools/gyp/libv8_libbase.a', '/home/kurazu/tmp/v8/out/x64.release/obj.target/tools/gyp/libv8_nosnapshot.a', '/home/kurazu/tmp/v8/out/x64.release/obj.target/tools/gyp/libv8_libplatform.a', '/home/kurazu/tmp/v8/out/x64.release/obj.target/third_party/icu/libicuuc.a', '/home/kurazu/tmp/v8/out/x64.release/obj.target/third_party/icu/libicui18n.a', '/home/kurazu/tmp/v8/out/x64.release/obj.target/third_party/icu/libicudata.a', '-Wl,--end-group'],
    #extra_link_args='-Wl,--start-group /home/kurazu/tmp/v8/out/native/obj.target/tools/gyp/libv8_base.a /home/kurazu/tmp/v8/out/native/obj.target/tools/gyp/libv8_libbase.a /home/kurazu/tmp/v8/out/native/obj.target/tools/gyp/libv8_nosnapshot.a /home/kurazu/tmp/v8/out/native/obj.target/tools/gyp/libv8_libplatform.a /home/kurazu/tmp/v8/out/native/obj.target/third_party/icu/libicuuc.a /home/kurazu/tmp/v8/out/native/obj.target/third_party/icu/libicui18n.a /home/kurazu/tmp/v8/out/native/obj.target/third_party/icu/libicudata.a -Wl,--end-group -lrt -ldl'.split(' ')
)


setup(
    name='bridge',
    version='0.1',
    description='This is a demo of Python - JS integration',
    ext_modules=[runjs]
)
