#version 450

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_uv;

layout (std140, set = 0, binding = 0) uniform UBO 
{
	mat4 PVM;
} ubo;

layout (location = 0) out vec4 v_clipPos;

void main() 
{
	v_clipPos = ubo.PVM * vec4(a_position, 1.0);
	gl_Position = v_clipPos;		
}
