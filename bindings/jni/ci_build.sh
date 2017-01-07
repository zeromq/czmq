#!/usr/bin/env bash

set -e

# Set this to enable verbose profiling
[ -n "${CI_TIME-}" ] || CI_TIME=""
case "$CI_TIME" in
    [Yy][Ee][Ss]|[Oo][Nn]|[Tt][Rr][Uu][Ee])
        CI_TIME="time -p " ;;
    [Nn][Oo]|[Oo][Ff][Ff]|[Ff][Aa][Ll][Ss][Ee])
        CI_TIME="" ;;
esac

# Set this to enable verbose tracing
[ -n "${CI_TRACE-}" ] || CI_TRACE="no"
case "$CI_TRACE" in
    [Nn][Oo]|[Oo][Ff][Ff]|[Ff][Aa][Ll][Ss][Ee])
        set +x ;;
    [Yy][Ee][Ss]|[Oo][Nn]|[Tt][Rr][Uu][Ee])
        set -x ;;
esac

########################################################################
# Build and check the jni binding
########################################################################

BUILD_PREFIX=/tmp

CONFIG_OPTS=()
CONFIG_OPTS+=("CFLAGS=-I${BUILD_PREFIX}/include")
CONFIG_OPTS+=("CPPFLAGS=-I${BUILD_PREFIX}/include")
CONFIG_OPTS+=("CXXFLAGS=-I${BUILD_PREFIX}/include")
CONFIG_OPTS+=("LDFLAGS=-L${BUILD_PREFIX}/lib")
CONFIG_OPTS+=("PKG_CONFIG_PATH=${BUILD_PREFIX}/lib/pkgconfig")
CONFIG_OPTS+=("--prefix=${BUILD_PREFIX}")
CONFIG_OPTS+=("--with-docs=no")
if [ -z "${CI_CONFIG_QUIET-}" ] || [ "${CI_CONFIG_QUIET-}" = yes ] || [ "${CI_CONFIG_QUIET-}" = true ]; then
    CONFIG_OPTS+=("--quiet")
fi

pushd ../../..

# Clone and build dependencies
[ -z "$CI_TIME" ] || echo "`date`: Starting build of dependencies (if any)..."
$CI_TIME git clone --quiet --depth 1 https://github.com/zeromq/libzmq.git libzmq
cd libzmq
git --no-pager log --oneline -n1
if [ -e autogen.sh ]; then
    $CI_TIME ./autogen.sh 2> /dev/null
fi
if [ -e buildconf ]; then
    $CI_TIME ./buildconf 2> /dev/null
fi
if [ ! -e autogen.sh ] && [ ! -e buildconf ] && [ ! -e ./configure ] && [ -s ./configure.ac ]; then
    $CI_TIME libtoolize --copy --force && \
    $CI_TIME aclocal -I . && \
    $CI_TIME autoheader && \
    $CI_TIME automake --add-missing --copy && \
    $CI_TIME autoconf || \
    $CI_TIME autoreconf -fiv
fi
$CI_TIME ./configure "${CONFIG_OPTS[@]}"
$CI_TIME make -j4
$CI_TIME make install
cd ..

popd
pushd ../..

[ -z "$CI_TIME" ] || echo "`date`: Starting build of currently tested project..."
$CI_TIME ./autogen.sh 2> /dev/null
$CI_TIME ./configure "${CONFIG_OPTS[@]}"
$CI_TIME make -j4
$CI_TIME make install
[ -z "$CI_TIME" ] || echo "`date`: Build completed without fatal errors!"

popd

TERM=dumb PKG_CONFIG_PATH=$BUILD_PREFIX/lib/pkgconfig $CI_TIME ./gradlew build jar
TERM=dumb $CI_TIME ./gradlew clean

########################################################################
#  Build and check the jni android binding
########################################################################

pushd ../../builds/android

. ./ci_build.sh

popd

pushd android

TERM=dumb PKG_CONFIG_PATH=$BUILD_PREFIX/lib/pkgconfig $CI_TIME ./build.sh

popd
