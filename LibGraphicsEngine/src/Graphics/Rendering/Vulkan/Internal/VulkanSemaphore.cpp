#include "VulkanSemaphore.hpp"
#include "VulkanDevice.hpp"
#include "VulkanInitializers.hpp"
#include "VulkanHelpers.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanSemaphore::VulkanSemaphore()
	: mpDevice(nullptr)
	, mHandle(VK_NULL_HANDLE)
{}

VulkanSemaphore::VulkanSemaphore(VulkanDevice* pDevice, VkSemaphoreCreateFlags flags)
	: mpDevice(pDevice)
	, mHandle(VK_NULL_HANDLE)
{
	Create(flags);
}

VulkanSemaphore::~VulkanSemaphore()
{
	Destroy();
}

void VulkanSemaphore::Create(VkSemaphoreCreateFlags flags)
{
	assert(mpDevice != nullptr);

	VkSemaphoreCreateInfo semaphoreCreateInfo = VulkanInitializers::SemaphoreCreateInfo(flags);

	VK_CHECK_RESULT(vkCreateSemaphore(mpDevice->GetDeviceHandle(), &semaphoreCreateInfo, nullptr, &mHandle));
}

void VulkanSemaphore::Destroy()
{
	assert(mpDevice != nullptr);

	if (mHandle)
	{
		vkDestroySemaphore(mpDevice->GetDeviceHandle(), mHandle, nullptr);
		mHandle = VK_NULL_HANDLE;
	}

	if (mpDevice)
	{
		mpDevice = nullptr;
	}
}

const VkSemaphore& VulkanSemaphore::GetHandle() const
{
	return mHandle;
}