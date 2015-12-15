# Building JNI for Android

We use a shell script (build.sh) that calls CMake with a toolchain:

* build.sh - main shell script
* CMakeLists.txt - general CMake configuration for project
* android_toolchain.cmake - specifics for building for Android

You must have the Android NDK installed. The script targets API level 8 and ARM architecture.

Requires these environment variables be set, e.g.:

* ANDROID_NDK_ROOT=$HOME/android-ndk-r10e
* TOOLCHAIN_NAME=arm-linux-androideabi-4.9
* TOOLCHAIN_HOST=arm-linux-androideabi
* TOOLCHAIN_ARCH=arm
* TOOLCHAIN_PATH=$ANDROID_NDK_ROOT/toolchains/$TOOLCHAIN_NAME/prebuilt/linux-x86_64/bin

To debug this, remove the comment before VERBOSE=1, so you see each compile and link command.
