#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanTexture.hpp"
#include "Graphics/Rendering/Backends/Vulkan/VulkanRenderer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanDevice.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanImage.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanImageView.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanSampler.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanInitializers.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

GADRTexture::GADRTexture()
	: mpVulkanImage(nullptr)
	, mpVulkanImageView(nullptr)
	, mpVulkanSampler(nullptr)
	, mpTexture(nullptr)
{}

GADRTexture::GADRTexture(Renderer* pRenderer, Texture* pTexture)
	: mpVulkanImage(nullptr)
	, mpVulkanImageView(nullptr)
	, mpVulkanSampler(nullptr)
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

	//TODO

}

void GADRTexture::Destroy()
{
	//TODO
}

VkImageType GADRTexture::TypeToVulkanType(const Texture::TextureType& type) const
{
	VkImageType vulkanImageType = VkImageType::VK_IMAGE_TYPE_MAX_ENUM;

	switch (type)
	{
	case Texture::TextureType::GE_TT_1D:
	case Texture::TextureType::GE_TT_1D_ARRAY:
		vulkanImageType = VkImageType::VK_IMAGE_TYPE_1D;
		break;
	case Texture::TextureType::GE_TT_2D:
	case Texture::TextureType::GE_TT_2D_ARRAY:
	case Texture::TextureType::GE_TT_CUBEMAP: // a cubemap is formed from 6 faces, each face a 2d map
		vulkanImageType = VkImageType::VK_IMAGE_TYPE_2D;
		break;
	case Texture::TextureType::GE_TT_3D:
	case Texture::TextureType::GE_TT_3D_ARRAY:
		vulkanImageType = VkImageType::VK_IMAGE_TYPE_3D;
		break;
	case Texture::TextureType::GE_TT_COUNT:
	default:
		LOG_ERROR("Invalid Vulkan Image Type!");
	}

	return vulkanImageType;
}

VkFormat GADRTexture::FormatToVulkanFormat(const Texture::Format& format) const
{
	return VkFormat::VK_FORMAT_MAX_ENUM; //TODO
}

VkSamplerAddressMode GADRTexture::WrapModeToVulkanWrapMode(const Texture::WrapMode& wrapMode) const
{
	VkSamplerAddressMode vulkanWrapMode = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;

	switch (wrapMode)
	{
	case Texture::WrapMode::GE_WM_REPEAT:
		vulkanWrapMode = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
		break;
	case Texture::WrapMode::GE_WM_MIRROR_REPEAT:
		vulkanWrapMode = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		break;
	case Texture::WrapMode::GE_WM_CLAMP_TO_EDGE:
		vulkanWrapMode = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		break;
	case Texture::WrapMode::GE_WM_MIRROR_CLAMP_TO_EDGE:
		vulkanWrapMode = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
		break;
	case Texture::WrapMode::GE_WM_CLAMP_TO_BORDER:
		vulkanWrapMode = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		break;
	case Texture::WrapMode::GE_WM_COUNT:
		default:
			LOG_ERROR("Invalid Vulkan Wrap Mode!");
	}

	return vulkanWrapMode;
}

VkFilter GADRTexture::FilterModeToVulkanFilterMode(const Texture::FilterMode& filterMode) const
{
	VkFilter vulkanFilterMode = VkFilter::VK_FILTER_MAX_ENUM;
	switch (filterMode)
	{
	case Texture::FilterMode::GE_FM_NEAREST:
		vulkanFilterMode = VkFilter::VK_FILTER_NEAREST;
		break;
	case Texture::FilterMode::GE_FM_LINEAR:
		vulkanFilterMode = VkFilter::VK_FILTER_LINEAR;
		break;
	case Texture::FilterMode::GE_FM_COUNT:
	default:
		LOG_ERROR("Invalid Vulkan Filter Mode!");
	}

	return vulkanFilterMode;
}

VkSamplerMipmapMode GADRTexture::MipMapModeToVulkanMipMapMode(const Texture::MipMapMode& mipMapMode) const
{
	VkSamplerMipmapMode vulkanMipMapMode = VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_MAX_ENUM;
	switch (mipMapMode)
	{
	case Texture::MipMapMode::GE_MM_NEAREST:
		vulkanMipMapMode = VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST;
		break;
	case Texture::MipMapMode::GE_MM_LINEAR:
		vulkanMipMapMode = VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_LINEAR;
		break;
	case Texture::MipMapMode::GE_MM_COUNT:
	default:
		LOG_ERROR("Invalid Vulkan MipMap Mode!");
	}

	return vulkanMipMapMode;
}