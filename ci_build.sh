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

    # Build, check, and install libsodium if WITH_LIBSODIUM is set
    if [ -n "$WITH_LIBSODIUM" ]; then
        git clone git://github.com/jedisct1/libsodium.git &&
        ( cd libsodium; ./autogen.sh && ./configure --prefix=$BUILD_PREFIX &&
            make check && make install ) || exit 1
    fi

    # Build, check, and install the version of ZeroMQ given by ZMQ_REPO
    git clone git://github.com/zeromq/${ZMQ_REPO}.git &&
    ( cd ${ZMQ_REPO}; ./autogen.sh && ./configure "${CONFIG_OPTS[@]}" &&
        make check && make install ) || exit 1

    # Build, check, and install CZMQ from local source
    (./autogen.sh && ./configure "${CONFIG_OPTS[@]}" && make check-verbose VERBOSE=1 && make install) || exit 1
else
    cd ./builds/${BUILD_TYPE} && ./ci_build.sh
fi

