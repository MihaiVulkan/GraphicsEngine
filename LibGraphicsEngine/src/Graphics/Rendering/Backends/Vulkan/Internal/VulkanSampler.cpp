#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanSampler.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanDevice.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanInitializers.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanHelpers.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanSampler::VulkanSampler()
	: mpDevice(nullptr)
	, mHandle(VK_NULL_HANDLE)
	, mSamplerData{}
{}

VulkanSampler::VulkanSampler(VulkanDevice* pDevice,
	VkFilter magFilter, VkFilter minFilter, VkSamplerMipmapMode mipmapMode,
	VkSamplerAddressMode addressModeU, VkSamplerAddressMode addressModeV, VkSamplerAddressMode addressModeW, 
	float32_t mipLodBias, VkBool32 anisotropyEnable, float32_t maxAnisotropy,
	VkBool32 compareEnable, VkCompareOp compareOp, float32_t minLod, float32_t maxLod, 
	VkBorderColor borderColor, VkBool32 unnormalizedCoordinates, VkSamplerCreateFlags flags)
	: mpDevice(pDevice)
	, mHandle(VK_NULL_HANDLE)
	, mSamplerData{}
{
	VkSamplerCreateInfo samplerCreateInfo =
		VulkanInitializers::SamplerCreateInfo(magFilter, minFilter, mipmapMode, addressModeU, addressModeV, addressModeW,
			mipLodBias, anisotropyEnable, maxAnisotropy, compareEnable, compareOp, minLod, maxLod, borderColor, unnormalizedCoordinates, flags);

	Create(samplerCreateInfo);
}

VulkanSampler::~VulkanSampler()
{
	Destroy();
}

void VulkanSampler::Create(const VkSamplerCreateInfo& samplerCreateInfo)
{
	assert(mpDevice != nullptr);

	mSamplerData.magFilter = samplerCreateInfo.magFilter;
	mSamplerData.minFilter = samplerCreateInfo.minFilter;
	mSamplerData.mipmapMode = samplerCreateInfo.mipmapMode;
	mSamplerData.addressModeU = samplerCreateInfo.addressModeU;
	mSamplerData.addressModeV = samplerCreateInfo.addressModeV;
	mSamplerData.addressModeW = samplerCreateInfo.addressModeW;
	mSamplerData.mipLodBias = samplerCreateInfo.mipLodBias;
	mSamplerData.anisotropyEnable = samplerCreateInfo.anisotropyEnable;
	mSamplerData.maxAnisotropy = samplerCreateInfo.maxAnisotropy;
	mSamplerData.compareEnable = samplerCreateInfo.compareEnable;
	mSamplerData.compareOp = samplerCreateInfo.compareOp;
	mSamplerData.minLod = samplerCreateInfo.minLod;
	mSamplerData.maxLod = samplerCreateInfo.maxLod;
	mSamplerData.borderColor = samplerCreateInfo.borderColor;
	mSamplerData.unnormalizedCoordinates = samplerCreateInfo.unnormalizedCoordinates;

	VK_CHECK_RESULT(vkCreateSampler(mpDevice->GetDeviceHandle(), &samplerCreateInfo, nullptr, &mHandle));
}

void VulkanSampler::Destroy()
{
	assert(mpDevice != nullptr);

	if (mHandle)
	{
		vkDestroySampler(mpDevice->GetDeviceHandle(), mHandle, nullptr);
		mHandle = VK_NULL_HANDLE;
	}

	if (mpDevice)
	{
		mpDevice = nullptr;
	}
}

const VkSampler& VulkanSampler::GetHandle() const
{
	return mHandle;
}

const VulkanSampler::Data& VulkanSampler::GetData() const
{
	return mSamplerData;
}