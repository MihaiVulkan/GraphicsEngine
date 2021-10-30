#version 450
layout (location = 0) in vec3 a_position;

//NOTE! First binding
layout (std140, set = 0, binding = 0) uniform UBO 
{
	mat4 PVM;
} ubo;

layout (location = 0) out vec3 v_uvw;

out gl_PerVertex 
{
    vec4 gl_Position;  
};


void main() 
{
	v_uvw = a_position;

	gl_Position = ubo.PVM * vec4(a_position, 1.0);
}
