#version 450
layout (location = 0) in vec3 v_posWS;
layout (location = 1) in vec3 v_normalWS;
layout (location = 2) in vec3 v_viewPosWS;
layout (location = 3) in vec4 v_shadowCoordWS;
layout (location = 4) in vec3 v_color;
layout (location = 5) flat in int v_isGLNDK;

layout (location = 0) out vec4 outFragColor;

//NOTE! binding = 0 is used by the UBO in vertex shader
layout (std140, set = 0, binding = 1) uniform UniformBuffer 
{
	vec4 lightPos;
	vec4 lightColor;
} uUBOLight;
// a diferent name for this UBO compared to vertes shader 
// as OpenGL doesn't allow the same UBO name across shader stages even if Vulkan does !!!

layout (set = 0, binding = 2) uniform sampler2D u2DShadowTexture;


#define AMBIENT 0.2
#define PCF_SCALE 1.5

float computeShadow(vec4 shadowCoords)
{
	// compute depth of current fragment from light's perspective

	float shadowFactor = 0.0;

	if (shadowCoords.z > 1.0)
        return 0.0;

    // check whether current frag pos is in shadow

	// do a PCF filtering to get a more smooth shadow per object
	ivec2 texelSize = textureSize(u2DShadowTexture, 0);
	float dx = PCF_SCALE * 1.0 / float(texelSize.x);
	float dy = PCF_SCALE * 1.0 / float(texelSize.y);

	int count = 0;
	const int range = 1;
	for (int x = -range; x <= range; x++)
	{
		for (int y = -range; y <= range; y++)
		{
			float pcfDepth = texture(u2DShadowTexture, shadowCoords.xy + vec2(dx * x, dy * y)).r;
			if (v_isGLNDK == 1)
			{
				// small bias - to avoid shadow acnee
				const float bias = 0.0005;
				shadowFactor += 1.0 - (shadowCoords.z - bias > pcfDepth ? 1.0 : 0.0);
			}
			else
			{
				if (shadowCoords.w > 0.0 && pcfDepth < shadowCoords.z) 
				{
					pcfDepth = AMBIENT;
				}
				shadowFactor += pcfDepth;
			}
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
	vec3 L = normalize(uUBOLight.lightPos.xyz - v_posWS);
	vec3 R = reflect(-L, N);

	float diffuse = max(dot(N, L), 0.0);
	float specular = pow(max(dot(R, V), 0.0), 16.0);

	// shadow calculation in WorldSpace (WS)
	vec4 shadowCoords = v_shadowCoordWS;
	// perspective division is important if a perspective proj matrix has been used, in case of ortho matrix it does nothing
	shadowCoords = shadowCoords / shadowCoords.w;

	// Vulkan NDK is in [0, 1], OpenGL NDK is in [-1, 1]
	// we need the shadow coords in [0, 1] texture space
	// in case of OpenGL we transform the coords in [-1, 1] to [0, 1]
	if (v_isGLNDK == 1)
	{
		shadowCoords = shadowCoords * 0.5 + 0.5;
	}
	float shadow = computeShadow(shadowCoords);

	vec3 finalColor = (AMBIENT + diffuse * shadow) * uUBOLight.lightColor.rgb * v_color + specular * uUBOLight.lightColor.rgb;

	outFragColor = vec4(finalColor, 1.0);
}