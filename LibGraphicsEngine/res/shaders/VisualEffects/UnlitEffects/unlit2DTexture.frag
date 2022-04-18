#version 450
layout (location = 0) in vec2 v_uv;

layout (location = 0) out vec4 outFragColor;

//NOTE! binding = 0 is used by the UBO in vertex shader
layout (set = 0, binding = 1) uniform sampler2D u2DTexture;

void main() 
{
	outFragColor = texture(u2DTexture, v_uv);
}