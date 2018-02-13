#!/usr/bin/env bash

set -x

# Go back to the root and build, just like the default target Only difference
# is meant to be the final step, where we make install on this project, then
# run the python tests.
cd ../..
mkdir tmp
BUILD_PREFIX=$PWD/tmp

CONFIG_OPTS=()
CONFIG_OPTS+=("CFLAGS=-I${BUILD_PREFIX}/include")
CONFIG_OPTS+=("CPPFLAGS=-I${BUILD_PREFIX}/include")
CONFIG_OPTS+=("CXXFLAGS=-I${BUILD_PREFIX}/include")
CONFIG_OPTS+=("LDFLAGS=-L${BUILD_PREFIX}/lib")
CONFIG_OPTS+=("PKG_CONFIG_PATH=${BUILD_PREFIX}/lib/pkgconfig")
CONFIG_OPTS+=("--prefix=${BUILD_PREFIX}")
CONFIG_OPTS+=("--with-docs=no")
CONFIG_OPTS+=("--quiet")

# Start of recipe for dependency: libzmq
if ! (command -v dpkg-query >/dev/null 2>&1 && dpkg-query --list libzmq3-dev >/dev/null 2>&1) || \
       (command -v brew >/dev/null 2>&1 && brew ls --versions libzmq >/dev/null 2>&1) \
; then
    echo ""
    BASE_PWD=${PWD}
    echo "`date`: INFO: Building prerequisite 'libzmq' from Git repository..." >&2
    git clone --quiet --depth 1 https://github.com/zeromq/libzmq.git libzmq
    cd libzmq
    CCACHE_BASEDIR=${PWD}
    export CCACHE_BASEDIR
    git --no-pager log --oneline -n1
    if [ -e autogen.sh ]; then
        ./autogen.sh 2> /dev/null
    fi
    if [ -e buildconf ]; then
        ./buildconf 2> /dev/null
    fi
    if [ ! -e autogen.sh ] && [ ! -e buildconf ] && [ ! -e ./configure ] && [ -s ./configure.ac ]; then
        libtoolize --copy --force && \
        aclocal -I . && \
        autoheader && \
        automake --add-missing --copy && \
        autoconf || \
        autoreconf -fiv
    fi
    ./configure "${CONFIG_OPTS[@]}"
    make -j4
    make install
    cd "${BASE_PWD}"
fi

# Build and install this project
./autogen.sh 2> /dev/null
./configure --enable-drafts=yes "${CONFIG_OPTS[@]}"
make VERBOSE=1 install

# LD_LIBRARY_PATH/DYLD_LIBRARY_PATH do not work anymore on OSX, change directory instead
BASE_PWD=${PWD}
cd ${BUILD_PREFIX}/lib
LD_LIBRARY_PATH=$BUILD_PREFIX/lib python ${BASE_PWD}/bindings/python/test.py
