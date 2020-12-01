#ifndef GRAPHICS_RENDERING_VULKAN_RESOURCES_UNIFORM_BUFFER_HPP
#define GRAPHICS_RENDERING_VULKAN_RESOURCES_UNIFORM_BUFFER_HPP

#include "VulkanResource.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Renderer;
		class VulkanBuffer;
		class UniformBuffer;

		// Vulkan implementation of the Graphics API Dependent Resource
		class GADRUniformBuffer : public GADRResource
		{
			GE_RTTI(GraphicsEngine::Graphics::GADRUniformBuffer)

		public:
			GADRUniformBuffer();
			explicit GADRUniformBuffer(Renderer* pRenderer, UniformBuffer* pUniformBuffer);
			virtual ~GADRUniformBuffer();

			void UpdateData(UniformBuffer* pUniformBuffer = nullptr);
			void Bind() const;

			//
			VulkanBuffer* GetVKBuffer() const;

		private:
			void Create(Renderer* pRenderer);
			void Destroy();

			VulkanBuffer* mpVulkanBuffer;

			UniformBuffer* mpUniformBuffer;
		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_RESOURCES_UNIFORM_BUFFER_HPP