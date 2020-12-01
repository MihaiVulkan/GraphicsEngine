#ifndef GRAPHICS_RENDERING_VULKAN_FRAMEBUFFER_ATTACHMENT_HPP
#define GRAPHICS_RENDERING_VULKAN_FRAMEBUFFER_ATTACHMENT_HPP

#include "Foundation/TypeDefs.hpp"
#include "vulkan/vulkan.h"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class VulkanDevice;
		class VulkanImage;
		class VulkanImageView;

		// 2D framebuffer attachment
		class VulkanFrameBufferAttachment
		{
		public:
			enum Type
			{
				COLOR = 0,
				DEPTH_STENCIL,
				COUNT
			};

			VulkanFrameBufferAttachment();
		//	explicit VulkanFrameBufferAttachment(VkImage imageHandle, VkImageView imageViewHandle, Type type);
			explicit VulkanFrameBufferAttachment(VulkanDevice* pDevice, VkImage imageHandle, VkFormat format, Type type);
			explicit VulkanFrameBufferAttachment(VulkanDevice* pDevice, VkFormat format, VkImageUsageFlags usage, uint32_t width, uint32_t height, Type type);
			virtual ~VulkanFrameBufferAttachment();



			const VkImage& GetImageHandle() const;
			const VkImageView& GetImageViewHandle() const;
			//const VkFormat& GetFormat() const;
			const VulkanFrameBufferAttachment::Type& GetType() const;

		private:
			void Create(VkImage imageHandle, VkFormat format);
			void Create(VkFormat format, VkImageUsageFlags usage, uint32_t width, uint32_t height);
			void Destroy();

			VulkanDevice* mpDevice;

			VulkanImage* mpImage;
			VulkanImageView* mpImageView;
			Type mType;
		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_FRAMEBUFFER_ATTACHMENT_HPP