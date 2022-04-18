#ifndef GRAPHICS_RENDERING_BACKENDS_VULKAN_RESOURCES_VULKAN_VERTEX_BUFFER_HPP
#define GRAPHICS_RENDERING_BACKENDS_VULKAN_RESOURCES_VULKAN_VERTEX_BUFFER_HPP

#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanResource.hpp"
#include "Graphics/Rendering/Resources/VertexBuffer.hpp"


namespace GraphicsEngine
{
	namespace Graphics
	{
		class Renderer;
		class VulkanRenderer;
		class VulkanBuffer;

		// Vulkan implementation of the Graphics API Dependent Resource
		class GADRVertexBuffer : public GADRResource
		{
			GE_RTTI(GraphicsEngine::Graphics::GADRVertexBuffer)

		public:
			GADRVertexBuffer();
			explicit GADRVertexBuffer(Renderer* pRenderer, VertexBuffer* pVertexBuffer);
			virtual ~GADRVertexBuffer();

			virtual void OnBind(uint32_t currentBufferIdx = 0) override;
			virtual void OnUnBind(uint32_t currentBufferIdx = 0) override;

			const Buffer::BufferUsage& GetBufferUsage() const;

		private:
			void Create(Renderer* pRenderer);
			void Destroy();

			VulkanRenderer* mpVulkanRenderer;

			VulkanBuffer* mpVulkanBuffer;
			VertexBuffer* mpVertexBuffer;
		};
	}
}

#endif // GRAPHICS_RENDERING_BACKENDS_VULKAN_RESOURCES_VULKAN_VERTEX_BUFFER_HPP