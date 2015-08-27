import os
from setuptools import find_packages, setup, Extension

SPIDER_MONKEY_PREFIX = os.environ.get('SPIDER_MONKEY_PREFIX', '/usr/local')
SPIDER_MONKEY_VERSION = os.environ.get('SPIDER_MONKEY_VERSION', 'mozjs-')

runjs = Extension(
    'runjs',
    sources=['src/runjs_sm.cpp', 'src/runjs_type.cpp', 'src/runjs_module.cpp'],
    include_dirs=[
        os.path.join(SPIDER_MONKEY_PREFIX, 'include', SPIDER_MONKEY_VERSION)
    ],
    libraries=[SPIDER_MONKEY_VERSION, 'm', 'dl'],
    library_dirs=[
        os.path.join(SPIDER_MONKEY_PREFIX, 'lib')
    ],
    extra_compile_args=['-std=gnu++0x'],
)


setup(
    name='bridge',
    version='0.1',
    author='Tomasz Maćkowiak',
    author_email='kurazu@kurazu.net',
    description='This is a demo of Python - JS integration',
    url='https://github.com/kurazu/bridge',
    license='MIT',
    classifiers=[
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.4',
    ],
    ext_modules=[runjs],
    packages=find_packages('src', exclude=['examples']),
    package_dir={'': 'src'},
    include_package_data=False,
)
