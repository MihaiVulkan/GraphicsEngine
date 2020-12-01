#include "AppConfig.hpp"
#include "VulkanQueryPool.hpp"
#include "VulkanDevice.hpp"
#include "VulkanLogicalDevice.hpp"
#include "VulkanInitializers.hpp"
#include "VulkanHelpers.hpp"
#include "Foundation/Platform/Platform.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

#define QUERY_ID 0

VulkanQueryPool::VulkanQueryPool()
	: mpDevice(nullptr)
	, mHandle(VK_NULL_HANDLE)
{}

VulkanQueryPool::VulkanQueryPool(VulkanDevice* pDevice, VkQueryType queryType, uint32_t queryCount, VkQueryPipelineStatisticFlags pipelineStatistics, VkQueryPoolCreateFlags flags)
	: mpDevice(pDevice)
	, mHandle(VK_NULL_HANDLE)
{
	VkQueryPoolCreateInfo queryPoolInfo = VulkanInitializers::QueryPoolCreateInfo(queryType, queryCount, pipelineStatistics, flags);

	Create(queryPoolInfo);
}

VulkanQueryPool::~VulkanQueryPool()
{
	Destroy();
}

void VulkanQueryPool::Create(const VkQueryPoolCreateInfo& queryPoolInfo)
{
	assert(mpDevice != nullptr);

	VK_CHECK_RESULT(vkCreateQueryPool(mpDevice->GetDeviceHandle(), &queryPoolInfo, nullptr, &mHandle));
}

void VulkanQueryPool::Destroy()
{
	if (mHandle)
	{
		vkDestroyQueryPool(mpDevice->GetDeviceHandle(), mHandle, nullptr);
		mHandle = VK_NULL_HANDLE;
	}

	if (mpDevice)
	{
		mpDevice = nullptr;
	}
}

void VulkanQueryPool::BeginQuery(VkCommandBuffer commandBufferHandle)
{
	vkCmdBeginQuery(commandBufferHandle, mHandle, QUERY_ID, 0);
}

void VulkanQueryPool::EndQuery(VkCommandBuffer commandBufferHandle)
{
	vkCmdEndQuery(commandBufferHandle, mHandle, QUERY_ID);
}

void VulkanQueryPool::ResetQuery(VkCommandBuffer commandBufferHandle, uint32_t queryCount)
{
	//vkResetQueryPool(mpDevice->GetDeviceHandle(), mHandle, QUERY_ID, queryCount);

	vkCmdResetQueryPool(commandBufferHandle, mHandle, QUERY_ID, queryCount);
}

void VulkanQueryPool::GetQueryResults(uint32_t queryCount, void* pData, size_t dataSize, VkDeviceSize stride, VkQueryResultFlags flags)
{
	assert(mpDevice != nullptr);

//	VK_CHECK_RESULT(vkGetQueryPoolResults(mpDevice->GetDeviceHandle(), mHandle, QUERY_ID, queryCount, dataSize, pData, stride, flags));

	// TODO - for now we get VK_NOT_READY as response
	vkGetQueryPoolResults(mpDevice->GetDeviceHandle(), mHandle, QUERY_ID, queryCount, dataSize, pData, stride, flags);
}

const VkQueryPool& VulkanQueryPool::GetHandle() const
{
	return mHandle;
}