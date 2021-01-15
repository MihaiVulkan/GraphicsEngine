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
				// vertex attributes
				constexpr const char_t* VERTEX_POSITION = "a_position";
				constexpr const char_t* VERTEX_NORMAL = "a_normal";
				constexpr const char_t* VERTEX_TANGET = "a_tangent";
				constexpr const char_t* VERTEX_COLOR = "a_color";
				constexpr const char_t* VERTEX_TEX_COORDS = "a_uv";

				// UBO
				constexpr const char_t* UBO = "UBO";

				// Uniforms
				constexpr const char_t* UNIFORM_PVM_MATRIX = "u_PVM";
			}

			enum class UniformType : uint8_t
			{
				GE_UT_MODEL_MATRIX3 = 0, //3x3
				GE_UT_NORMAL_MATRIX3,

				GE_UT_MODEL_MATRIX4, //4x4
				GE_UT_VIEW_MATRIX4,
				GE_UT_PROJECTION_MATRIX4,
				GE_UT_PVM_MATRIX4,
				// others
				GE_UT_COUNT
			};
		}
	}
}


#endif // GRAPHICS_SHADER_TOOLS_GLSL_GLSL_SHADER_TYPES_HPP