#ifndef GRAPHICS_RENDERING_RESOURCES_SHADER_HPP
#define GRAPHICS_RENDERING_RESOURCES_SHADER_HPP

#include "Graphics/Rendering/Resources/Resource.hpp"
#include <string>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class GLSLShaderParser;

		// Shader - used to store shader data to pass to a specific Graphics API
		class Shader : public Resource
		{
			GE_RTTI(GraphicsEngine::Graphics::Shader)

		public:
			enum class ShaderStage : uint8_t
			{
				GE_SS_VERTEX = 0,
				GE_SS_TESSELATION_CONTROL,
				GE_SS_TESSELATION_EVALUATION,
				GE_SS_GEOMETRY,
				GE_SS_FRAGMENT,
				GE_SS_ALL_GRAPHICS, // all the above shader types combined (no compute type and no extension types)
				GE_SS_COMPUTE,
				GE_SS_COUNT
			};

			Shader();
			explicit Shader(const std::string& sourcePath);
			virtual ~Shader();

			const Shader::ShaderStage& GetShaderStage() const;
			const std::string& GetSourcePath() const;

			GLSLShaderParser* GetGLSLParser() const;

		private:
			void Create();
			void Destroy();

			Shader::ShaderStage mShaderStage;
			std::string mSourcePath;

			GLSLShaderParser* mpGLSLParser;
		};
	}
}

#endif // GRAPHICS_RENDERING_RESOURCES_SHADER_HPP