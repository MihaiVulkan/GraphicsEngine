#version 450
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_color;

layout (binding = 0) uniform UBO 
{
	mat4 PVM;
} ubo;

layout (location = 0) out vec3 v_color;

out gl_PerVertex 
{
    vec4 gl_Position;  
	float gl_PointSize;
};


void main() 
{
	gl_PointSize = 10;

	v_color = a_color;

	gl_Position = ubo.PVM * vec4(a_position.xyz, 1.0);
}
