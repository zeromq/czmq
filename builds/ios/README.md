# iOS Build

## Prerequisites

The build script require to be run on MacOs with XCode and the developer SDK installed.

This project is tested against SDK 15.5.

If you want to specify another version you need to set the environment variable below:

    export SDK_VERSION=15.5

You can list all the versions of the SDK installed on your Mac using the command below:

    xcodebuild -showsdks

## Build

In the ios directory, run:
    ./build.sh [ iPhoneOS armv7 | iPhoneOS armv7s | iPhoneOS arm64 | iPhoneSimulator i386 | iPhoneSimulator x86_64 ]

Note that certain target architectures may or may not be available depending on your target SDK Version. For example, iOS 10 is the maximum deployment target for 32-bit targets.

[This website](https://docs.elementscompiler.com/Platforms/Cocoa/CpuArchitectures/) can help you choose which architecture you need to target depending on your SDK version.
