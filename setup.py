from distutils.core import setup, Extension


runjs = Extension(
    'runjs',
    sources=['runjs.c'],
    #include_dirs=['/usr/local/include'],
    #libraries=['tcl83'],
    #library_dirs=['/usr/local/lib']
)


setup(
    name='bridge',
    version='0.1',
    description='This is a demo of Python - JS integration',
    ext_modules=[runjs]
)
