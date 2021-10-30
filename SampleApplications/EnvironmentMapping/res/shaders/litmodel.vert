#version 450
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;

//NOTE! First binding
layout (std140, set = 0, binding = 0) uniform UBO 
{
	mat4 PVM;
	mat4 Model;
	mat4 Normal;
	vec3 cameraPos;
} ubo;

layout (location = 0) out vec3 v_normalWS;
layout (location = 1) out vec3 v_viewPosWS;

out gl_PerVertex 
{
    vec4 gl_Position;  
};


void main() 
{
	// Lighting calculation in WorldSpace (WS)
	vec3 posWS = vec3(ubo.Model * vec4(a_position, 1.0));

	//v_normalWS = ubo.Normal * a_normal;
	v_normalWS = vec3(ubo.Normal * vec4(a_normal, 0.0));
	v_viewPosWS = ubo.cameraPos - posWS; // camera pos is in world space

	gl_Position = ubo.PVM * vec4(a_position, 1.0);
}
