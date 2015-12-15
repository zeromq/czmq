set (CMAKE_SYSTEM_NAME Linux)  # Tell CMake we're cross-compiling
set (ANDROID_NDK_ROOT $ENV{ANDROID_NDK_ROOT})
include (CMakeForceCompiler)

# where is the target environment
set (ANDROID_TOOLCHAIN_ROOT ${ANDROID_NDK_ROOT}/toolchains/$ENV{TOOLCHAIN_NAME}/prebuilt/linux-x86_64)
set (ANDROID_TOOLCHAIN_MACHINE_NAME $ENV{TOOLCHAIN_HOST})

# api level see doc https://github.com/taka-no-me/android-cmake
set (ANDROID_NATIVE_API_LEVEL android-8)
set (ANDROID_ARCH_NAME arm)
set (ANDROID_SYSROOT "${ANDROID_NDK_ROOT}/platforms/android-${ANDROID_NATIVE_API_LEVEL}/arch-${ANDROID_ARCH_NAME}" )
set (CMAKE_INSTALL_PREFIX /tmp)
set (CMAKE_FIND_ROOT_PATH "${ANDROID_TOOLCHAIN_ROOT}/bin" "${ANDROID_TOOLCHAIN_ROOT}/${ANDROID_TOOLCHAIN_MACHINE_NAME}" "${ANDROID_SYSROOT}" "${CMAKE_INSTALL_PREFIX}" "${CMAKE_INSTALL_PREFIX}/share")

# Prefix detection only works with compiler id "GNU"
# CMake will look for prefixed g++, cpp, ld, etc. automatically
CMAKE_FORCE_C_COMPILER (${ANDROID_TOOLCHAIN_ROOT}/bin/arm-linux-androideabi-gcc GNU)
cmake_policy (SET CMP0015 NEW)
set (ANDROID True)
set (BUILD_ANDROID True)

#   Find location of ZeroMQ header file
include (FindPkgConfig)
pkg_check_modules (PC_ZEROMQ "libzmq")
if (NOT PC_ZEROMQ_FOUND)
    pkg_check_modules(PC_ZEROMQ "zmq")
endif (NOT PC_ZEROMQ_FOUND)

if (PC_ZEROMQ_FOUND)
    set (PC_ZEROMQ_INCLUDE_HINTS ${PC_ZEROMQ_INCLUDE_DIRS} ${PC_ZEROMQ_INCLUDE_DIRS}/*)
endif (PC_ZEROMQ_FOUND)
find_path (ZEROMQ_INCLUDE_DIR NAMES zmq.h HINTS ${PC_ZEROMQ_INCLUDE_HINTS})

include_directories (
    ${ZEROMQ_INCLUDE_DIR}
    ../../../include
    ../../../bindings/jni/src/native/include
    ../../../builds/android/prefix/arm-linux-androideabi-4.9/include
    ${ANDROID_NDK_ROOT}/platforms/android-8/arch-arm/usr/include
)
link_directories (
    ../../../builds/android/prefix/arm-linux-androideabi-4.9/lib
    ${ANDROID_NDK_ROOT}/platforms/android-8/arch-arm/usr/lib
)
