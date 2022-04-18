#version 450
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_color;

layout (location = 0) out vec3 v_color;

layout (std140, set = 0, binding = 0) uniform UniformBuffer 
{
	mat4 PVM;
} uUBO;

void main() 
{
	v_color = a_color;

	gl_Position = uUBO.PVM * vec4(a_position, 1.0);
}
