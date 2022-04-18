#version 450
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;

layout (location = 0) out vec3 v_normalWS;
layout (location = 1) out vec3 v_viewPosWS;

//NOTE! First binding
layout (std140, set = 0, binding = 0) uniform UniformBuffer 
{
	mat4 PVM;
	mat4 Model;
	mat4 Normal;
	vec4 cameraPos;
} uUBO;

void main() 
{
	// Lighting calculation in WorldSpace (WS)
	vec3 posWS = vec3(uUBO.Model * vec4(a_position, 1.0));

	v_normalWS = vec3(uUBO.Normal * vec4(a_normal, 0.0));
	v_viewPosWS = uUBO.cameraPos.xyz - posWS; // camera pos is in world space

	gl_Position = uUBO.PVM * vec4(a_position, 1.0);
}
