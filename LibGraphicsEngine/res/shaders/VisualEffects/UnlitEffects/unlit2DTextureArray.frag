#version 450
layout (location = 0) in vec3 v_uvw;

layout (location = 0) out vec4 outFragColor;

//NOTE! binding = 0 is used by the UBO in vertex shader
layout (set = 0, binding = 1) uniform sampler2DArray u2DTextureArray;

void main() 
{
	ivec3 size = textureSize(u2DTextureArray, 0);

	outFragColor = texture(u2DTextureArray, vec3(v_uvw.xy, float(mod(v_uvw.z, size.z))));
}