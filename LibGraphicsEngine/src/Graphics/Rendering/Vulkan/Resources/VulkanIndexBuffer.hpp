#ifndef GRAPHICS_RENDERING_VULKAN_RESOURCES_VULKAN_INDEX_BUFFER_HPP
#define GRAPHICS_RENDERING_VULKAN_RESOURCES_VULKAN_INDEX_BUFFER_HPP

#include "VulkanResource.hpp"
#include "Graphics/Rendering/Resources/IndexBuffer.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Renderer;
		class VulkanRenderer;
		class VulkanBuffer;

		// Vulkan implementation of the Graphics API Dependent Resource
		class GADRIndexBuffer : public GADRResource
		{
			GE_RTTI(GraphicsEngine::Graphics::GADRIndexBuffer)

		public:
			GADRIndexBuffer();
			explicit GADRIndexBuffer(Renderer* pRenderer, IndexBuffer* pIndexBuffer);
			virtual ~GADRIndexBuffer();

			virtual void OnBind(uint32_t currentBufferIdx = 0) override;
			virtual void OnUnBind(uint32_t currentBufferIdx = 0) override;

			const Buffer::BufferUsage& GetBufferUsage() const;
			const IndexBuffer::IndexType& GetIndexType() const;

		private:
			void Create(Renderer* pRenderer);
			void Destroy();

			VulkanRenderer* mpVulkanRenderer;

			VulkanBuffer* mpVulkanBuffer;
			IndexBuffer* mpIndexBuffer;
		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_RESOURCES_VULKAN_INDEX_BUFFER_HPP