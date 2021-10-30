#version 450
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;

//NOTE! First binding
layout (std140, set = 0, binding = 0) uniform UBO 
{
	mat4 PVM;
	mat4 Model;
	vec3 cameraPos;
} ubo;

layout (location = 0) out vec2 v_uv;
layout (location = 1) out vec3 v_normalWS;
layout (location = 2) out vec3 v_viewPosWS;

out gl_PerVertex 
{
    vec4 gl_Position;  
};


void main() 
{
	v_uv = a_uv;

	// Lighting calculation in WorldSpace (WS)
	vec3 posWS = vec3(ubo.Model * vec4(a_position, 1.0));

	v_normalWS = mat3(ubo.Model) * a_normal; //no actual transform, only identity matrix
	v_viewPosWS = ubo.cameraPos - posWS; // camera pos is in world space

	gl_Position = ubo.PVM * vec4(a_position, 1.0);
}
