#version 450
layout (location = 0) in vec3 v_posWS;
layout (location = 1) in vec3 v_normalWS;
layout (location = 2) in vec3 v_viewPosWS;
layout (location = 3) in vec4 v_shadowCoordWS;
layout (location = 4) in vec3 v_color;

layout (location = 0) out vec4 outFragColor;

//NOTE! binding = 0 is used by the UBO in vertex shader
layout (std140, set = 0, binding = 1) uniform UniformBuffer 
{
	vec4 lightPos;
	vec4 lightColor;
} uUBO;

layout (set = 0, binding = 2) uniform sampler2D u2DShadowTexture;


#define AMBIENT 0.2

float textureProj(vec4 shadowCoord, vec2 off)
{
	float shadow = 1.0;
	if ( shadowCoord.z > -1.0 && shadowCoord.z < 1.0 ) 
	{
		float dist = texture(u2DShadowTexture, shadowCoord.st + off).r;
		if (shadowCoord.w > 0.0 && dist < shadowCoord.z) 
		{
			shadow = AMBIENT;
		}
	}
	return shadow;
}

#define PCF_SCALE 1.5

float filterPCF(vec4 sc)
{
	ivec2 texDim = textureSize(u2DShadowTexture, 0);
	float dx = PCF_SCALE * 1.0 / float(texDim.x);
	float dy = PCF_SCALE * 1.0 / float(texDim.y);

	float shadowFactor = 0.0;
	int count = 0;
	const int range = 1;
	for (int x = -range; x <= range; x++)
	{
		for (int y = -range; y <= range; y++)
		{
			shadowFactor += textureProj(sc, vec2(dx * x, dy * y));
			count ++;
		}
	}

	return shadowFactor / count;
}

void main() 
{
	// Lighting calculation in WorldSpace (WS)

	// computing directions for: normal, light, view, reflection vectors
	vec3 N = normalize(v_normalWS);
	vec3 V = normalize(v_viewPosWS);
	vec3 L = normalize(uUBO.lightPos.xyz - v_posWS);
	vec3 R = reflect(-L, N);

	//float ambient = 0.2;
	float diffuse = max(dot(N, L), 0.0);
	float specular = pow(max(dot(R, V), 0.0), 16.0);

	// shadow calculation in WorldSpace (WS)
	float shadow = filterPCF(v_shadowCoordWS / v_shadowCoordWS.w);

	vec3 finalColor = (AMBIENT + diffuse * shadow) * uUBO.lightColor.rgb * v_color + specular * uUBO.lightColor.rgb;

	outFragColor = vec4(finalColor, 1.0);
}