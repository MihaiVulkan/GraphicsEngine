#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanQueue.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanInitializers.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanHelpers.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanQueue::VulkanQueue()
	: mHandle(VK_NULL_HANDLE)
	, mTypeFlags(0)
	, mFamilyIndex(0)
	, mIndex(0)
	, mPriority(0)
	, mCreateInfo{}
{}

VulkanQueue::VulkanQueue(VkQueueFlags typeFlags, uint32_t familyIndex, uint32_t index, float32_t priority, VkDeviceQueueCreateFlags flags)
	: mHandle(VK_NULL_HANDLE)
	, mTypeFlags(typeFlags)
	, mFamilyIndex(familyIndex)
	, mIndex(index)
	, mPriority(priority)
	, mCreateInfo{}
{
	Create(flags);
}

VulkanQueue::~VulkanQueue()
{
	Destroy();
}

void VulkanQueue::Create(VkDeviceQueueCreateFlags flags)
{
	// NOTE! A VkQueue is created together with the VkDevice logical device

	mCreateInfo = VulkanInitializers::DeviceQueueCreateInfo(mFamilyIndex, 1, &mPriority, flags);
}

void VulkanQueue::Destroy()
{
	// NOTE! A VkQueue is destroyed together with the VkDevice logical device

	mCreateInfo = {};

	mPriority = 0;
	mIndex = 0;
	mFamilyIndex = 0;
	mTypeFlags = 0;

	if (mHandle)
	{
		mHandle = VK_NULL_HANDLE;
	}
}

void VulkanQueue::Init(VkDevice deviceHandle)
{
	// now we just get the queue handle
	vkGetDeviceQueue(deviceHandle, mFamilyIndex, mIndex, &mHandle);
}

VkResult VulkanQueue::WaitIdle() const
{
	// To wait on the host for the completion of outstanding queue operations for a given queue
	// wait until queue is idle (finish processing queue commands)
	return vkQueueWaitIdle(mHandle);
}

VkResult VulkanQueue::Submit(uint32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fenceHandle) const
{
	// Submits a sequence of semaphores or command buffers to a queue
	return vkQueueSubmit(mHandle, submitCount, pSubmits, fenceHandle);
}

VkResult VulkanQueue::Present(const VkPresentInfoKHR& presentInfo) const
{
	// Return the image to the swap chain for presentation
	return vkQueuePresentKHR(mHandle, &presentInfo);
}

const VkQueue& VulkanQueue::GetHandle() const
{
	return mHandle;
}

VkQueueFlags VulkanQueue::GetTypeFlags() const
{
	return mTypeFlags;
}

uint32_t VulkanQueue::GetFamilyIndex() const
{
	return mFamilyIndex;
}

uint32_t VulkanQueue::GetIndex() const
{
	return mIndex;
}

float32_t VulkanQueue::GetPriority() const
{
	return mPriority;
}

const VkDeviceQueueCreateInfo& VulkanQueue::GetCreateInfo() const
{
	return mCreateInfo;
}