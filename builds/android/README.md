# Android Build

## Preamble

The last known NDK is automatically downloaded, if not specified otherwise.

As indicated in LIBZMQ main [README](https://github.com/zeromq/libzmq/blob/master/README.md#supported-platforms-with-primary-CI),
Android support is still DRAFT.

## Configuration

### Basics

Basically, CZMQ build for Android, relies on exported variables.

Provided build scripts can mainly be used like

    export XXX=xxx
    export YYY=yyy
    ...
    cd <czmq>/builds/android
    ./<build_script>


### Android NDK

CZMQ is tested against Android NDK versions r19 to r25.

By default, CZMQ uses NDK `android-ndk-r25`, but you can specify
a different one:

    export NDK_VERSION=android-ndk-r23c

If you already have installed your favorite NDK somewhere, all you have to
do is to export and set NDK_VERSION and ANDROID_NDK_ROOT environment
variables, e.g:

    export NDK_VERSION="android-ndk-r23b"
    export ANDROID_NDK_ROOT=$HOME/${NDK_VERSION}

**Important:** ANDROID_NDK_ROOT must be an absolute path !

If you specify only NDK_VERSION, ANDROID_NDK_ROOT will be automatically set
to its default:

    export ANDROID_NDK_ROOT=/tmp/${NDK_VERSION}

To specify the minimum SDK version set the environment variable below:

    export MIN_SDK_VERSION=21   # Default value if unset

To specify the build directory set the environment variable below:

    export ANDROID_BUILD_DIR=${HOME}/android_build

**Important:** ANDROID_BUILD_ROOT must be an absolute path !

### Android build folder

All Android libraries will be generated under:

    ${ANDROID_BUILD_DIR}/prefix/<arch>/lib

where <arch> is one of `arm`, `arm64`, `x86` or `x86_64`.

### Android build cleanup

Build and Dependency storage folders are automatically cleaned,
by `ci_build.sh`. This can be avoided with the help of

    ANDROID_BUILD_DIR="no"

If you turn this to "no", make sure to clean what has to be, before
calling `build.sh` or `ci_build.sh`.

### Prebuilt Android libraries

Android prebuilt libraries have to be stored under

    ANDROID_BUILD_DIR/prefix/<arch>/lib

Do not forget to disable [Android cleanup](#android-build-cleanup).

### Dependencies

By default, `build.sh` download dependencies under `/tmp/tmp-deps`.

You can specify another folder with the help of ANDROID_DEPENDENCIES_DIR:

   ANDROID_DEPENDENCIES_DIR=${HOME}/my_dependencies

If you place your own dependency source trees there,
do not forget to disable [Android cleanup](#android-build-cleanup).

## Build

See chapter [Configuration](#configuration) for configuration options and
other details.

Select your preferred parameters:

    export XXX=xxx
    export YYY=yyy
    ...

and run:

    cd <czmq>/builds/android
    ./build.sh [ arm | arm64 | x86 | x86_64 ]

Parameter selection and the calls to build.sh can be located in a
SHELL script, like in ci_build.sh.

## CI build

Basically, it will call `build.sh` once, for each Android target.

This script accepts the same configuration variables, but some are set
with different default values. For instance, the dependencies are not
downloaded or cloned in `/tmp/tmp-deps, but inside LIBZMQ clone.

It can be used in the same way as build.sh

    export XXX=xxx
    export YYY=yyy
    cd <czmq>/builds/android
    ./ci_build.sh


