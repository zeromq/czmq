#!/usr/bin/env bash

################################################################################
#  Note: this particular file has been edited for non-standard improvements.   #
#  Please take care to review changes with `git difftool` such as `meld` after #
#  re-generating the project.                                                  #
#  See below for test-randof integration.                                      #
################################################################################

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

LANG=C
LC_ALL=C
export LANG LC_ALL

if [ -d "./tmp" ]; then
    rm -rf ./tmp
fi
mkdir -p tmp
BUILD_PREFIX=$PWD/tmp

PATH="`echo "$PATH" | sed -e 's,^/usr/lib/ccache/?:,,' -e 's,:/usr/lib/ccache/?:,,' -e 's,:/usr/lib/ccache/?$,,' -e 's,^/usr/lib/ccache/?$,,'2`"
CCACHE_PATH="$PATH"
CCACHE_DIR="${HOME}/.ccache"
export CCACHE_PATH CCACHE_DIR PATH
HAVE_CCACHE=no
if which ccache && ls -la /usr/lib/ccache ; then
    HAVE_CCACHE=yes
fi

mkdir -p "${CCACHE_DIR}" || HAVE_CCACHE=no
if [ "$HAVE_CCACHE" = yes ] && [ -d "$CCACHE_DIR" ]; then
    echo "CCache stats before build:"
    ccache -s || true
fi

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

CMAKE_OPTS=()
CMAKE_OPTS+=("-DCMAKE_INSTALL_PREFIX:PATH=${BUILD_PREFIX}")
CMAKE_OPTS+=("-DCMAKE_PREFIX_PATH:PATH=${BUILD_PREFIX}")
CMAKE_OPTS+=("-DCMAKE_LIBRARY_PATH:PATH=${BUILD_PREFIX}/lib")
CMAKE_OPTS+=("-DCMAKE_INCLUDE_PATH:PATH=${BUILD_PREFIX}/include")

# Clone and build dependencies
[ -z "$CI_TIME" ] || echo "`date`: Starting build of dependencies (if any)..."
if ! ((command -v dpkg-query >/dev/null 2>&1 && dpkg-query --list libzmq3-dev >/dev/null 2>&1) || \
       (command -v brew >/dev/null 2>&1 && brew ls --versions libzmq >/dev/null 2>&1)); then
    $CI_TIME git clone --quiet --depth 1 https://github.com/zeromq/libzmq.git libzmq
    BASE_PWD=${PWD}
    cd libzmq
    CCACHE_BASEDIR=${PWD}
    export CCACHE_BASEDIR
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
    cd "${BASE_PWD}"
fi

# Build and check this project
cd ../..
[ -z "$CI_TIME" ] || echo "`date`: Starting build of currently tested project..."
CCACHE_BASEDIR=${PWD}
export CCACHE_BASEDIR
PKG_CONFIG_PATH=${BUILD_PREFIX}/lib/pkgconfig $CI_TIME cmake "${CMAKE_OPTS[@]}" .
$CI_TIME make VERBOSE=1 -j4 all
$CI_TIME ctest -V
$CI_TIME make install

# Note: this is a manual addition for czmq project
if [ -x ./test_randof ] ; then
    echo ""
    echo "`date`: INFO: Starting test of randof()..."
    # Report built-in tunables
    $CI_TIME ./test_randof -h 2>&1 | grep ZSYS || true
    $CI_TIME ./test_randof -r 10000000 -i 300000000 || exit $?
else
    echo "SKIPPED test of randof() : can't find a `pwd`/test_randof" >&2
fi

[ -z "$CI_TIME" ] || echo "`date`: Builds completed without fatal errors!"

echo "=== Are GitIgnores good after making the project '$BUILD_TYPE'? (should have no output below)"
git status -s || true
echo "==="

if [ "$HAVE_CCACHE" = yes ]; then
    echo "CCache stats after build:"
    ccache -s
fi
