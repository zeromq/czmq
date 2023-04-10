# czmq-jni

JNI Binding for CZMQ

## Preamble

As stated in LIBZMQ documentation, Android build systems are still DRAFT.

[ZActor & ZLoop](https://github.com/zeromq/czmq/issues/2214) are not (yet ?) supported.
It's also probably the case for a few other features.

This being said, CZMQ can already be used for Android.


## Prerequisites

GRADLE need to be installed on your system.

Note also that GRADLE requires CMake 3.6. For old distributions, this
may mean an upgrade of CMake. This can do done from sources and is rather
easy to rebuild/install though (tested on CentOS 7, Fedora 24, ...)


## Building the JNI Layer for Linux and OSX

Before you start make sure CZMQ is built and installed on your system.

Next, ensure you have gradle and cmake installed, then run:

    gradle build jar
    gradle test

If you don't like to install gradle beforehand simply use the gradle wrapper.

    ./gradlew build jar
    ./gradlew test

This does the following:

* It calls javah to build the headers in src/native/include
* It compiles the C and Java classes
* It creates a jar file and a shareable native library

If libraries of dependent projects are not installed in any of the default locations of your OS use parameter `buildPrefix` to point to their location, e.g.:

    ./gradlew build jar -PbuildPrefix=/tmp/jni_build

## Building the JNI Layer for Android

### Manual build

Before you start make sure that you've built the JNI Layer for your current OS.

Please read the preamble section of the [README](../../builds/android/README.md) in the android build directory.

You only need to set the environment variables.

Then in the jni's android directory (czmq-jni/android), run:

    export XXX=xxx
    export YYY=yyy
    cd <czmq>/bindings/jni/czmq-jni/android
    ./build.sh [ arm | arm64 | x86 | x86_64 ]

This does the following:

* It compiles the CZMQ C sources for Android, into a native library libczmq.so in /tmp/android_build/<architecture>/lib
* It compiles the JNI Java classes into a jar file czmq-jni-4.2.2.jar in bindings/jni/czmq-jni/build/libs
* It compiles the JNI C sources for Android, into a native library libczmqjni.so.
* It combines all these into jar file for the built architecture, which you can use in your Android projects.
* It merges the jar files built for the different architectures into one jar file.


### More automated build mecanism

You may also use `bindings/jni/ci_build.sh`:

    export XXX=xxx
    export YYY=yyy
    ./ci_build.sh

Basically, this script builds the whole for JAVA and but also for Android,
but generated libraries are available in a different place:

* bindings/jni/.deps         # all required dependencies

* bindings/jni/.build/       # all generated native libraries

* bindings/jni/.build/prefix # all generated android libraries


If you have your own `prebuilt` Android libraries, place them under

* bindings/jni/.build/prefix/{arm,arm6,x86,x86_64}/lib/.

They will be automatically packed in generated JAR files.


### Compatibility

This build system is tested on a few recent distributions:

* CentOS 7 (see [PREREQUISITES](#prerequisites)

* Fedora (24 to 37 and see [PREREQUISITES](#prerequisites)

* Rocky Linux (8 & 9)

* Debian (9 to 11)

* Ubuntu (16, 18, 20 & 22.04)

Both build systems (`build.sh` and `ci_build.sh`) are tested with NDK 19 to 25,
with current default of `android-ndk-25`.


### Configuration

Both come with many different configuration possibilities.
Again, refer to [builds/android/README](../../builds/android/README.md) for details.


## Building the JNI Layer for Windows

Prerequisites:
* MS Visual Studio or MS Visual Studio Tools 2010 or later are installed
* Java JDK 8 or later is installed

Environment Variables:
* Add MSBuild.exe to the PATH (e.g. `C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\MSBuild\Current\Bin`)
* Set JAVA_HOME to the installation location (e.g. `C:\Program Files\Java\jdk1.8.0_66`).

1. Check out all dependent projects from github, at the same level as this project (e.g. libzmq, czmq).
2. Follow the dependent projects instructions to build their `.dll` and `.lib` file.

If you used cmake to install the dependencies you can skip the following steps.

3. Create a folder where to place the dlls and libs (e.g. `C:\tmp\deps`).
4. Copy all dependent `.dll` files to the `bin` subfolder (e.g. `C:\tmp\deps\bin`)
5. Copy all dependent `.lib` files to the `lib` subfolder (e.g. `C:\tmp\deps\lib`)
6. Copy all dependent `.h` files to the `include` subfolder (e.g. `C:\tmpdeps\include`)

Now run:

    gradlew build jar -PbuildPrefix=C:\tmp\deps
    gradlew test -PbuildPrefix=C:\tmp\deps


## Installing the JNI Layer

If you like to use this JNI Layer in another project you'll need to distribute it
to a location where the other project can locate it. The easiest way to do this
is by leveraging maven and install to the local maven repository located at
$HOME/.m2. Therefore simply run:

    ./gradlew publishToMavenLocal

By default the JNI Layer builds SNAPSHOT versions (e.g. 1.1.3-SNAPSHOT). If you
like to build a release version you need the set the release switch:

    ./gradlew publishToMavenLocal -PisRelease


## Using the JNI API

- to be written.


## License


Copyright (c) the Contributors as noted in the AUTHORS file.
This file is part of CZMQ, the high-level C binding for 0MQ:
http://czmq.zeromq.org.

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.


## Information for maintainers

BINTRAY is no more accepting PUBLISH. Hence, this chapter has to be reviewed.

See [CZMQ issue #2249](https://github.com/zeromq/czmq/issues/2249) and probably a few others.


### Create or update the gradle wrapper

The gradle wrapper is a tool that allows to use gradle on multiple platforms
without installing it beforehand. Make sure you have installed a version of
gradle that is at least the version the wrapper should have (local version >=  wrapper version).
Then just run

    gradle wrapper

Now commit all generated files to the project. Yes the jar file as well! Users
will now be able to call the gradle wrapper (gradlew) which will install gradle
for them.


### Travis build

Travis can build and check this jni layer there add the following line to your
travis environment matrix

    - BUILD_TYPE=bindings BINDING=jni


### Deploy to bintray with Travis CI

When tagging a release travis can automatically deploy this jni layer to bintray.
Therefore you'll need to supply travis with three environment variables:

* BINTRAY_USER - your personal user name
* BINTRAY_KEY - your personal api key
* BINTRAY_USER_ORG - the organisation you like to publish to

You may extent .travis.yml as follows

    - BUILD_TYPE=bindings BINDING=jni BINTRAY_USER=<user> BINTRAY_KEY=<key> BINTRAY_USER_ORG=<org>

But I recommend to encrypt your bintray api key. This can be done with the
travis commandline client

    travis encrypt BINTRAY_KEY=123...

Please be aware that secure environmental variables can only be added as global.

    global:
       - secure: "ZMvDhR..."
    matrix:
       - BUILD_TYPE=bindings BINDING=jni BINTRAY_USER=<user> BINTRAY_USER_ORG=<org>

