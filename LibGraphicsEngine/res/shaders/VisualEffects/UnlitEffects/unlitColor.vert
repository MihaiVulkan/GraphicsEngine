#version 450
layout (location = 0) in vec3 a_position;

layout (std140, set = 0, binding = 0) uniform UniformBuffer 
{
	mat4 PVM;
} uUBO;

void main() 
{
	gl_Position = uUBO.PVM * vec4(a_position, 1.0);
}
