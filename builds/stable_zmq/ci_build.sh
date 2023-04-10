#!/usr/bin/env bash

set -x
set -e

mkdir -p tmp
BUILD_PREFIX=$PWD/tmp

CONFIG_OPTS=()
CONFIG_OPTS+=("CFLAGS=-I${BUILD_PREFIX}/include")
CONFIG_OPTS+=("CPPFLAGS=-I${BUILD_PREFIX}/include")
CONFIG_OPTS+=("CXXFLAGS=-I${BUILD_PREFIX}/include")
CONFIG_OPTS+=("LDFLAGS=-L${BUILD_PREFIX}/lib")
CONFIG_OPTS+=("PKG_CONFIG_PATH=${BUILD_PREFIX}/lib/pkgconfig")
CONFIG_OPTS+=("--prefix=${BUILD_PREFIX}")
CONFIG_OPTS+=("--with-docs=no")
CONFIG_OPTS+=("--without-documentation")
CONFIG_OPTS+=("--quiet")

if ! ((command -v dpkg-query >/dev/null 2>&1 && dpkg-query --list libsodium-dev >/dev/null 2>&1) || \
       (command -v brew >/dev/null 2>&1 && brew ls --versions libsodium >/dev/null 2>&1)); then
    git clone --quiet --depth 1 -b stable https://github.com/jedisct1/libsodium.git
    ( cd libsodium; ./autogen.sh && ./configure --prefix=$BUILD_PREFIX && make -j4 install) || exit 1
fi

# Clone and build dependencies
git clone --quiet --depth 1 https://github.com/zeromq/${ZMQ_REPO} libzmq
cd libzmq
git --no-pager log --oneline -n1
autoreconf --install --force --verbose -I config
./configure "${CONFIG_OPTS[@]}" --with-libsodium
make -j4
make install
cd ../../..

# Start of recipe for dependency: uuid
if (command -v dpkg-query >/dev/null 2>&1 && dpkg-query --list uuid-dev >/dev/null 2>&1) || \
       (command -v brew >/dev/null 2>&1 && brew ls --versions uuid >/dev/null 2>&1) \
; then
    CONFIG_OPTS+=("--with-uuid=yes")
fi

# Start of recipe for dependency: systemd
if (command -v dpkg-query >/dev/null 2>&1 && dpkg-query --list libsystemd-dev >/dev/null 2>&1) || \
       (command -v brew >/dev/null 2>&1 && brew ls --versions systemd >/dev/null 2>&1) \
; then
    CONFIG_OPTS+=("--with-libsystemd=yes")
fi

# Start of recipe for dependency: lz4
if (command -v dpkg-query >/dev/null 2>&1 && dpkg-query --list liblz4-dev >/dev/null 2>&1) || \
       (command -v brew >/dev/null 2>&1 && brew ls --versions lz4 >/dev/null 2>&1) \
; then
    CONFIG_OPTS+=("--with-liblz4=yes")
fi

# Build and check this project
./autogen.sh 2> /dev/null
./configure "${CONFIG_OPTS[@]}"
make -j4
make check
make install
