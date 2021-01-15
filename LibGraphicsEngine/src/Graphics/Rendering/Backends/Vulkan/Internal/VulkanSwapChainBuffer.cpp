#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanSwapChainBuffer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanDevice.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanImage.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanImageView.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanInitializers.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanHelpers.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanSwapChainBuffer::VulkanSwapChainBuffer()
	: mpDevice(nullptr)
	, mpImage(nullptr)
	, mpImageView(nullptr)
	, mBufferType(BufferType::GE_BT_COUNT)
{}


VulkanSwapChainBuffer::VulkanSwapChainBuffer(VulkanDevice* pDevice, VkImage imageHandle, VkFormat format, BufferType type)
	: mpDevice(pDevice)
	, mpImage(nullptr)
	, mpImageView(nullptr)
	, mBufferType(type)
{
	Create(imageHandle, format);
}

VulkanSwapChainBuffer::VulkanSwapChainBuffer(VulkanDevice* pDevice, VkFormat format, VkImageUsageFlags usage, uint32_t width, uint32_t height, BufferType type)
	: mpDevice(pDevice)
	, mpImage(nullptr)
	, mpImageView(nullptr)
	, mBufferType(type)
{
	Create(format, usage, width, height);
}

VulkanSwapChainBuffer::~VulkanSwapChainBuffer()
{
	Destroy();
}


void VulkanSwapChainBuffer::Create(VkImage imageHandle, VkFormat format)
{
	assert(mpDevice != nullptr);
	assert(imageHandle != VK_NULL_HANDLE);

	VkImageAspectFlags aspectMask = 0;

	switch (mBufferType)
	{
		case BufferType::GE_BT_COLOR:
		{
			aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
		}
		break;
		case BufferType::GE_BT_DEPTH_STENCIL:
		{
			aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT;

			// Stencil aspect should only be set on depth + stencil formats (VK_FORMAT_D16_UNORM_S8_UINT..VK_FORMAT_D32_SFLOAT_S8_UINT
			if (format >= VkFormat::VK_FORMAT_D16_UNORM_S8_UINT)
			{
				aspectMask |= VkImageAspectFlagBits::VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}
		break;
		default:
			LOG_ERROR("Invalid swap chain buffer type!");
	}

	assert(aspectMask > 0);

	// ImageView
	/*VkComponentMapping componentMapping = 
		VulkanInitializers::ComponentMapping(VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R, VkComponentSwizzle::VK_COMPONENT_SWIZZLE_G, 
											VkComponentSwizzle::VK_COMPONENT_SWIZZLE_B, VkComponentSwizzle::VK_COMPONENT_SWIZZLE_A);
	VkImageSubresourceRange imageSubresourceRange = VulkanInitializers::ImageSubresourceRange(aspectMask, 0, 1, 0, 1);*/

	VkComponentMapping componentMapping{};
	componentMapping.r = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R;
	componentMapping.g = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_G;
	componentMapping.b = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_B;
	componentMapping.a = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_A;

	VkImageSubresourceRange imageSubresourceRange{};
	imageSubresourceRange.aspectMask = aspectMask;
	imageSubresourceRange.baseMipLevel = 0;
	imageSubresourceRange.levelCount = 1;
	imageSubresourceRange.baseArrayLayer = 0;
	imageSubresourceRange.layerCount = 1;
	 
	mpImageView = GE_ALLOC(VulkanImageView)(mpDevice, imageHandle, VkImageViewType::VK_IMAGE_VIEW_TYPE_2D, format, componentMapping, imageSubresourceRange);
	assert(mpImageView != nullptr);
}

void VulkanSwapChainBuffer::Create(VkFormat format, VkImageUsageFlags usage, uint32_t width, uint32_t height)
{
	assert(mpDevice != nullptr);

	VkImageAspectFlags aspectMask = 0;

	switch (mBufferType)
	{
		case BufferType::GE_BT_COLOR:
		{
			aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
		}
		break;
		case BufferType::GE_BT_DEPTH_STENCIL:
		{
			aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT;

			// Stencil aspect should only be set on depth + stencil formats (VK_FORMAT_D16_UNORM_S8_UINT..VK_FORMAT_D32_SFLOAT_S8_UINT
			if (format >= VkFormat::VK_FORMAT_D16_UNORM_S8_UINT)
			{
				aspectMask |= VkImageAspectFlagBits::VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}
		break;
		default:
			LOG_ERROR("Invalid farmebuffer Buffer type!");
	}

	assert(aspectMask > 0);

	// Image
	mpImage = GE_ALLOC(VulkanImage)(mpDevice, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VkImageType::VK_IMAGE_TYPE_2D, format,
									{ width, height, 1 }, 1, 1, VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT, VkImageTiling::VK_IMAGE_TILING_OPTIMAL, usage);
	assert(mpImage != nullptr);

	// ImageView
	/*VkComponentMapping componentMapping = 
		VulkanInitializers::ComponentMapping(VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R, VkComponentSwizzle::VK_COMPONENT_SWIZZLE_G, 
											VkComponentSwizzle::VK_COMPONENT_SWIZZLE_B, VkComponentSwizzle::VK_COMPONENT_SWIZZLE_A);
	VkImageSubresourceRange imageSubresourceRange = VulkanInitializers::ImageSubresourceRange(aspectMask, 0, 1, 0, 1);*/

	VkComponentMapping componentMapping{};
	componentMapping.r = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R;
	componentMapping.g = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_G;
	componentMapping.b = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_B;
	componentMapping.a = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_A;

	VkImageSubresourceRange imageSubresourceRange{};
	imageSubresourceRange.aspectMask = aspectMask;
	imageSubresourceRange.baseMipLevel = 0;
	imageSubresourceRange.levelCount = 1;
	imageSubresourceRange.baseArrayLayer = 0;
	imageSubresourceRange.layerCount = 1;

	mpImageView = GE_ALLOC(VulkanImageView)(mpDevice, mpImage->GetHandle(), VkImageViewType::VK_IMAGE_VIEW_TYPE_2D, format, componentMapping, imageSubresourceRange);
	assert(mpImageView != nullptr);
}

void VulkanSwapChainBuffer::Destroy()
{
	mBufferType = BufferType::GE_BT_COUNT;

	GE_FREE(mpImageView);

	GE_FREE(mpImage);

	if (mpDevice)
	{
		mpDevice = nullptr;
	}
}

const VkImage& VulkanSwapChainBuffer::GetImageHandle() const
{
	assert(mpImage != nullptr);

	return mpImage->GetHandle();
}

const VkImageView& VulkanSwapChainBuffer::GetImageViewHandle() const
{
	assert(mpImageView != nullptr);

	return mpImageView->GetHandle();
}

const VulkanSwapChainBuffer::BufferType& VulkanSwapChainBuffer::GetBufferType() const
{
	return mBufferType;
}