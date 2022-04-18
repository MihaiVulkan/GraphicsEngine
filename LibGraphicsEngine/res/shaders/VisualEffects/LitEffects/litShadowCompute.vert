#version 450
layout (location = 0) in vec3 a_position;

//NOTE! First binding
layout (std140, set = 0, binding = 0) uniform UniformBuffer 
{
	mat4 LightPVM;
	mat4 Model;
} uUBO;

void main() 
{
	gl_Position = uUBO.LightPVM * uUBO.Model * vec4(a_position, 1.0);
}