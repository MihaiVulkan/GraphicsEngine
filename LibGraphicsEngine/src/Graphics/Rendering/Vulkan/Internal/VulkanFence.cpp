#include "VulkanFence.hpp"
#include "VulkanDevice.hpp"
#include "VulkanInitializers.hpp"
#include "VulkanHelpers.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;


VulkanFence::VulkanFence()
	: mpDevice(nullptr)
	, mHandle(VK_NULL_HANDLE)
{}

VulkanFence::VulkanFence(VulkanDevice* pDevice, VkFenceCreateFlags flags)
	: mpDevice(pDevice)
	, mHandle(VK_NULL_HANDLE)
{
	Create(flags);
}

VulkanFence::~VulkanFence()
{
	Destroy();
}

void VulkanFence::Create(VkFenceCreateFlags flags)
{
	assert(mpDevice != nullptr);

	VkFenceCreateInfo fenceCreateInfo = VulkanInitializers::FenceCreateInfo(flags);

	VK_CHECK_RESULT(vkCreateFence(mpDevice->GetDeviceHandle(), &fenceCreateInfo, nullptr, &mHandle));
}

void VulkanFence::Destroy()
{
	if (mHandle)
	{
		vkDestroyFence(mpDevice->GetDeviceHandle(), mHandle, nullptr);
		mHandle = VK_NULL_HANDLE;
	}

	if (mpDevice)
	{
		mpDevice = nullptr;
	}
}

VkResult VulkanFence::Reset()
{
	return vkResetFences(mpDevice->GetDeviceHandle(), 1, &mHandle);
}

VkResult VulkanFence::WaitIdle(VkBool32 waitAll, uint64_t timeout)
{
	return vkWaitForFences(mpDevice->GetDeviceHandle(), 1, &mHandle, waitAll, timeout);
}

VkResult VulkanFence::GetStatus() const
{
	return vkGetFenceStatus(mpDevice->GetDeviceHandle(), mHandle);
}

const VkFence& VulkanFence::GetHandle() const
{
	return mHandle;
}