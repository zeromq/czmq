#!/usr/bin/env bash

set -x

mkdir tmp
BUILD_PREFIX=$PWD/tmp

CONFIG_OPTS=()
CONFIG_OPTS+=("CFLAGS=-I${BUILD_PREFIX}/include")
CONFIG_OPTS+=("CPPFLAGS=-I${BUILD_PREFIX}/include")
CONFIG_OPTS+=("CXXFLAGS=-I${BUILD_PREFIX}/include")
CONFIG_OPTS+=("LDFLAGS=-L${BUILD_PREFIX}/lib")
CONFIG_OPTS+=("PKG_CONFIG_PATH=${BUILD_PREFIX}/lib/pkgconfig")
CONFIG_OPTS+=("--prefix=${BUILD_PREFIX}")

git clone git://github.com/jedisct1/libsodium.git &&
( cd libsodium; ./autogen.sh && ./configure --prefix=$BUILD_PREFIX && make check && make install ) || exit 1

# Build, check, and install ZeroMQ
git clone git://github.com/zeromq/libzmq.git &&
( cd libzmq; ./autogen.sh && ./configure "${CONFIG_OPTS[@]}" && make check && make install ) || exit 1

# Build, check, and install CZMQ from local source
( cd ../..; ./autogen.sh && ./configure "${CONFIG_OPTS[@]}" && make check-py ) || exit 1
