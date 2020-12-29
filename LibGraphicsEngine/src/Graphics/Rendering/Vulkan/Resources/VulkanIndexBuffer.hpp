#ifndef GRAPHICS_RENDERING_VULKAN_RESOURCES_VULKAN_INDEX_BUFFER_HPP
#define GRAPHICS_RENDERING_VULKAN_RESOURCES_VULKAN_INDEX_BUFFER_HPP

#include "VulkanResource.hpp"
#include "Graphics/Rendering/Resources/IndexBuffer.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Renderer;
		class VulkanBuffer;

		// Vulkan implementation of the Graphics API Dependent Resource
		class GADRIndexBuffer : public GADRResource
		{
			GE_RTTI(GraphicsEngine::Graphics::GADRIndexBuffer)

		public:
			GADRIndexBuffer();
			explicit GADRIndexBuffer(Renderer* pRenderer, IndexBuffer* pIndexBuffer);
			virtual ~GADRIndexBuffer();

			//void Bind();

			const Buffer::BufferUsage& GetBufferUsage() const;
			const IndexBuffer::IndexType& GetIndexType() const;

			VulkanBuffer* GetVkBuffer() const;

			static VkIndexType IndexTypeToVulkanIndexType(const IndexBuffer::IndexType& indexType);

		private:
			void Create(Renderer* pRenderer);
			void Destroy();

			VulkanBuffer* mpVulkanBuffer;
			IndexBuffer* mpIndexBuffer;
		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_RESOURCES_VULKAN_INDEX_BUFFER_HPP