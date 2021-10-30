#version 450
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec3 a_color;

//NOTE! First binding
layout (std140, set = 0, binding = 0) uniform UBO 
{
	mat4 PVM;
	vec3 cameraPos;
} ubo;

layout (location = 0) out vec3 v_normalWS;
layout (location = 1) out vec3 v_viewPosWS;
layout (location = 2) out vec3 v_color;

out gl_PerVertex 
{
    vec4 gl_Position;  
};


void main() 
{
	// Lighting calculation in WorldSpace (WS)
	v_color = a_color;
	v_normalWS = a_normal;
	v_viewPosWS = ubo.cameraPos - a_position; // camera pos is in world space

	gl_Position = ubo.PVM * vec4(a_position, 1.0);
}
