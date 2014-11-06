#!/usr/bin/env bash

# Assumes Travis CI os:linux (x86_64)
(cd '/tmp' \
    && wget http://dl.google.com/android/ndk/android-ndk-r9-linux-x86_64.tar.bz2 \
    && tar -xf android-ndk-r9-linux-x86_64.tar.bz2 \
    && mv android-ndk-r9 android-ndk)

export ANDROID_NDK_ROOT="/tmp/android-ndk"
export TOOLCHAIN_PATH="/tmp/android-ndk/toolchains/arm-linux-androideabi-4.8/prebuilt/linux-x86_64/bin"
export TOOLCHAIN_NAME="arm-linux-androideabi-4.8"
export TOOLCHAIN_HOST="arm-linux-androideabi"
export TOOLCHAIN_ARCH="arm"

export ZMQ_ROOT="/tmp/libzmq"
git clone git://github.com/zeromq/libzmq.git $ZMQ_ROOT

source ./build.sh
