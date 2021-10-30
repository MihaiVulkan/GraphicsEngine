#version 450

layout (location = 0) in vec3 v_uvw;

//NOTE! binding = 0 is used by the UBO in vertex shader
layout (set = 0, binding = 1) uniform samplerCube samplerCubeMap;

layout (location = 0) out vec4 outFragColor;

void main() 
{
	outFragColor = texture(samplerCubeMap, v_uvw);
}