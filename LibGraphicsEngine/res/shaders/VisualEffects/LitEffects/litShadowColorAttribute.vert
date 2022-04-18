#version 450
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec3 a_color;

layout (location = 0) out vec3 v_posWS;
layout (location = 1) out vec3 v_normalWS;
layout (location = 2) out vec3 v_viewPosWS;
layout (location = 3) out vec4 v_shadowCoordWS;
layout (location = 4) out vec3 v_color;

//NOTE! First binding
layout (std140, set = 0, binding = 0) uniform UniformBuffer 
{
	mat4 PVM;
	mat4 LightPVM;
	mat4 Model;
	mat4 Normal;
	vec4 cameraPos;
} uUBO;

const mat4 biasMat = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0
	);

void main() 
{
	v_color = a_color;

	// Lighting calculation in WorldSpace (WS)
	vec4 posWS = uUBO.Model * vec4(a_position, 1.0);
	v_posWS = posWS.xyz;

	v_normalWS = vec3(uUBO.Normal * vec4(a_normal, 0.0));
	v_viewPosWS = uUBO.cameraPos.xyz - posWS.xyz; // camera pos is in world space

	v_shadowCoordWS = biasMat * uUBO.LightPVM * posWS;

	gl_Position = uUBO.PVM * vec4(a_position, 1.0);
}