#version 450

layout (location = 0) in vec2 v_uv;
layout (location = 1) flat in float v_crrTime;

//NOTE! binding = 0 is used by the UBO in vertex shader
layout (set = 0, binding = 1) uniform sampler2DArray samplerArray;

layout (location = 0) out vec4 outFragColor;

void main() 
{
	ivec3 size = textureSize(samplerArray, 0);

	outFragColor = texture(samplerArray, vec3(v_uv, mod(v_crrTime, float(size.z))));

}