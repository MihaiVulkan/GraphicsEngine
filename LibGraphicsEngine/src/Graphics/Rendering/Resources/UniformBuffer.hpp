#ifndef GRAPHICS_RENDERING_RESOURCES_UNIFORM_BUFFER_HPP
#define GRAPHICS_RENDERING_RESOURCES_UNIFORM_BUFFER_HPP

#include "Graphics/Rendering/Resources/Buffer.hpp"
#include "Graphics/ShaderTools/GLSL/GLSLShaderTypes.hpp"
#include "Foundation/Variant.hpp"
#include "glm/mat4x4.hpp"
#include <unordered_map>

namespace GraphicsEngine
{
	namespace Graphics
	{
		// UniformBuffer - used to store uniform data among shaders to pass to a specific Graphics API
		class UniformBuffer : public Buffer
		{
			GE_RTTI(GraphicsEngine::Graphics::UniformBuffer)

		public:
			struct Uniform
			{
				Variant data;
				std::size_t size;
			};

			typedef std::unordered_map<GLSLShaderTypes::UniformType, Uniform> UniformMap;

			UniformBuffer();
			virtual ~UniformBuffer();

			void AddUniform(GLSLShaderTypes::UniformType type);

			Variant GetUniform(GLSLShaderTypes::UniformType type) const;

			template <typename T>
			void SetUniform(GLSLShaderTypes::UniformType type, const T& val)
			{
				assert(type < GLSLShaderTypes::UniformType::GE_UT_COUNT);

				auto iter = mUniformMap.find(type);

				if (iter != mUniformMap.end())
				{
					auto& ref = iter->second;
					bool_t ret = ref.data.SetValue(val);
					ref.size = sizeof(T);

					mIsUniformMapUpdated = ret;
				}
				else
				{
					LOG_ERROR("Uniform type not found!");
					mIsUniformMapUpdated = false;
				}
			}

			bool_t HasUniform(GLSLShaderTypes::UniformType type) const;

			void* GetData();


		private:

			void Create();
			void Destroy();

			UniformMap mUniformMap;
			bool_t mIsUniformMapSizeChanged;
			bool_t mIsUniformMapUpdated;
		};
	}
}

#endif // GRAPHICS_RENDERING_RESOURCES_UNIFORM_BUFFER_HPP