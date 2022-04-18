#version 450
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_uv;

layout (location = 0) out vec3 v_uvw;

//NOTE! First binding
layout (std140, set = 0, binding = 0) uniform UniformBuffer 
{
	mat4 PVM;
} uUBO;

void main() 
{
	v_uvw = vec3(a_uv, float(gl_VertexIndex));

	gl_Position = uUBO.PVM * vec4(a_position, 1.0);
}
