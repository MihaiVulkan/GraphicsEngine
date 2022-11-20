#version 450
layout (location = 0) in vec3 v_normalWS;
layout (location = 1) in vec3 v_viewPosWS;

layout (location = 0) out vec4 outFragColor;

//NOTE! binding = 0 is used by the UBO in vertex shader
layout (std140, set = 0, binding = 1) uniform UniformBuffer 
{
	vec4 lightDir;
	vec4 lightColor;
	vec4 color;
} uUBOLight;
// a diferent name for this UBO compared to vertes shader 
// as OpenGL doesn't allow the same UBO name across shader stages even if Vulkan does !!!

void main() 
{
	// Lighting calculation in WorldSpace (WS)

	// computing directions for: normal, light, view, reflection vectors
	vec3 N = normalize(v_normalWS);
	vec3 V = normalize(v_viewPosWS);
	vec3 L = normalize(uUBOLight.lightDir.xyz);
	vec3 R = reflect(-L, N);

	float ambient = 0.2;
	float diffuse = max(dot(N, L), 0.0);
	float specular = pow(max(dot(R, V), 0.0), 16.0);
	vec3 finalColor = (ambient + diffuse) * uUBOLight.lightColor.rgb * uUBOLight.color.rgb + specular * uUBOLight.lightColor.rgb * uUBOLight.color.a;

	outFragColor = vec4(finalColor, 1.0);

}