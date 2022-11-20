#if defined(VULKAN_RENDERER)
#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanTexture.hpp"
#include "Graphics/Rendering/Backends/Vulkan/VulkanRenderer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanDevice.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanCommandPool.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanCommandBuffer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanQueue.hpp"
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
	, mVulkanDescriptorInfo{}
	, mpTexture(nullptr)
{}

GADRTexture::GADRTexture(Renderer* pRenderer, Texture* pTexture)
	: mpVulkanImage(nullptr)
	, mpVulkanImageView(nullptr)
	, mpVulkanSampler(nullptr)
	, mVulkanDescriptorInfo{}
	, mpTexture(pTexture)
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

	Texture::UsageType textureUsageType = mpTexture->GetUsageType();

	VkImageType imageType = VulkanUtils::TextureTypeToVulkanImageType(mpTexture->GetMetaData().type);
	VkFormat format = VulkanUtils::TextureFormatToVulkanFormat(mpTexture->GetMetaData().format);

	// common for all texture usage types
	VkImageUsageFlags usage = VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT; // to be able to sample from it in shader

	VulkanBuffer* pStagingBuffer = nullptr;
	if (textureUsageType == Texture::UsageType::GE_UT_RENDER)
	{
		// To copy data from host (CPU) to device (GPU) we use staging buffers
		// host local buffer is efficient for host, device local buffer is efficient for device

		// Create a host-visible buffer to copy the image data to (staging buffer)
		pStagingBuffer = GE_ALLOC(VulkanBuffer)
			(
				pDevice,
				VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				mpTexture->GetMetaData().dataSize, mpTexture->GetMetaData().mpData
				);
		assert(pStagingBuffer != nullptr);

		// update the usage bits
		usage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}
	else if (textureUsageType == Texture::UsageType::GE_UT_RENDER_TARGET)
	{
		// update the usage bits basaed on the used format (color or depth)
		if (mpTexture->IsColorFormat())
		{
			usage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		}
		else if (mpTexture->IsDepthFormat())
		{
			usage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		}
	}

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
		usage,
		VkSharingMode::VK_SHARING_MODE_EXCLUSIVE, 0, nullptr, 
		VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
		(mpTexture->GetMetaData().faceCount > 1 ? VkImageCreateFlagBits::VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0)
	);
	assert(mpVulkanImage != nullptr);

	VkImageAspectFlags aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM;
	if (mpTexture->IsColorFormat())
	{
		aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
	}
	else if (mpTexture->IsDepthFormat())
	{
		if (mpTexture->GetSamplingType() == Texture::SamplingType::GE_ST_SAMPLING)
		{
			// when sampling we actually need only the depth bit - according to Vulkan spec
			aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT;
		}
		else
		{
			// if not sampling we use both bits: depth and stencil
			aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT | VkImageAspectFlagBits::VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}

	if (textureUsageType == Texture::UsageType::GE_UT_RENDER && pStagingBuffer)
	{
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
					bufferCopyRegion.imageSubresource.aspectMask = aspectMask;
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
	}

	////////////////////

	if (mpTexture->IsDepthFormat() && mpTexture->GetSamplingType() == Texture::SamplingType::GE_ST_SAMPLING)
	{
		VulkanCommandPool copyCommandPool(pDevice, pDevice->GetGraphicsQueue()->GetFamilyIndex());
		VulkanCommandBuffer copyCommandBuffer(pDevice, copyCommandPool.GetHandle(), VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

		VkImageSubresourceRange subresourceRange{};
		subresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = 1;
		subresourceRange.layerCount = 1;

		// Image barrier for optimal image (target)
		// Optimal image will be used as destination for the copy
		VulkanUtils::SetImageLayout(copyCommandBuffer.GetHandle(), mpVulkanImage->GetHandle(),
			VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED, VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
			subresourceRange);

		copyCommandBuffer.Flush(pDevice->GetGraphicsQueue());
	}
	////////////////////

	// image view
	VkImageSubresourceRange subresourceRange{};
	subresourceRange.aspectMask = aspectMask;// VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = mpTexture->GetMetaData().mipLevels;
	subresourceRange.layerCount = mpTexture->GetMetaData().faceCount * mpTexture->GetMetaData().layerCount;

	VkImageViewType imageViewType = VulkanUtils::TextureTypeToVulkanImageViewType(mpTexture->GetMetaData().type);
	mpVulkanImageView = GE_ALLOC(VulkanImageView)
	(
		pDevice, 
		mpVulkanImage->GetHandle(), imageViewType,
		format, 
		//TODO - component swizzle should be based on format (rgba, rgb, rg, etc.)
		{},// { VkComponentSwizzle::VK_COMPONENT_SWIZZLE_R, VkComponentSwizzle::VK_COMPONENT_SWIZZLE_G, VkComponentSwizzle::VK_COMPONENT_SWIZZLE_B, VkComponentSwizzle::VK_COMPONENT_SWIZZLE_A },
		subresourceRange
	);
	assert(mpVulkanImageView != nullptr);

	// add a sampler only if we need to sample from the texture in shader
	if (mpTexture->GetSamplingType() == Texture::SamplingType::GE_ST_SAMPLING)
	{
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
	}

	UpdateDecriptorInfo();
}

void GADRTexture::Destroy()
{
	mVulkanDescriptorInfo = {};

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
	assert(mpTexture != nullptr);

	if (mpTexture->GetSamplingType() == Texture::SamplingType::GE_ST_SAMPLING)
	{
		assert(mpVulkanSampler != nullptr);
	}
	assert(mpVulkanImageView != nullptr);

	VkSampler samplerHandle = (mpTexture->GetSamplingType() == Texture::SamplingType::GE_ST_SAMPLING ? mpVulkanSampler->GetHandle() : VK_NULL_HANDLE);
	mVulkanDescriptorInfo.sampler = samplerHandle;
	mVulkanDescriptorInfo.imageView = mpVulkanImageView->GetHandle();

	VkImageLayout imageLayout = VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; //default
	if (mpTexture->IsDepthFormat() && mpTexture->GetSamplingType() == Texture::SamplingType::GE_ST_SAMPLING)
	{
		imageLayout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
	}
	mVulkanDescriptorInfo.imageLayout = imageLayout;
}

Texture* GADRTexture::GetTexture()
{
	return mpTexture;
}

VulkanImage* GADRTexture::GetVkImage()
{
	return mpVulkanImage;
}

VulkanImageView* GADRTexture::GetVkImageView()
{
	return mpVulkanImageView;
}

VulkanSampler* GADRTexture::GetVkSampler()
{
	return mpVulkanSampler;
}

const VkDescriptorImageInfo& GADRTexture::GetVkDescriptorInfo() const
{
	return mVulkanDescriptorInfo;
}
#endif // VULKAN_RENDERER