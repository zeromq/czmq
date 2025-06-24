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

configure_tracing() {
	# Set this to enable verbose tracing
	[ -n "${CI_TRACE-}" ] || CI_TRACE="no"
	case "$CI_TRACE" in
		[Nn][Oo]|[Oo][Ff][Ff]|[Ff][Aa][Ll][Ss][Ee])
			set +x ;;
		[Yy][Ee][Ss]|[Oo][Nn]|[Tt][Rr][Uu][Ee])
			set -x ;;
	esac
}
configure_tracing

fold_start() {
  set +x
  echo -e "travis_fold:start:$1\033[33;1m$2\033[0m"
  configure_tracing
}

fold_end() {
  set +x
  echo -e "\ntravis_fold:end:$1\r"
  configure_tracing
}

LANG=C
LC_ALL=C
export LANG LC_ALL

if [ -d "./tmp" ]; then
    # Proto installation area for this project and its deps
    rm -rf ./tmp
fi
if [ -d "./tmp-deps" ]; then
    # Checkout/unpack and build area for dependencies
    rm -rf ./tmp-deps
fi
mkdir -p tmp tmp-deps
BUILD_PREFIX=$PWD/tmp

# Use tools from prerequisites we might have built
PATH="${BUILD_PREFIX}/sbin:${BUILD_PREFIX}/bin:${PATH}"
export PATH

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

if [ "$CLANG_FORMAT" != "" ] ; then
    CMAKE_OPTS+=("-DCLANG_FORMAT=${CLANG_FORMAT}")
fi

# Clone and build dependencies, if not yet installed to Travis env as DEBs
# or MacOS packages; other OSes are not currently supported by Travis cloud
[ -z "$CI_TIME" ] || echo "`date`: Starting build of dependencies (if any)..."
# Start of recipe for dependency: libmicrohttpd
fold_start dependency.libmicrohttpd "Install dependency libmicrohttpd"
if ! ((command -v dpkg >/dev/null 2>&1 && dpkg -s libmicrohttpd-dev >/dev/null 2>&1) || \
    (command -v brew >/dev/null 2>&1 && brew ls --versions libmicrohttpd >/dev/null 2>&1)) \
; then
    BASE_PWD=${PWD}
    cd tmp-deps
    wget http://ftp.gnu.org/gnu/libmicrohttpd/libmicrohttpd-0.9.44.tar.gz
    tar -xzf $(basename "http://ftp.gnu.org/gnu/libmicrohttpd/libmicrohttpd-0.9.44.tar.gz")
    cd $(basename "http://ftp.gnu.org/gnu/libmicrohttpd/libmicrohttpd-0.9.44.tar.gz" .tar.gz) || exit $?
    CCACHE_BASEDIR=${PWD}
    export CCACHE_BASEDIR
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
    if [ -e ./configure ]; then
        $CI_TIME ./configure "${CONFIG_OPTS[@]}"
    else
        mkdir build
        cd build
        $CI_TIME cmake .. -DCMAKE_INSTALL_PREFIX=$BUILD_PREFIX -DCMAKE_PREFIX_PATH=$BUILD_PREFIX
    fi
    if [ -e ./configure ]; then
        $CI_TIME make -j4
        $CI_TIME make install
    else
        $CI_TIME cmake --build . --config Release --target install
    fi
    cd "${BASE_PWD}"
    CONFIG_OPTS+=("--with-libmicrohttpd=yes")
fi
fold_end dependency.libmicrohttpd


# Start of recipe for dependency: libzmq
fold_start dependency.libzmq "Install dependency libzmq"
if ! ((command -v dpkg >/dev/null 2>&1 && dpkg -s libzmq3-dev >/dev/null 2>&1) || \
      (command -v brew >/dev/null 2>&1 && brew ls --versions libzmq >/dev/null 2>&1)); then
    BASE_PWD=${PWD}
    cd tmp-deps
    $CI_TIME git clone --quiet --depth 1 https://github.com/zeromq/libzmq.git libzmq
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
    if [ -e ./configure ]; then
        $CI_TIME ./configure "${CONFIG_OPTS[@]}"
    else
        mkdir build
        cd build
        $CI_TIME cmake .. -DCMAKE_INSTALL_PREFIX=$BUILD_PREFIX -DCMAKE_PREFIX_PATH=$BUILD_PREFIX
    fi
    if [ -e ./configure ]; then
        $CI_TIME make -j4
        $CI_TIME make install
    else
        $CI_TIME cmake --build . --config Release --target install
    fi
    cd "${BASE_PWD}"
fi
fold_end dependency.libzmq

# Start of recipe for dependency: libcurl
fold_start dependency.libcurl "Install dependency libcurl"
if ! ((command -v dpkg >/dev/null 2>&1 && dpkg -s libcurl4-gnutls-dev >/dev/null 2>&1) || \
      (command -v brew >/dev/null 2>&1 && brew ls --versions libcurl >/dev/null 2>&1)); then
    BASE_PWD=${PWD}
    cd tmp-deps
    $CI_TIME git clone --quiet --depth 1 https://github.com/curl/curl.git libcurl
    cd libcurl
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
    ( # Custom additional options for libcurl
      CONFIG_OPTS+=("--with-secure-transport")
      $CI_TIME ./configure "${CONFIG_OPTS[@]}"
    )
    if [ -e ./configure ]; then
        $CI_TIME make -j4
        $CI_TIME make install
    else
        $CI_TIME cmake --build . --config Release --target install
    fi
    cd "${BASE_PWD}"
    CONFIG_OPTS+=("--with-libcurl=yes")
fi
fold_end dependency.libcurl


cd ../..

# always install custom builds from dist if the autotools chain exists
# to make sure that `make dist` doesn't omit any files required to build & test
if [ -z "$DO_CLANG_FORMAT_CHECK" -a -f configure.ac ]; then
    fold_start check.clang_format_check "Do clang format check"
    $CI_TIME ./autogen.sh
    $CI_TIME ./configure "${CONFIG_OPTS[@]}"
    $CI_TIME make -j5 dist-gzip
    $CI_TIME tar -xzf czmq-4.2.2.tar.gz
    cd czmq-4.2.2
    fold_end check.clang_format_check
fi

# Build and check this project
fold_start build.project "Build and check this project"
[ -z "$CI_TIME" ] || echo "`date`: Starting build of currently tested project..."
CCACHE_BASEDIR=${PWD}
export CCACHE_BASEDIR
if [ "$DO_CLANG_FORMAT_CHECK" = "1" ] ; then
    { PKG_CONFIG_PATH=${BUILD_PREFIX}/lib/pkgconfig $CI_TIME cmake "${CMAKE_OPTS[@]}" . \
      && make clang-format-check-CI ; exit $? ; }
else
    PKG_CONFIG_PATH=${BUILD_PREFIX}/lib/pkgconfig $CI_TIME cmake "${CMAKE_OPTS[@]}" .
    $CI_TIME make all VERBOSE=1 -j4
    $CI_TIME ctest -V
    $CI_TIME make install
fi
[ -z "$CI_TIME" ] || echo "`date`: Builds completed without fatal errors!"

echo "=== Are GitIgnores good after making the project '$BUILD_TYPE'? (should have no output below)"
git status -s || true
echo "==="
fold_end build.project
