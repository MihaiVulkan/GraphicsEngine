#include "VulkanImage.hpp"
#include "VulkanDevice.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanPassThroughAllocator.hpp"
#include "VulkanInitializers.hpp"
#include "VulkanHelpers.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanImage::VulkanImage()
	: mpDevice(nullptr)
	, mHandle(VK_NULL_HANDLE)
	, mAllocation()
	, mDefaultDescriptorInfo{}
{}

VulkanImage::VulkanImage(VulkanDevice* pDevice, VkMemoryPropertyFlags memoryPropertyFlags, VkImageType imageType, VkFormat format, const VkExtent3D& extent,
							uint32_t mipLevels, uint32_t arrayLayers, VkSampleCountFlagBits samples, VkImageTiling tiling, VkImageUsageFlags usage,
							VkSharingMode sharingMode, uint32_t queueFamilyIndexCount, const uint32_t* pQueueFamilyIndices, VkImageLayout initialLayout, 
							VkImageCreateFlags flags)
	: mpDevice(pDevice)
	, mHandle(VK_NULL_HANDLE)
	, mAllocation()
	, mDefaultDescriptorInfo{}
{
	VkImageCreateInfo imageCreateInfo = VulkanInitializers::ImageCreateInfo(imageType, format, extent, mipLevels, arrayLayers, samples, tiling,
																			usage, sharingMode, queueFamilyIndexCount, pQueueFamilyIndices, initialLayout, flags);

	Create(imageCreateInfo, memoryPropertyFlags);
}

VulkanImage::~VulkanImage()
{
	Destroy();
}

void VulkanImage::Create(const VkImageCreateInfo& imageCreateInfo, VkMemoryPropertyFlags memoryPropertyFlags)
{
	assert(mpDevice != nullptr);

	mImageData.type = imageCreateInfo.imageType;
	mImageData.format = imageCreateInfo.format;
	mImageData.extent = imageCreateInfo.extent;
	mImageData.mipLevels = imageCreateInfo.mipLevels;
	mImageData.arrayLayers = imageCreateInfo.arrayLayers;
	mImageData.samples = imageCreateInfo.samples;
	mImageData.tiling = imageCreateInfo.tiling;

	VK_CHECK_RESULT(vkCreateImage(mpDevice->GetDeviceHandle(), &imageCreateInfo, nullptr, &mHandle));

	VkMemoryRequirements memReqs;
	vkGetImageMemoryRequirements(mpDevice->GetDeviceHandle(), mHandle, &memReqs);

	uint32_t typeIndex = 0;
	assert(mpDevice->GetMemoryTypeFromProperties(memReqs.memoryTypeBits, memoryPropertyFlags, typeIndex) == true);

	// Alloc
	mpDevice->GetAllocator()->Alloc(mAllocation, memoryPropertyFlags, typeIndex, memReqs.size);

	VK_CHECK_RESULT(Bind(mAllocation.offset));

	//SetDecriptorInfo();
}

void VulkanImage::Destroy()
{
	mDefaultDescriptorInfo = {};

	// Free
	mpDevice->GetAllocator()->Free(mAllocation);

	mImageData = {};

	if (mHandle)
	{
		vkDestroyImage(mpDevice->GetDeviceHandle(), mHandle, nullptr);
		mHandle = VK_NULL_HANDLE;
	}

	if (mpDevice)
	{
		mpDevice = nullptr;
	}
}

VkResult VulkanImage::Bind(VkDeviceSize offset)
{
	return vkBindImageMemory(mpDevice->GetDeviceHandle(), mHandle, mAllocation.handle, offset);
}

void VulkanImage::CopyTo(VulkanImage* pDestImage, VkImageLayout srcLayout, VkImageLayout destLayout, VulkanQueue* pQueue, VkImageCopy* pCopyRegion)
{
	assert(pDestImage != nullptr);
	assert(pQueue != nullptr);

	VulkanCommandBuffer copyCommandBuffer(mpDevice, VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

	VkImageCopy imageCopy{};
	if (pCopyRegion == nullptr)
	{
		imageCopy.srcSubresource = { VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
		imageCopy.srcOffset = { 0, 0, 0 };
		imageCopy.dstSubresource = { VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
		imageCopy.dstOffset = { 0, 0, 0 };
		imageCopy.extent = { mImageData.extent.width, mImageData.extent.height, mImageData.extent.depth };
	}
	else
	{
		imageCopy = *pCopyRegion;
	}

	vkCmdCopyImage(copyCommandBuffer.GetHandle(), mHandle, srcLayout, pDestImage->GetHandle(), destLayout, 1, &imageCopy);

	copyCommandBuffer.Flush(pQueue);

	//TODO - image synchronization may be needed!

}

void VulkanImage::SetDecriptorInfo(VkSampler samplerHandle, VkImageView imageViewHandle, VkImageLayout imageLayout)
{
	mDefaultDescriptorInfo = VulkanInitializers::DescriptorImageInfo(samplerHandle, imageViewHandle, imageLayout);
}

const VkImage& VulkanImage::GetHandle() const
{
	return mHandle;
}

const VulkanImage::Data& VulkanImage::GetData() const
{
	return mImageData;
}


const VkDescriptorImageInfo& VulkanImage::GetDescriptorInfo() const
{
	return mDefaultDescriptorInfo;
}