#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanTexture.hpp"
#include "Graphics/Rendering/Backends/Vulkan/VulkanRenderer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanDevice.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanBuffer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanImage.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanImageView.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanSampler.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanInitializers.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Common/VulkanUtils.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"
#include <vector>
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

GADRTexture::GADRTexture()
	: mpVulkanImage(nullptr)
	, mpVulkanImageView(nullptr)
	, mpVulkanSampler(nullptr)
	, mpTexture(nullptr)
	, mDefaultDescriptorInfo{}
{}

GADRTexture::GADRTexture(Renderer* pRenderer, Texture* pTexture)
	: mpVulkanImage(nullptr)
	, mpVulkanImageView(nullptr)
	, mpVulkanSampler(nullptr)
	, mpTexture(pTexture)
	, mDefaultDescriptorInfo{}
{
	Create(pRenderer);
}

GADRTexture::~GADRTexture()
{
	Destroy();
}

void GADRTexture::Create(Renderer* pRenderer)
{
	assert(pRenderer != nullptr);
	assert(mpTexture != nullptr);

	// pRenderer must be a pointer to VulkanRenderer otherwise the cast will fail!
	VulkanRenderer* pVulkanRenderer = dynamic_cast<VulkanRenderer*>(pRenderer);
	assert(pVulkanRenderer != nullptr);

	VulkanDevice* pDevice = pVulkanRenderer->GetDevice();
	assert(pDevice != nullptr);

	VkImageType imageType = VulkanUtils::TextureTypeToVulkanImageType(mpTexture->GetMetaData().type);
	VkFormat format = VulkanUtils::TextureFormatToVulkanFormat(mpTexture->GetMetaData().format);

	// To copy data from host (CPU) to device (GPU) we use staging buffers
	// host local buffer is efficient for host, device local buffer is efficient for device

	// Create a host-visible buffer to copy the image data to (staging buffer)
	VulkanBuffer* pStagingBuffer = GE_ALLOC(VulkanBuffer)
	(
		pDevice,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		mpTexture->GetMetaData().dataSize, mpTexture->GetMetaData().mpData
	);
	assert(pStagingBuffer != nullptr);

	// Create a device local image to which the (host local) image data will be copied and which will be used for rendering
	mpVulkanImage = GE_ALLOC(VulkanImage)
	(
		pDevice,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		imageType, format,
		{ mpTexture->GetMetaData().width, mpTexture->GetMetaData().height, mpTexture->GetMetaData().depth },
		mpTexture->GetMetaData().mipLevels,
		mpTexture->GetMetaData().faceCount * mpTexture->GetMetaData().layerCount,
		VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
		VkImageTiling::VK_IMAGE_TILING_OPTIMAL,
		VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		VkSharingMode::VK_SHARING_MODE_EXCLUSIVE, 0, nullptr, 
		VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
		(mpTexture->GetMetaData().faceCount > 1 ? VkImageCreateFlagBits::VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0)
	);
	assert(mpVulkanImage != nullptr);

	// Setup buffer copy regions for each mipmap level (base level + the others)
	size_t regionCount = mpTexture->GetMetaData().mipLevels * mpTexture->GetMetaData().faceCount * mpTexture->GetMetaData().layerCount;
	std::vector<VkBufferImageCopy> bufferCopyRegions(regionCount);
	assert(bufferCopyRegions.size() == mpTexture->GetMetaData().mipmaps.size());

	for (uint32_t layer = 0; layer < mpTexture->GetMetaData().layerCount; ++layer)
	{
		for (uint32_t face = 0; face < mpTexture->GetMetaData().faceCount; ++face)
		{
			for (uint32_t level = 0; level < mpTexture->GetMetaData().mipLevels; ++level)
			{
				uint8_t index = mpTexture->GetMetaData().faceCount * layer +
					mpTexture->GetMetaData().mipLevels * face + level;
				auto& mipmapRef = mpTexture->GetMetaData().mipmaps[index];

				// NOTE! Mipmaps have depth of 1 and layerCount of 1
				VkBufferImageCopy bufferCopyRegion{};
				bufferCopyRegion.imageSubresource.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
				bufferCopyRegion.imageSubresource.mipLevel = level;
				bufferCopyRegion.imageSubresource.baseArrayLayer = mpTexture->GetMetaData().faceCount * layer + face;
				bufferCopyRegion.imageSubresource.layerCount = 1;
				bufferCopyRegion.imageExtent.width = mipmapRef.width;
				bufferCopyRegion.imageExtent.height = mipmapRef.height;
				bufferCopyRegion.imageExtent.depth = 1;
				bufferCopyRegion.bufferOffset = mipmapRef.offset;

				bufferCopyRegions[index] = bufferCopyRegion;
			}
		}
	}

	// Buffer copies have to be submitted to a queue, so we need a command buffer for them
	// Note: Some devices offer a dedicated transfer/present queue (with only the transfer bit set) that may be faster when doing lots of copies

	if (pDevice->IsPresentQueueSupported()) // separate present queue
	{
		pStagingBuffer->CopyTo(mpVulkanImage, pDevice->GetPresentQueue(), bufferCopyRegions);
	}
	else // graphics and present queue are the same
	{
		pStagingBuffer->CopyTo(mpVulkanImage, pDevice->GetGraphicsQueue(), bufferCopyRegions);
	}
	GE_FREE(pStagingBuffer);

	// image view
	VkImageSubresourceRange subresourceRange{};
	subresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = mpTexture->GetMetaData().mipLevels;
	subresourceRange.layerCount = mpTexture->GetMetaData().faceCount * mpTexture->GetMetaData().layerCount;

	VkImageViewType imageViewType = VulkanUtils::TextureTypeToVulkanImageViewType(mpTexture->GetMetaData().type);
	mpVulkanImageView = GE_ALLOC(VulkanImageView)
	(
		pDevice, 
		mpVulkanImage->GetHandle(), imageViewType,
		format, 
		{ VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R, VkComponentSwizzle::VK_COMPONENT_SWIZZLE_G, VkComponentSwizzle::VK_COMPONENT_SWIZZLE_B, VkComponentSwizzle::VK_COMPONENT_SWIZZLE_A },
		subresourceRange
	);
	assert(mpVulkanImageView != nullptr);

	//sampler
	VkFilter filter = VulkanUtils::TextureFilterModeToVulkanFilterMode(mpTexture->GetMetaData().filterMode);
	VkSamplerMipmapMode mipmapMode = VulkanUtils::TextureMipMapModeToVulkanMipMapMode(mpTexture->GetMetaData().mipMapMode);
	VkSamplerAddressMode addressMode = VulkanUtils::TextureWrapModeToVulkanWrapMode(mpTexture->GetMetaData().wrapMode);

	mpVulkanSampler = GE_ALLOC(VulkanSampler)
	(
		pDevice,
		filter, filter, mipmapMode,
		addressMode, addressMode, addressMode,
		0.0f, pDevice->GetPhysicalDeviceEnabledFeatures().samplerAnisotropy,
		static_cast<float32_t>(pDevice->GetPhysicalDeviceEnabledFeatures().samplerAnisotropy ? mpTexture->GetMetaData().mipLevels : 1.0f),
		VK_FALSE, VkCompareOp::VK_COMPARE_OP_NEVER, 0.0f, static_cast<float32_t>(mpTexture->GetMetaData().mipLevels),
		VkBorderColor::VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE
	);
	assert(mpVulkanSampler != nullptr);

	UpdateDecriptorInfo();
}

void GADRTexture::Destroy()
{
	mDefaultDescriptorInfo = {};

	GE_FREE(mpVulkanSampler);
	GE_FREE(mpVulkanImageView);
	GE_FREE(mpVulkanImage);

	if (mpTexture)
	{
		mpTexture = nullptr;
	}
}

void GADRTexture::UpdateDecriptorInfo()
{
	assert(mpVulkanSampler != nullptr);
	assert(mpVulkanImageView != nullptr);

	mDefaultDescriptorInfo.sampler = mpVulkanSampler->GetHandle();
	mDefaultDescriptorInfo.imageView = mpVulkanImageView->GetHandle();
	mDefaultDescriptorInfo.imageLayout = VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; //TODO - by default output image layout is optimal - majority of cases
}

const VkDescriptorImageInfo& GADRTexture::GetDescriptorInfo() const
{
	return mDefaultDescriptorInfo;
}