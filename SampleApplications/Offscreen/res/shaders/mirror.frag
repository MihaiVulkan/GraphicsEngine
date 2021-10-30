#version 450

layout (set = 0, binding = 1) uniform sampler2D samplerColor;
layout (set = 0, binding = 2) uniform sampler2D samplerNormal;

layout (location = 0) in vec4 v_clipPos;
layout (location = 0) out vec4 outFragColor;

void main() 
{
	// projected coords used to sample from the color Render Target
	// to simulate the mirror effect
	vec2 coord = (v_clipPos.xy / v_clipPos.w) * 0.5f + 0.5f; // projection of coords in NDC space

	// normal texture used to wabble the mirror effect
	vec4 normal = texture(samplerNormal, coord);
	vec2 offset = normal.xz * 0.03f;

	outFragColor = texture(samplerColor, coord + offset);
}