#!/usr/bin/env bash
set -x

# Note: the ABI compliance checker script currently assumes that:
# 1) Your project sources have a "latest_release" branch or tag
#    to check out and compare the current commit's ABI to;
# 2) Prerequisites are available as packages - no custom rebuilds.

cd ../../

mkdir tmp
BUILD_PREFIX=$PWD/tmp

CONFIG_OPTS=()
CONFIG_OPTS+=("CFLAGS=-I${BUILD_PREFIX}/include -g -Og")
CONFIG_OPTS+=("CPPFLAGS=-I${BUILD_PREFIX}/include")
CONFIG_OPTS+=("CXXFLAGS=-I${BUILD_PREFIX}/include -g -Og")
CONFIG_OPTS+=("LDFLAGS=-L${BUILD_PREFIX}/lib")
CONFIG_OPTS+=("PKG_CONFIG_PATH=${BUILD_PREFIX}/lib/pkgconfig")
CONFIG_OPTS+=("--prefix=${BUILD_PREFIX}")
CONFIG_OPTS+=("--enable-drafts=no")

function print_abi_api_breakages() {
   echo "ABI breakages detected:"
   cat compat_reports/libczmq/${LATEST_VERSION}_to_HEAD/abi_affected.txt | c++filt
   echo "API breakages detected:"
   cat compat_reports/libczmq/${LATEST_VERSION}_to_HEAD/src_affected.txt | c++filt
   exit 1
}

git fetch --unshallow
git fetch --all --tags
LATEST_VERSION=$(git describe --abbrev=0 --tags)

./autogen.sh
./configure "${CONFIG_OPTS[@]}"
make VERBOSE=1 -j5
abi-dumper -public-headers include src/.libs/libczmq.so -o ${BUILD_PREFIX}/libczmq.head.dump -lver HEAD

git clone --depth 1 -b ${LATEST_VERSION} https://github.com/zeromq/czmq latest_release
cd latest_release
./autogen.sh
./configure "${CONFIG_OPTS[@]}"
make VERBOSe=1 -j5
abi-dumper -public-headers include src/.libs/libczmq.so -o ${BUILD_PREFIX}/libczmq.latest.dump -lver ${LATEST_VERSION}

abi-compliance-checker -l libczmq -d1 ${BUILD_PREFIX}/libczmq.latest.dump -d2 ${BUILD_PREFIX}/libczmq.head.dump -list-affected || print_abi_api_breakages
