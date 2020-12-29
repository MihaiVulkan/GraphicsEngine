#ifndef GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_SWAP_CHAIN_BUFFER_HPP
#define GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_SWAP_CHAIN_BUFFER_HPP

#include "Graphics/Rendering/Vulkan/Common/VulkanObject.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class VulkanDevice;
		class VulkanImage;
		class VulkanImageView;

		/*
			Wrapper for the swap chain images + image views
				
			The image views are used as attachments for the VkFramebuffer
		*/
		class VulkanSwapChainBuffer : public VulkanObject
		{
			GE_RTTI(GraphicsEngine::Graphics::VulkanSwapChainBuffer)

		public:
			enum class BufferType : uint8_t
			{
				GE_BT_COLOR = 0,
				GE_BT_DEPTH_STENCIL,
				GE_BT_COUNT
			};

			VulkanSwapChainBuffer();
			explicit VulkanSwapChainBuffer(VulkanDevice* pDevice, VkImage imageHandle, VkFormat format, BufferType type);
			explicit VulkanSwapChainBuffer(VulkanDevice* pDevice, VkFormat format, VkImageUsageFlags usage, uint32_t width, uint32_t height, BufferType type);
			virtual ~VulkanSwapChainBuffer();



			const VkImage& GetImageHandle() const;
			const VkImageView& GetImageViewHandle() const;
			const VulkanSwapChainBuffer::BufferType& GetBufferType() const;

		private:
			void Create(VkImage imageHandle, VkFormat format);
			void Create(VkFormat format, VkImageUsageFlags usage, uint32_t width, uint32_t height);
			void Destroy();

			VulkanDevice* mpDevice;

			VulkanImage* mpImage;
			VulkanImageView* mpImageView;
			BufferType mBufferType;
		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_SWAP_CHAIN_BUFFER_HPP