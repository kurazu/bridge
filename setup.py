from distutils.core import setup, Extension


runjs = Extension(
    'runjs',
    sources=['runjs.cpp'],
    include_dirs=['/usr/local/include', '/home/kurazu/apps/v8', '/home/kurazu/apps/boost/include'],
    libraries=['boost_python', 'v8', 'icui18n', 'icuuc'],
    library_dirs=[
        '/usr/local/lib', '/home/kurazu/apps/v8/out/native/lib.target', '/home/kurazu/apps/boost/lib'
    ],
    extra_compile_args=['-std=c++0x'],
    #extra_link_args='-Wl,--start-group /home/kurazu/apps/v8/out/native/obj.target/tools/gyp/libv8_base.a /home/kurazu/apps/v8/out/native/obj.target/tools/gyp/libv8_libbase.a /home/kurazu/apps/v8/out/native/obj.target/tools/gyp/libv8_nosnapshot.a /home/kurazu/apps/v8/out/native/obj.target/tools/gyp/libv8_libplatform.a /home/kurazu/apps/v8/out/native/obj.target/third_party/icu/libicuuc.a /home/kurazu/apps/v8/out/native/obj.target/third_party/icu/libicui18n.a /home/kurazu/apps/v8/out/native/obj.target/third_party/icu/libicudata.a -Wl,--end-group -lrt -ldl'.split(' ')
)


setup(
    name='bridge',
    version='0.1',
    description='This is a demo of Python - JS integration',
    ext_modules=[runjs]
)
