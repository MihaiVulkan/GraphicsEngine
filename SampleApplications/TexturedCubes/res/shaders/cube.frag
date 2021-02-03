#version 450

layout (location = 0) in vec2 v_uv;

//NOTE! binding = 0 is used by the UBO in vertex shader
layout (binding = 1) uniform sampler2D samplerColor;

layout (location = 0) out vec4 outFragColor;

void main() 
{
	outFragColor = texture(samplerColor, v_uv);
}