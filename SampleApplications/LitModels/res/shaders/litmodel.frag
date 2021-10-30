#version 450

//NOTE! binding = 0 is used by the UBO in vertex shader
layout (std140, set = 0, binding = 1) uniform UBO 
{
	vec3 lightDir;
	vec3 lightColor;
} ubo;

//NOTE! binding = 0 is used by the UBO in vertex shader
layout (set = 0, binding = 2) uniform sampler2D samplerColorMap;

layout (location = 0) in vec2 v_uv;
layout (location = 1) in vec3 v_normalWS;
layout (location = 2) in vec3 v_viewPosWS;

layout (location = 0) out vec4 outFragColor;

void main() 
{
	vec4 color = texture(samplerColorMap, v_uv);

	// Lighting calculation in WorldSpace (WS)

	// computing directions for: normal, light, view, reflection vectors
	vec3 N = normalize(v_normalWS);
	vec3 V = normalize(v_viewPosWS);
	vec3 L = normalize(ubo.lightDir);
	vec3 R = reflect(-L, N);

	vec3 diffuse = max(dot(N, L), 0.0) * color.rgb;
	vec3 specular = pow(max(dot(R, V), 0.0), 16.0) * color.a * ubo.lightColor;
	vec3 finalColor = diffuse + specular;

	outFragColor = vec4(finalColor, 1.0);

}