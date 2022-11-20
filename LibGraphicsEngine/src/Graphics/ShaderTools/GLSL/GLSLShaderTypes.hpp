#ifndef GRAPHICS_SHADER_TOOLS_GLSL_GLSL_SHADER_TYPES_HPP
#define GRAPHICS_SHADER_TOOLS_GLSL_GLSL_SHADER_TYPES_HPP

#include "Foundation/TypeDefines.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		namespace GLSLShaderTypes
		{
			namespace Constants
			{
				// glsl data types
				constexpr const char_t* BOOL_TYPE = "bool";
				constexpr const char_t* INT_TYPE = "int";
				constexpr const char_t* FLOAT_TYPE = "float";
				constexpr const char_t* VEC2_TYPE = "vec2";
				constexpr const char_t* VEC3_TYPE = "vec3";
				constexpr const char_t* VEC4_TYPE = "vec4";
				constexpr const char_t* MAT2_TYPE = "mat2";
				constexpr const char_t* MAT3_TYPE = "mat3";
				constexpr const char_t* MAT4_TYPE = "mat4";
				constexpr const char_t* SAMPLER_2D_TYPE = "sampler2D";
				constexpr const char_t* SAMPLER_2D_ARRAY_TYPE = "sampler2DArray";
				constexpr const char_t* SAMPLER_CUBEMAP_TYPE = "samplerCube";

				// vertex attributes
				constexpr const char_t* VERTEX_POSITION = "a_position";
				constexpr const char_t* VERTEX_NORMAL = "a_normal";
				constexpr const char_t* VERTEX_TANGET = "a_tangent";
				constexpr const char_t* VERTEX_COLOR = "a_color";
				constexpr const char_t* VERTEX_TEX_COORDS = "a_uv";

				// UBO
				constexpr const char_t* UBO = "uUBO";

				// Uniforms
				constexpr const char_t* UNIFORM_PVM_MATRIX = "PVM"; // no 'u' in front of the name as it is part of UBO
				constexpr const char_t* UNIFORM_LIGHT_PVM_MATRIX = "LightPVM"; // no 'u' in front of the name as it is part of UBO
				constexpr const char_t* UNIFORM_MODEL_MATRIX = "Model"; // no 'u' in front of the name as it is part of UBO
				constexpr const char_t* UNIFORM_NORMAL_MATRIX = "Normal"; // no 'u' in front of the name as it is part of UBO
				constexpr const char_t* UNIFORM_CAMERA_POS = "cameraPos"; // no 'u' in front of the name as it is part of UBO
				constexpr const char_t* UNIFORM_COLOR = "color"; // no 'u' in front of the name as it is part of UBO
				constexpr const char_t* UNIFORM_ROUGHNESS = "roughness"; // no 'u' in front of the name as it is part of UBO
				constexpr const char_t* UNIFORM_LIGHT_DIR = "lightDir"; // no 'u' in front of the name as it is part of UBO
				constexpr const char_t* UNIFORM_LIGHT_POS = "lightPos"; // no 'u' in front of the name as it is part of UBO
				constexpr const char_t* UNIFORM_LIGHT_COLOR = "lightColor"; // no 'u' in front of the name as it is part of UBO
				constexpr const char_t* UNIFORM_IS_GL_NDK = "isGLNDK"; // no 'u' in front of the name as it is part of UBO

				constexpr const char_t* UNIFORM_2D_TEXTURE = "u2DTexture";
				constexpr const char_t* UNIFORM_2D_COLOR_TEXTURE = "u2DColorTexture";
				constexpr const char_t* UNIFORM_2D_NORMAL_TEXTURE = "u2DNormalTexture";
				constexpr const char_t* UNIFORM_2D_TEXTURE_ARRAY = "u2DTextureArray";
				constexpr const char_t* UNIFORM_2D_SHADOW_TEXTURE = "u2DShadowTexture";
				constexpr const char_t* UNIFORM_CUBEMAP_TEXTURE = "uCubemapTexture";
			}

			enum class UniformType : uint8_t
			{
				// order uniforms by type as to get the best alignment in the shaders according to sdt140 memory layout rules
				// we avoid mat3, vec3 types becauase of their alignment and padding potential issues

				// mat4
				GE_UT_PVM_MATRIX4 = 0,
				GE_UT_PV_CUBEMAP_MATRIX4, // special case for cubemaps, translation is removed
				GE_UT_LIGHT_PVM_MATRIX4,
				GE_UT_PROJECTION_MATRIX4,
				GE_UT_VIEW_MATRIX4,
				GE_UT_MODEL_MATRIX4,
				GE_UT_NORMAL_MATRIX4,

				// vec4 
				GE_UT_CAMERA_POS,  //viewPos
				GE_UT_LIGHT_DIR,
				GE_UT_LIGHT_POS,
				GE_UT_LIGHT_COLOR,
				GE_UT_COLOR_VEC4,

				// floats
				GE_UT_CRR_TIME, // timer crr time
				GE_UT_ROUGHNESS, // surface roughness

				// GL NDK detection
				GE_UT_IS_GL_NDK,

				// others
				GE_UT_COUNT
			};
		}
	}
}


#endif // GRAPHICS_SHADER_TOOLS_GLSL_GLSL_SHADER_TYPES_HPP