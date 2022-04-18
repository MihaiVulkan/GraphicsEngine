#version 450

layout (location = 0) in vec2 vUV;
layout (location = 0) out vec4 outFragColor;

layout (set = 0, binding = 0) uniform sampler2D u2DShadowTexture;

float linearizeDepth(float depth)
{
  float n = 1.0; // camera z near
  float f = 128.0; // camera z far
  float z = depth;
  return (2.0 * n) / (f + n - z * (f - n));	
}

void main() 
{
	float depth = texture(u2DShadowTexture, vUV).r;
	float linDepth = 1.0 - linearizeDepth(depth);

	outFragColor = vec4(vec3(linDepth), 1.0);
}