# czmq-jni

[ ![Download](https://api.bintray.com/packages/zeromq/maven/czmq-jni/images/download.svg) ](https://bintray.com/zeromq/maven/czmq-jni/_latestVersion)

JNI Binding for CZMQ

## Building the JNI Layer for Linux

Ensure you have gradle and cmake installed, then run:

    gradle build jar
    gradle test

If you don't like to install gradle beforehand just use the gradle wrapper.

    ./gradlew build jar
    ./gradlew test

This calls javah to build the headers in src/native/include, and then compiles the C and Java pieces to create a jar file a sharable library (.so).

## Installing the JNI Layer for Linux

If you like to use this JNI Layer in another project you'll need to distribute it
to a location where the other project can locate it. The easiest way to do this
is by leveraging maven and install to the local maven repository located at
$HOME/.m2. Therefore simply run:

    ./gradlew publishToMavenLocal

## Building the JNI Layer for Android

See bindings/jni/android/build.sh.

You need the Android Native Development Kit (NDK) installed.

Set these environment variables, e.g:

    ANDROID_NDK_ROOT=$HOME/android-ndk-r11c
    TOOLCHAIN_VERSION=4.9
    TOOLCHAIN_HOST=arm-linux-androideabi
    TOOLCHAIN_NAME=$TOOLCHAIN_HOST-$TOOLCHAIN_VERSION
    TOOLCHAIN_ARCH=arm
    TOOLCHAIN_PATH=$ANDROID_NDK_ROOT/toolchains/$TOOLCHAIN_NAME/prebuilt/linux-x86_64/bin

Then in the android directory, run:

    ./build.sh

This does the following:

* It compiles the CZMQ C sources for Android, into a native library libczmq.so in builds/android/
* It compiles the JNI Java classes into a jar file czmq-jni-4.1.0.jar in bindings/jni/build/libs
* It compiles the JNI C sources for Android, into a native library libczmqjni.so.
* It combines all these into czmq-android.jar, which you can use in your Android projects.

## Building the JNI Layer for Windows

You need MS Visual Studio 2010 or later.

You need the Java SDK. Set the JAVA_HOME environment to the installation location, e.g. C:Program FilesJavajdk1.8.0_66.

1. Check out all dependent projects from github, at the same level as this project. E.g.: libzmq, czmq.
2. In each project, open a console in builds/msvc/vs2010 and run the build.bat batch file.
3. In this project, open a console in bindings/jni/msvc/vs2010 and run the build.bat batch file.

The resulting libraries (czmqjni.dll, czmqjni.lib) are created in bindings/jni/msvc/bin.

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

### Building the gradle wrapper

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

### Travis deploy to bintray

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

