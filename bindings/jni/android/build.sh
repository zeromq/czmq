#!/bin/bash

source ../../../builds/android/android_build_helper.sh
android_build_env

#   Ensure we've built dependencies for Android
echo "********  Building Android native libraries"
( cd ../../../builds/android && ./build.sh )

#   Ensure we've built JNI interface
echo "********  Building JNI interface & classes"
( cd .. && gradle build jar )

rm -Rf build
mkdir build
cd build
cmake -v -DCMAKE_TOOLCHAIN_FILE=../android_toolchain.cmake ..

#   CMake wrongly searches current directory and then toolchain path instead
#   of lib path for these files, so make them available temporarily
ln -s $ANDROID_NDK_ROOT/platforms/android-8/arch-arm/usr/lib/crtend_so.o
ln -s $ANDROID_NDK_ROOT/platforms/android-8/arch-arm/usr/lib/crtbegin_so.o

echo "********  Building JNI for Android"
make #VERBOSE=1
rm crtend_so.o crtbegin_so.o
cd ..

mkdir build/libs
mv build/*.so build/libs
cp ../build/libs/* build/libs
cp ../../../builds/android/prefix/*/lib/*.so build/libs

echo "********  Complete: build products are in ./build/libs"
ls build/libs
