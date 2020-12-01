#include "VulkanDescriptorPool.hpp"
#include "VulkanDevice.hpp"
#include "VulkanInitializers.hpp"
#include "VulkanHelpers.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanDescriptorPool::VulkanDescriptorPool()
	: mpDevice(nullptr)
	, mHandle(VK_NULL_HANDLE)
{}

VulkanDescriptorPool::VulkanDescriptorPool(VulkanDevice* pDevice, uint32_t maxSets, const std::vector<VkDescriptorPoolSize>& poolSizes, VkDescriptorPoolCreateFlags flags)
	: mpDevice(pDevice)
	, mHandle(VK_NULL_HANDLE)
{
	/*mPoolSizes.resize(poolSizes.size());
	for (size_t i = 0;i < mPoolSizes.size(); ++i)
	{
		mPoolSizes[i] = poolSizes[i];
	}*/
	mPoolSizes = poolSizes;


	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo =
		VulkanInitializers::DescriptorPoolCreateInfo(maxSets, static_cast<uint32_t>(mPoolSizes.size()), mPoolSizes.data(), flags);

	Create(descriptorPoolCreateInfo);
}

VulkanDescriptorPool::~VulkanDescriptorPool()
{
	Destroy();
}

void VulkanDescriptorPool::Create(const VkDescriptorPoolCreateInfo& descriptorPoolCreateInfo)
{
	assert(mpDevice != nullptr);

	VK_CHECK_RESULT(vkCreateDescriptorPool(mpDevice->GetDeviceHandle(), &descriptorPoolCreateInfo, nullptr, &mHandle));
}

void VulkanDescriptorPool::Destroy()
{
	mPoolSizes.clear();

	if (mHandle)
	{
		vkDestroyDescriptorPool(mpDevice->GetDeviceHandle(), mHandle, nullptr);
		mHandle = VK_NULL_HANDLE;
	}

	if (mpDevice)
	{
		mpDevice = nullptr;
	}
}

VkResult VulkanDescriptorPool::Reset(VkDescriptorPoolResetFlags flags)
{
	return vkResetDescriptorPool(mpDevice->GetDeviceHandle(), mHandle, flags);
}

const VkDescriptorPool& VulkanDescriptorPool::GetHandle() const
{
	return mHandle;
}

const std::vector<VkDescriptorPoolSize>& VulkanDescriptorPool::GetPoolSizes() const
{
	return mPoolSizes;
}