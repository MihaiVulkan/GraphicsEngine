#ifndef GRAPHICS_RENDERING_VULKAN_IMAGE_VIEW_HPP
#define GRAPHICS_RENDERING_VULKAN_IMAGE_VIEW_HPP

#include "Foundation/TypeDefs.hpp"
#include "vulkan/vulkan.h"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class VulkanDevice;

		/*
			*Wrapper for VkImageView object*

			Image objects are not directly accessed by pipeline shaders for reading or writing image data. Instead, image views 
			representing contiguous ranges of the image subresources and containing additional metadata are used for that purpose.
			Views must be created on images of compatible types, and must represent a valid subset of image subresources.
		*/
		class VulkanImageView
		{
		public:
			VulkanImageView();
			explicit VulkanImageView(VulkanDevice* pDevice, VkImage imageHandle, VkImageViewType viewType, VkFormat format, const VkComponentMapping& components,
				const VkImageSubresourceRange& subresourceRange, VkImageViewCreateFlags flags = 0);
			virtual ~VulkanImageView();

			const VkImageView& GetHandle() const;

		private:
			void Create(const VkImageViewCreateInfo& imageViewCreateInfo);
			void Destroy();


			VulkanDevice* mpDevice;

			VkImageView mHandle;
		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_IMAGE_VIEW_HPP