#version 450
layout (location = 0) in vec2 v_uv;
layout (location = 1) in vec3 v_normalWS;
layout (location = 2) in vec3 v_viewPosWS;

layout (location = 0) out vec4 outFragColor;

//NOTE! binding = 0 is used by the UBO in vertex shader
layout (std140, set = 0, binding = 1) uniform UniformBuffer 
{
	vec4 lightDir;
	vec4 lightColor;
} uUBO;

//NOTE! binding = 1 is used by the uUBO here
layout (set = 0, binding = 2) uniform samplerCube uCubemapTexture;

void main() 
{
	// Environment reflection in WorldSpace (WS)

	vec3 N = normalize(v_normalWS);

	// Environment mapping
	vec3 cI = normalize(- v_viewPosWS);
	const float ratio = 1.00 / 1.52; // air / glass ratio
    vec3 cR = refract(cI, N, ratio);

	vec4 color = texture(uCubemapTexture, cR);

	// Lighting calculation in WorldSpace (WS)

	// computing directions for: normal, light, view, reflection vectors
	vec3 V = normalize(v_viewPosWS);
	vec3 L = normalize(uUBO.lightDir.xyz);
	vec3 R = reflect(-L, N);

	float ambient = 1.0;
	float diffuse = max(dot(N, L), 0.0) * 0.3;
	float specular = pow(max(dot(R, V), 0.0), 16.0);
	vec3 finalColor = (ambient + diffuse + specular) * uUBO.lightColor.rgb * color.rgb;

	outFragColor = vec4(finalColor, 1.0);

}