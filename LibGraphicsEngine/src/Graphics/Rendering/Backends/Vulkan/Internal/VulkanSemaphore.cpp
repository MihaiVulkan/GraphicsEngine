#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanSemaphore.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanDevice.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanInitializers.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanHelpers.hpp"
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