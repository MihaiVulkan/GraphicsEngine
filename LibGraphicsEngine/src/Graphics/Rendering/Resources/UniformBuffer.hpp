#ifndef GRAPHICS_RENDERING_RESOURCES_UNIFORM_BUFFER_HPP
#define GRAPHICS_RENDERING_RESOURCES_UNIFORM_BUFFER_HPP

#include "Resource.hpp"
#include "glm/mat4x4.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		// UniformBuffer - used to store uniform data among shaders to pass to a specific Graphics API
		class UniformBuffer : public Resource
		{
			GE_RTTI(GraphicsEngine::Graphics::UniformBuffer)

		public:
			enum class Usage : uint8_t
			{
				U_STATIC = 0,
				U_DYNAMIC,
				U_COUNT
			};

			//TODO, work on this
			enum class UniformUsage : uint8_t
			{
				UU_PVM = 0,
				UU_PVM_MAT,
				UU_TEX,
				UU_PVM_TEX,
				UU_COUNT
			};

			// TODO - make generic, improve
			// Uniform structs
			// layout as in vertex shader
			struct Uniforms
			{
			//	glm::mat4 projectionMatrix;
			//	glm::mat4 modelMatrix;
			//	glm::mat4 viewMatrix;

				glm::mat4 PVM;

				// others
			} Uniforms;

			UniformBuffer();
			explicit UniformBuffer(UniformUsage uniformUsage);
			virtual ~UniformBuffer();

			const UniformBuffer::UniformUsage& GetUniformUsage() const;

			void* GetData();

			//void* GetData() const;
			//void SetData(void* pData, uint32_t size);
			//const uint32_t& GetSize() const;

			uint32_t GetSize() const;


		private:

			void Create();
			void Destroy();

			void SetupUniforms();

			UniformUsage mUniformUsage;
			//void* mpData;
			//uint32_t mSize;
		};
	}
}

#endif // GRAPHICS_RENDERING_RESOURCES_UNIFORM_BUFFER_HPP