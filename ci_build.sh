#!/usr/bin/env bash

set -x

if [ $BUILD_TYPE == "default" ]; then
    mkdir tmp
    BUILD_PREFIX=$PWD/tmp

    CONFIG_OPTS=()
    CONFIG_OPTS+=("CFLAGS=-I${BUILD_PREFIX}/include")
    CONFIG_OPTS+=("CPPFLAGS=-I${BUILD_PREFIX}/include")
    CONFIG_OPTS+=("CXXFLAGS=-I${BUILD_PREFIX}/include")
    CONFIG_OPTS+=("LDFLAGS=-L${BUILD_PREFIX}/lib")
    CONFIG_OPTS+=("PKG_CONFIG_PATH=${BUILD_PREFIX}/lib/pkgconfig")
    CONFIG_OPTS+=("--prefix=${BUILD_PREFIX}")

    # Clone and build dependencies
    git clone --depth 1 https://github.com/jedisct1/libsodium libsodium
    ( cd libsodium && ./autogen.sh && ./configure "${CONFIG_OPTS[@]}" && make -j4 && make check && make install ) || exit 1

    git clone --depth 1 https://github.com/zeromq/libzmq libzmq
    ( cd libzmq && ./autogen.sh && ./configure "${CONFIG_OPTS[@]}" && make -j4 && make check && make install ) || exit 1

    # Build and check this project
    ( ./autogen.sh && ./configure "${CONFIG_OPTS[@]}" && make -j4 && make check && make memcheck && make install ) || exit 1
else
    cd ./builds/${BUILD_TYPE} && ./ci_build.sh
fi
