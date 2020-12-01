#ifndef GRAPHICS_RENDERING_VULKAN_RESOURCES_VERTEX_BUFFER_HPP
#define GRAPHICS_RENDERING_VULKAN_RESOURCES_VERTEX_BUFFER_HPP

#include "VulkanResource.hpp"
#include "Graphics/Rendering/Resources/VertexBuffer.hpp"


namespace GraphicsEngine
{
	namespace Graphics
	{
		class Renderer;
		class VulkanBuffer;

		// Vulkan implementation of the Graphics API Dependent Resource
		class GADRVertexBuffer : public GADRResource
		{
			GE_RTTI(GraphicsEngine::Graphics::GADRVertexBuffer)

		public:
			GADRVertexBuffer();
			explicit GADRVertexBuffer(Renderer* pRenderer, VertexBuffer* pVertexBuffer);
			virtual ~GADRVertexBuffer();

			void Bind() const;

			const VertexBuffer::Usage& GetUsage() const;
			const VertexBuffer::InputRate& GetInputRate() const;

			VulkanBuffer* GetVkBuffer() const;
			const VkVertexInputBindingDescription& GetVkInputBinding() const;

			static VkVertexInputRate InputRateToVulkanInputRate(const VertexBuffer::InputRate& inputRate);

		private:
			void Create(Renderer* pRenderer);
			void Destroy();


			VulkanBuffer* mpVulkanBuffer;

			VertexBuffer* mpVertexBuffer;

			VkVertexInputBindingDescription mInputBinding;

		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_RESOURCES_VERTEX_BUFFER_HPP