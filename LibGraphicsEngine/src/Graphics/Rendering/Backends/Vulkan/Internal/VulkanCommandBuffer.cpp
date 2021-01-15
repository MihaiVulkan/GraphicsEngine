#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanCommandBuffer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanDevice.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanQueue.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanFence.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanInitializers.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanHelpers.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanCommandBuffer::VulkanCommandBuffer()
	: mpDevice(nullptr)
	, mCommandPoolHandle(VK_NULL_HANDLE)
	, mHandle(VK_NULL_HANDLE)
{}

VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice* pDevice, VkCommandPool commandPoolHandle, VkCommandBufferLevel level, bool_t begin)
	: mpDevice(pDevice)
	, mCommandPoolHandle(commandPoolHandle)
	, mHandle(VK_NULL_HANDLE)
{
	Create(level, begin);
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
	Destroy();
}

void VulkanCommandBuffer::Create(VkCommandBufferLevel level, bool_t begin)
{
	assert(mpDevice != nullptr);
	assert(mCommandPoolHandle != VK_NULL_HANDLE);

	VkCommandBufferAllocateInfo commandBufferAllocateInfo = VulkanInitializers::CommandBufferAllocateInfo(mCommandPoolHandle, level, 1);

	VK_CHECK_RESULT(vkAllocateCommandBuffers(mpDevice->GetDeviceHandle(), &commandBufferAllocateInfo, &mHandle));

	// If requested, also start the new command buffer
	if (begin)
	{
		VK_CHECK_RESULT(Begin());
	}
}

void VulkanCommandBuffer::Destroy()
{
	assert(mpDevice != nullptr);

	if (mHandle)
	{
		vkFreeCommandBuffers(mpDevice->GetDeviceHandle(), mCommandPoolHandle, 1, &mHandle);
		mHandle = VK_NULL_HANDLE;
	}
	mCommandPoolHandle = VK_NULL_HANDLE;
	
	if (mpDevice)
	{
		mpDevice = nullptr;
	}
}

VkResult VulkanCommandBuffer::Begin(const VkCommandBufferInheritanceInfo* pInheritanceInfo)
{
	VkCommandBufferBeginInfo commandBufferBeginInfo = VulkanInitializers::CommandBufferBeginInfo(pInheritanceInfo);

	return vkBeginCommandBuffer(mHandle, &commandBufferBeginInfo);
}

VkResult VulkanCommandBuffer::End()
{
	return vkEndCommandBuffer(mHandle);
}

void VulkanCommandBuffer::Flush(VulkanQueue* pQueue)
{
	assert(mpDevice != nullptr);
	assert(pQueue != nullptr);

	VK_CHECK_RESULT(End());

	VkSubmitInfo submitInfo = VulkanInitializers::SubmitInfo(1, &mHandle);

	{
		//// Create fence to ensure that the command buffer has finished executing
		VulkanFence fence(mpDevice);

		//// Submit to the queue
		VK_CHECK_RESULT(pQueue->Submit(1, &submitInfo, fence.GetHandle()));

		//// Wait for the fence to signal that command buffer has finished executing
		VK_CHECK_RESULT(fence.WaitIdle(VK_TRUE, DEFAULT_FENCE_TIMEOUT));
	}
}

VkResult VulkanCommandBuffer::Reset(VkCommandBufferResetFlags flags)
{
	return vkResetCommandBuffer(mHandle, flags);
}

void VulkanCommandBuffer::ExecuteSecondaryCommandBuffers(const std::vector<VulkanCommandBuffer*>& secondaryCommandBuffers)
{
	// the secondary command buffers are executed as par tof the main command buffers
	std::vector<VkCommandBuffer> commandBuffers;
	commandBuffers.resize(secondaryCommandBuffers.size());

	for (size_t i = 0; i < commandBuffers.size(); ++ i)
	{
		commandBuffers[i] = secondaryCommandBuffers[i]->GetHandle();
	}

	vkCmdExecuteCommands(mHandle, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
}

const VkCommandBuffer& VulkanCommandBuffer::GetHandle() const
{
	return mHandle;
}