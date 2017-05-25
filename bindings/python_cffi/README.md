#czmq cffi bindings

This package contains low level python bindings for czmq based on cffi library.
Module is compatible with 
 * The “in-line”, “ABI mode”, which simply **dlopen** main library and parse C declaration on runtime
 * The “out-of-line”, “API mode”, which build C **native** Python extension

#Building
Run
    export PKG_CONFIG_PATH=/path/to/your/project/pkgconfig
    python czmq_cffi/build.py
In order to use the bindings, code will fallback to dlopen part, so no need for C
compiler or devel environment installed.

#TODO:
 * proper setuptools installation
