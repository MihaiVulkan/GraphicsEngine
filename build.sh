#!/bin/bash

if [ "$1" != "Debug" ] && [ "$1" != "Release" ]
then
    echo "Please specify build type: Debug or Release"
    echo "Example: ./build.sh Debug"
    exit 1
fi

export BUILD_DIR="build_$1"

cmake -DCMAKE_BUILD_TYPE="$1" -G "Visual Studio 16 2019" -A x64 -S . -B $BUILD_DIR
cmake --build $BUILD_DIR --parallel 4