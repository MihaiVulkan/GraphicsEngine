#version 450
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_color;

layout (std140, set = 0, binding = 0) uniform UBO 
{
	mat4 PVM;
} ubo;

layout (location = 0) out vec3 v_color;

out gl_PerVertex 
{
    vec4 gl_Position;  
};


void main() 
{
	v_color = a_color;

	gl_Position = ubo.PVM * vec4(a_position, 1.0);
}
