#version 450
layout (location = 0) in vec3 v_uvw;

layout (location = 0) out vec4 outFragColor;

//NOTE! binding = 0 is used by the UBO in vertex shader
layout (set = 0, binding = 1) uniform samplerCube uCubemapTexture;

void main() 
{
	outFragColor = texture(uCubemapTexture, v_uvw);
}