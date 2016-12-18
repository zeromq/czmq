# GYP Builds

## About GYP (Generate Your Projects)

GYP is a tool from Google, written in Python, that does multi-platform builds for C/C++ projects. It is rather cleaner and simpler than the old autotools, and CMake approaches. Like zproject, you define a model, not a script. GYP files are written in JSON.

To install GYP on Debian/Ubuntu:

    sudo apt-get install gyp

To install GYP on Windows, you need git and Python 2.7. Then:

    git clone https://chromium.googlesource.com/external/gyp
    cd gyp
    python setup.py install

## How to build this project

To build this project, first check out its dependencies, at the same level as this directory:

    git clone https://github.com/zeromq/libzmq.git

On Linux, build just this project in builds/gyp and it will automatically build its dependencies:

    gyp --depth=.
    make -j5

On Windows, you need Visual Studio 2015. Then:

    gyp --depth=.
    "Program Files (x86)Microsoft Visual Studio 14.0VCbinvcvars32.bat"
    msbuild /m /v:m project.sln

