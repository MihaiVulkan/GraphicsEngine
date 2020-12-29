#include "VulkanCommandPool.hpp"
#include "VulkanDevice.hpp"
#include "VulkanInitializers.hpp"
#include "VulkanHelpers.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanCommandPool::VulkanCommandPool()
	: mpDevice(nullptr)
	, mHandle(VK_NULL_HANDLE)
	, mQueueFamilyIndex(0)
{}

VulkanCommandPool::VulkanCommandPool(VulkanDevice* pDevice, uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags)
	: mpDevice(pDevice)
	, mHandle(VK_NULL_HANDLE)
	, mQueueFamilyIndex(queueFamilyIndex)
{
	VkCommandPoolCreateInfo commandPoolCreateInfo = VulkanInitializers::CommandPoolCreateInfo(queueFamilyIndex, flags);

	Create(commandPoolCreateInfo);
}
VulkanCommandPool::~VulkanCommandPool()
{
	Destroy();
}

void VulkanCommandPool::Create(const VkCommandPoolCreateInfo& commandPoolCreateInfo)
{
	assert(mpDevice != nullptr);

	VK_CHECK_RESULT(vkCreateCommandPool(mpDevice->GetDeviceHandle(), &commandPoolCreateInfo, nullptr, &mHandle));
}

void VulkanCommandPool::Destroy()
{
	assert(mpDevice != nullptr);

	if (mHandle)
	{
		vkDestroyCommandPool(mpDevice->GetDeviceHandle(), mHandle, nullptr);
		mHandle = VK_NULL_HANDLE;
	}

	if (mpDevice)
	{
		mpDevice = nullptr;
	}
}

VkResult VulkanCommandPool::Reset(VkCommandPoolResetFlags flags)
{
	assert(mpDevice != nullptr);

	return vkResetCommandPool(mpDevice->GetDeviceHandle(), mHandle, flags);
}

void VulkanCommandPool::Trim(VkCommandPoolTrimFlags flags)
{
	assert(mpDevice != nullptr);

	vkTrimCommandPool(mpDevice->GetDeviceHandle(), mHandle, flags);
}

const VkCommandPool& VulkanCommandPool::GetHandle() const
{
	return mHandle;
}

uint32_t VulkanCommandPool::GetQueueFamilyIndex() const
{
	return mQueueFamilyIndex;
}