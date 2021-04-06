#version 450

layout (location = 0) in vec3 v_normal;

layout (location = 0) out vec4 outFragColor;

void main() 
{
	// we generate a simple color based on normal
	vec3 color = 5 * v_normal * vec3(0.1f, 0.7f, 0.5f);

	outFragColor = vec4(color, 1.0);
}