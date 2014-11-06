#!/usr/bin/env bash

# Use directory of current script as the build directory and working directory
cd "$( dirname "${BASH_SOURCE[0]}" )"
ANDROID_BUILD_DIR="$(pwd)"

# Get access to android_build functions and variables
source ${ANDROID_BUILD_DIR}/android_build_helper.sh

# Choose a C++ standard library implementation from the ndk
ANDROID_BUILD_CXXSTL="gnustl_shared_48"

# Set up android build environment and set ANDROID_BUILD_OPTS array
android_build_env
android_build_opts

# Use a temporary build directory
cache="/tmp/android_build/${TOOLCHAIN_NAME}"
mkdir -p "${cache}"

##
# Make sure zmq is built and copy the prefix

(android_build_verify_so "libzmq.so" &> /dev/null) || {
    # Use a default value assuming the zmq project sits alongside this one
    test -z "$ZMQ_ROOT" && ZMQ_ROOT="$(cd ../../../libzmq && pwd)"
    
    if [ ! -d "$ZMQ_ROOT" ]; then
        echo "The ZMQ_ROOT directory does not exist"
        echo "  ${ZMQ_ROOT}"
        exit 1
    fi
    
    (${ZMQ_ROOT}/builds/qt-android/build.sh) || exit 1
    UPSTREAM_PREFIX=${ZMQ_ROOT}/builds/qt-android/prefix/${TOOLCHAIN_NAME}
    cp -r ${UPSTREAM_PREFIX}/* ${ANDROID_BUILD_PREFIX}
}

##
# Build czmq from local source

(android_build_verify_so "libczmq.so" "libzmq.so" &> /dev/null) || {
    rm -rf "${cache}/czmq"
    (cp -r ../.. "${cache}/czmq" && cd "${cache}/czmq" \
        && make clean && rm configure config.status)
    rm 
    
    export LIBTOOL_EXTRA_LDFLAGS='-avoid-version'
    
    (cd "${cache}/czmq" && ./autogen.sh \
        && ./configure "${ANDROID_BUILD_OPTS[@]}" \
        && make \
        && make install) || exit 1
}

##
# Verify shared libraries in prefix

android_build_verify_so "libzmq.so"
android_build_verify_so "libczmq.so" "libzmq.so"
