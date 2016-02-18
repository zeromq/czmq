# czmq-jni

JNI Binding for CZMQ

## Building the JNI Layer for Linux

Ensure you have gradle and cmake installed, then run:

    gradle build jar
    gradle test

This calls javah to build the headers in src/native/include, and then compiles the C and Java pieces to create a jar file a sharable library (.so).

## Building the JNI Layer for Android

See bindings/jni/android/build.sh.

You need the Android Native Development Kit (NDK) installed.

Set these environment variables, e.g:

    ANDROID_NDK_ROOT=$HOME/android-ndk-r10e
    TOOLCHAIN_NAME=arm-linux-androideabi-4.9
    TOOLCHAIN_HOST=arm-linux-androideabi
    TOOLCHAIN_ARCH=arm
    TOOLCHAIN_PATH=$ANDROID_NDK_ROOT/toolchains/$TOOLCHAIN_NAME/prebuilt/linux-x86_64/bin

Then in the android directory, run:

    ./build.sh

This does the following:

* It compiles the CZMQ C sources for Android, into a native library libczmq.so in builds/android/
* It compiles the JNI Java classes into a jar file czmq-jni-3.0.3.jar in bindings/jni/build/libs
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

