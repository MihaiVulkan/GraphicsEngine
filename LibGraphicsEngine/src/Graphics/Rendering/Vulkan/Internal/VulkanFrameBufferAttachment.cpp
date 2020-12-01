#include "VulkanFrameBufferAttachment.hpp"
#include "VulkanDevice.hpp"
#include "VulkanImage.hpp"
#include "VulkanImageView.hpp"
#include "VulkanInitializers.hpp"
#include "VulkanHelpers.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanFrameBufferAttachment::VulkanFrameBufferAttachment()
	: mpDevice(nullptr)
	, mpImage(nullptr)
	, mpImageView(nullptr)
	, mType(Type::COUNT)
{}

//VulkanFrameBufferAttachment::VulkanFrameBufferAttachment(VkImage imageHandle, VkImageView imageViewHandle, Type type)
//	: mpDevice(nullptr)
//	, mpImage(nullptr)
//	, mpImageView(nullptr)
//	, mType(type)

//{}

VulkanFrameBufferAttachment::VulkanFrameBufferAttachment(VulkanDevice* pDevice, VkImage imageHandle, VkFormat format, Type type)
	: mpDevice(pDevice)
	, mpImage(nullptr)
	, mpImageView(nullptr)
	, mType(type)
{
	Create(imageHandle, format);
}

VulkanFrameBufferAttachment::VulkanFrameBufferAttachment(VulkanDevice* pDevice, VkFormat format, VkImageUsageFlags usage, uint32_t width, uint32_t height, Type type)
	: mpDevice(pDevice)
	, mpImage(nullptr)
	, mpImageView(nullptr)
	, mType(type)
{
	Create(format, usage, width, height);
}

VulkanFrameBufferAttachment::~VulkanFrameBufferAttachment()
{
	Destroy();
}


void VulkanFrameBufferAttachment::Create(VkImage imageHandle, VkFormat format)
{
	assert(mpDevice != nullptr);
	//assert(mImageHandle != VK_NULL_HANDLE);

	VkImageAspectFlags aspectMask = 0;
	//	VkImageLayout imageLayout;

	switch (mType)
	{
		case Type::COLOR:
		{
			aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
			//		imageLayout = VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}
		break;
		case Type::DEPTH_STENCIL:
		{
			aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT;
			//		imageLayout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			// Stencil aspect should only be set on depth + stencil formats (VK_FORMAT_D16_UNORM_S8_UINT..VK_FORMAT_D32_SFLOAT_S8_UINT
			if (format >= VkFormat::VK_FORMAT_D16_UNORM_S8_UINT)
			{
				aspectMask |= VkImageAspectFlagBits::VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}
		break;
		default:
			LOG_ERROR("Invalid farmebuffer attachment type!");
	}

	assert(aspectMask > 0);

	// ImageView
	VkComponentMapping componentMapping = 
		VulkanInitializers::ComponentMapping(VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R, VkComponentSwizzle::VK_COMPONENT_SWIZZLE_G, 
											VkComponentSwizzle::VK_COMPONENT_SWIZZLE_B, VkComponentSwizzle::VK_COMPONENT_SWIZZLE_A);
	VkImageSubresourceRange imageSubresourceRange = VulkanInitializers::ImageSubresourceRange(aspectMask, 0, 1, 0, 1);

	mpImageView = GE_ALLOC(VulkanImageView)(mpDevice, imageHandle, VkImageViewType::VK_IMAGE_VIEW_TYPE_2D, format, componentMapping, imageSubresourceRange);
}

void VulkanFrameBufferAttachment::Create(VkFormat format, VkImageUsageFlags usage, uint32_t width, uint32_t height)
{
	assert(mpDevice != nullptr);

	VkImageAspectFlags aspectMask = 0;
//	VkImageLayout imageLayout;

	switch (mType)
	{
		case Type::COLOR:
		{
			aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
			//		imageLayout = VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}
		break;
		case Type::DEPTH_STENCIL:
		{
			aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT;// | VK_IMAGE_ASPECT_STENCIL_BIT;
			//		imageLayout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			// Stencil aspect should only be set on depth + stencil formats (VK_FORMAT_D16_UNORM_S8_UINT..VK_FORMAT_D32_SFLOAT_S8_UINT
			if (format >= VkFormat::VK_FORMAT_D16_UNORM_S8_UINT)
			{
				aspectMask |= VkImageAspectFlagBits::VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}
		break;
		default:
			LOG_ERROR("Invalid farmebuffer attachment type!");
	}

	assert(aspectMask > 0);

	// Image
	mpImage = GE_ALLOC(VulkanImage)(mpDevice, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VkImageType::VK_IMAGE_TYPE_2D, format,
									{ width, height, 1 }, 1, 1, VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT, VkImageTiling::VK_IMAGE_TILING_OPTIMAL, usage);

	// ImageView
	VkComponentMapping componentMapping = 
		VulkanInitializers::ComponentMapping(VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R, VkComponentSwizzle::VK_COMPONENT_SWIZZLE_G, 
											VkComponentSwizzle::VK_COMPONENT_SWIZZLE_B, VkComponentSwizzle::VK_COMPONENT_SWIZZLE_A);
	VkImageSubresourceRange imageSubresourceRange = VulkanInitializers::ImageSubresourceRange(aspectMask, 0, 1, 0, 1);

	mpImageView = GE_ALLOC(VulkanImageView)(mpDevice, mpImage->GetHandle(), VkImageViewType::VK_IMAGE_VIEW_TYPE_2D, format, componentMapping, imageSubresourceRange);
}

void VulkanFrameBufferAttachment::Destroy()
{
	mType = Type::COUNT;

	GE_FREE(mpImageView);

	GE_FREE(mpImage);

	if (mpDevice)
	{
		mpDevice = nullptr;
	}
}

const VkImage& VulkanFrameBufferAttachment::GetImageHandle() const
{
	return mpImage->GetHandle();
}

const VkImageView& VulkanFrameBufferAttachment::GetImageViewHandle() const
{
	return mpImageView->GetHandle();
}

//const VkFormat& VulkanFrameBufferAttachment::GetFormat() const
//{
//	return mFormat;
//}

const VulkanFrameBufferAttachment::Type& VulkanFrameBufferAttachment::GetType() const
{
	return mType;
}