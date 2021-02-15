#!/bin/bash

# All GLSL shaders are compiled to SPIR-V binary format
echo "All GLSL shaders are compiled to SPIR-V binary format"

export GLSL_COMPILER=./glslc
export SHADER_EXT=".vert .frag"
export SPIRV_EXT=".spv"

echo "Supported shder ext: " $SHADER_EXT
echo "SPIR-V ext: " $SPIRV_EXT

for ext in $SHADER_EXT
do
	find . -type f -name *$ext -exec $GLSL_COMPILER {} -o {}$SPIRV_EXT \;
done