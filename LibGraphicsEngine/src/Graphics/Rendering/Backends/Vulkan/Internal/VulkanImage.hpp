#ifndef GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_IMAGE_HPP
#define GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_IMAGE_HPP

#include "Graphics/Rendering/Backends/Vulkan/Common/VulkanObject.hpp"
#include "VulkanAllocator.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class VulkanDevice;
		class VulkanQueue;

		/*
		vkCmdBlitImage
		vkCmdClearColorImage
		vkCmdClearDepthStencilImage
		vkCmdClearAttachments
		vkCmdCopyImageToBuffer
		vkCmdResolveImage

		vkGetPhysicalDeviceImageFormatProperties

		vks::tools::setImageLayout
		*/


		/*
			*Wrapper for VkImage object*

			Images represent multidimensional - up to 3 - arrays of data which can be used for various purposes (e.g. attachments, textures),
			by binding them to a graphics or compute pipeline via descriptor sets, or by directly specifying them as parameters to certain commands.

			Images are stored in implementation-dependent opaque layouts in memory. Each layout has limitations on what kinds of operations are
			supported for image subresources using the layout. At any given time, the data representing an image subresource in memory exists in a
			particular layout which is determined by the most recent layout transition that was performed on that image subresource. Applications 
			have control over which layout each image subresource uses, and can transition an image subresource from one layout to another. 
			Transitions can happen with an image memory barrier, included as part of a vkCmdPipelineBarrier or a vkCmdWaitEvents command buffer 
			command (see Image Memory Barriers), or as part of a subpass dependency within a render pass (see VkSubpassDependency). The image 
			layout is per-image subresource, and separate image subresources of the same image can be in different layouts at the same time with 
			one exception - depth and stencil aspects of a given image subresource must always be in the same layout.
		*/
		class VulkanImage : public VulkanObject
		{
			GE_RTTI(GraphicsEngine::Graphics::VulkanImage)

		public:
			typedef struct
			{
				VkImageType type;
				VkFormat format;
				VkImageLayout layout;
				VkExtent3D extent;
				uint32_t mipLevels;
				uint32_t arrayLayers;
				VkSampleCountFlagBits samples;
				VkImageTiling tiling;
			} Data;

			// ImageType is denoted by VkImageType

			// NOTE! For now we suppose we work only on hardware that supports COHERENT memory via the VK_MEMORY_PROPERTY_HOST_COHERENT_BIT flag
			VulkanImage();
			explicit VulkanImage(VulkanDevice* pDevice, VkMemoryPropertyFlags memoryPropertyFlags, 
				VkImageType imageType, VkFormat format, const VkExtent3D& extent, uint32_t mipLevels, uint32_t arrayLayers, VkSampleCountFlagBits samples,
				VkImageTiling tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL, VkImageUsageFlags usage = VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT,
				VkSharingMode sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE, uint32_t queueFamilyIndexCount = 0, const uint32_t * pQueueFamilyIndices = nullptr,
				VkImageLayout initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED, VkImageCreateFlags flags = 0);
			virtual ~VulkanImage();


			VkResult Bind(VkDeviceSize offset = 0);

			void CopyTo(VulkanImage* pDestImage, VkImageLayout srcLayout, VkImageLayout destLayout, VulkanQueue* pQueue, VkImageCopy* pCopyRegion = nullptr);

			void SetDecriptorInfo(VkSampler samplerHandle, VkImageView imageViewHandle, VkImageLayout imageLayout);

			const VkImage& GetHandle() const;

			const VulkanImage::Data& GetData() const;
			const VkDescriptorImageInfo& GetDescriptorInfo() const;

		private:
			void Create(const VkImageCreateInfo& imageCreateInfo, VkMemoryPropertyFlags memoryPropertyFlags);
			void Destroy();

			VulkanDevice* mpDevice;

			VkImage mHandle;
			VulkanAllocator::Allocation mAllocation;

			VulkanImage::Data mImageData;

			VkDescriptorImageInfo mDefaultDescriptorInfo;
		};
	}
}

#endif // GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_IMAGE_HPP