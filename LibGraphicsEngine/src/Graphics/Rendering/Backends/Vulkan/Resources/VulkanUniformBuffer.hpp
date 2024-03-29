#ifndef GRAPHICS_RENDERING_BACKENDS_VULKAN_RESOURCES_VULKAN_UNIFORM_BUFFER_HPP
#define GRAPHICS_RENDERING_BACKENDS_VULKAN_RESOURCES_VULKAN_UNIFORM_BUFFER_HPP

#if defined(VULKAN_RENDERER)
#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanResource.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Renderer;
		class VulkanBuffer;
		class UniformBuffer;

		//TODO - add support for Push constants besides UBO

		// Vulkan implementation of the Graphics API Dependent Resource
		class GADRUniformBuffer : public GADRResource
		{
			GE_RTTI(GraphicsEngine::Graphics::GADRUniformBuffer)

		public:
			GADRUniformBuffer();
			explicit GADRUniformBuffer(Renderer* pRenderer, UniformBuffer* pUniformBuffer);
			virtual ~GADRUniformBuffer();

			void UpdateData();
			
			virtual void OnBind(uint32_t currentBufferIdx = 0) override;
			virtual void OnUnBind(uint32_t currentBufferIdx = 0) override;

			//
			VulkanBuffer* GetVkBuffer() const;

		private:
			void Create(Renderer* pRenderer);
			void Destroy();

			VulkanBuffer* mpVulkanBuffer;

			UniformBuffer* mpUniformBuffer;
		};
	}
}
#endif // VULKAN_RENDERER

#endif // GRAPHICS_RENDERING_BACKENDS_VULKAN_RESOURCES_VULKAN_UNIFORM_BUFFER_HPP