#version 450
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_uv;

//NOTE! First binding
layout (set = 0, binding = 0) uniform UBO 
{
	mat4 PVM;
} ubo;

layout (location = 0) out vec2 v_uv;

out gl_PerVertex 
{
    vec4 gl_Position;  
};


void main() 
{
	v_uv = a_uv;

	gl_Position = ubo.PVM * vec4(a_position, 1.0);
}
