#version 450
layout (location = 0) in vec2 v_uv;
layout (location = 1) in vec3 v_normalWS;
layout (location = 2) in vec3 v_viewPosWS;
layout (location = 3) in vec4 v_clipPos;

layout (location = 0) out vec4 outFragColor;

//NOTE! binding = 0 is used by the UBO in vertex shader
layout (std140, set = 0, binding = 1) uniform UniformBuffer 
{
	vec4 lightDir;
	vec4 lightColor;
	float roughness;
} uUBO;

//NOTE! binding = 1 is used by the uUBO here
layout (set = 0, binding = 2) uniform sampler2D u2DColorTexture;
layout (set = 0, binding = 3) uniform sampler2D u2DNormalTexture;

void main() 
{
	// projected coords used to sample from the color Render Target
	// to simulate the mirror effect
	vec2 coord = (v_clipPos.xy / v_clipPos.w) * 0.5f + 0.5f; // NDC to tex coords: [-1, 1] -> [0, 1]

	// normal texture used to wabble the mirror effect
	vec4 normal = texture(u2DNormalTexture, coord);
	vec2 offset = normal.xz * clamp(uUBO.roughness, 0.0f, 0.1f);

	vec4 color = texture(u2DColorTexture, coord + offset);

	// Lighting calculation in WorldSpace (WS)

	// computing directions for: normal, light, view, reflection vectors
	vec3 N = normalize(v_normalWS);
	vec3 V = normalize(v_viewPosWS);
	vec3 L = normalize(uUBO.lightDir.xyz);
	vec3 R = reflect(-L, N);

	float ambient = 0.2;
	float diffuse = max(dot(N, L), 0.0);
	float specular = pow(max(dot(R, V), 0.0), 16.0);
	vec3 finalColor = (ambient + diffuse) * uUBO.lightColor.rgb * color.rgb + specular * uUBO.lightColor.rgb * color.a;

	outFragColor = vec4(finalColor, 1.0);

}