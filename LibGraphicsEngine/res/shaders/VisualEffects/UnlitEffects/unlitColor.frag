#version 450
layout (location = 0) out vec4 outFragColor;

//NOTE! binding = 0 is used by the UBO in vertex shader
layout (std140, set = 0, binding = 1) uniform UniformBuffer 
{
	vec4 color;
} uUBO;

void main() 
{
	outFragColor = uUBO.color;
}