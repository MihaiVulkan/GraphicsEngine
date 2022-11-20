#!/bin/bash

# $1 - platform: Win, Unix (Linux, Mac, Android)
# $2 - architecture/toolchain: x86, x64, arm
# $3 - build config: Debug, Release

if [ "$1" != "Win" ] && [ "$1" != "Unix" ]
then
    echo "Please specify build type: Win or Unix"
    echo "Example: ./build.sh Win 64bit Debug"
    exit 1
fi

if [ "$2" != "32bit" ] && [ "$2" != "64bit" ]
then
    echo "Please specify build type: x86 or x64"
    echo "Example: ./build.sh Unix 32bit Debug Vulkan"
    exit 1
fi

if [ "$3" != "Debug" ] && [ "$3" != "Release" ]
then
    echo "Please specify build type: Debug or Release"
    echo "Example: ./build.sh Win 64bit Release OpenGL"
    exit 1
fi

if [ "$4" != "Vulkan" ] && [ "$4" != "OpenGL" ]
then
    echo "Please specify renderer type: Vulkan or OpenGL"
    echo "Example: ./build.sh Win 64bit Release Vulkan"
    exit 1
fi

export BUILD_DIR="build_$1_$2_$3_$4"
export ARCH=$2
export BUILD_CONFIG=$3

if [ "$1" == "Win" ]
then
	export IDE="Visual Studio 16 2019"
	if [ "$2" == "32bit" ]
	then
		export VS_ARCH="Win32"
	elif [ "$2" == "64bit" ]
	then
		export VS_ARCH="x64"
	fi
fi

if [ "$5" != "-b" ] && [ "$6" != "-b" ]
then
	echo "No -b provided, so the script shall not build the code!"
fi

if [ "$5" != "-cs" ] && [ "$6" != "-cs" ]
then
	echo "No -cs provided, so the script shall not compile the shaders!"
fi

echo "Build dir: " $BUILD_DIR
echo "IDE: " $IDE
echo "Arch: " $ARCH
echo "VS Arch: " $VS_ARCH
echo "Build config: " $BUILD_CONFIG
echo "Renderer: " $4

cmake -DCMAKE_BUILD_TYPE=$BUILD_CONFIG -DRENDERER=$4 -G "$IDE" -A $VS_ARCH -S . -B $BUILD_DIR

if [ "$5" == "-b" ] || [ "$6" == "-b" ]
then
	cmake --build $BUILD_DIR --parallel 4
fi

if [ "$5" == "-cs" ] || [ "$6" == "-cs" ]
then
	./setupShaders.sh -c
fi
