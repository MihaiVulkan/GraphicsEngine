#!/bin/bash

# All GLSL shaders are compiled to SPIR-V binary format
echo "All GLSL shaders are compiled to SPIR-V binary format"

if [ "$1" != "-c" ] && [ "$1" != "-r" ]
then
	echo "No -c provided, so the script shall not compile the shaders!"
	echo "No -r provided, so the script shall not delete the binary *.spv shaders!"
	exit 1
fi

export GLSL_COMPILER=./glslc
export SHADER_EXT=".vert .frag"
export SPIRV_EXT=".spv"

echo "Supported shder ext: " $SHADER_EXT
echo "SPIR-V ext: " $SPIRV_EXT

if [ "$1" == "-c" ]
then
echo "Compiling shaders ..."
	for ext in $SHADER_EXT
	do
		find . -type f -name *$ext -exec $GLSL_COMPILER {} -o {}$SPIRV_EXT \;
	done
elif [ "$1" == "-r" ]
then
	echo "Removing binary *.spv shaders ..."
	for ext in $SPIRV_EXT
	do
		find . -type f -name *$ext -exec rm {} \;
	done
fi