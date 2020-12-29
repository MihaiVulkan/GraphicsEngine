#include "VulkanBuffer.hpp"
#include "VulkanDevice.hpp"
#include "VulkanQueue.hpp"
#include "VulkanCommandPool.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanPassThroughAllocator.hpp"
#include "VulkanInitializers.hpp"
#include "VulkanHelpers.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanBuffer::VulkanBuffer()
	: mpDevice(nullptr)
	, mHandle(VK_NULL_HANDLE)
	, mAllocation()
	, mDefaultDescriptorInfo{}
	, mSize(0)
	, mpMappedData(nullptr)
{}

VulkanBuffer::VulkanBuffer(VulkanDevice* pDevice, VkMemoryPropertyFlags memoryPropertyFlags, VkBufferUsageFlags usageFlags, VkDeviceSize size,
							void* pData, VkSharingMode sharingMode, uint32_t queueFamilyIndexCount, const uint32_t* pQueueFamilyIndices,
							VkBufferCreateFlags flags)
	: mpDevice(pDevice)
	, mHandle(VK_NULL_HANDLE)
	, mAllocation()
	, mDefaultDescriptorInfo{}
	, mSize(size)
	, mpMappedData(nullptr)
{
	VkBufferCreateInfo bufferCreateInfo = VulkanInitializers::BufferCreateInfo(size, usageFlags, sharingMode, queueFamilyIndexCount, pQueueFamilyIndices, flags);

	Create(bufferCreateInfo, memoryPropertyFlags, pData);
}

VulkanBuffer::~VulkanBuffer()
{
	Destroy();
}

void VulkanBuffer::Create(const VkBufferCreateInfo& bufferCreateInfo, VkMemoryPropertyFlags memoryPropertyFlags, void* pData)
{
	assert(mpDevice != nullptr);
	assert(mpDevice->GetAllocator() != nullptr);

	VK_CHECK_RESULT(vkCreateBuffer(mpDevice->GetDeviceHandle(), &bufferCreateInfo, nullptr, &mHandle));

	VkMemoryRequirements memReqs;
	vkGetBufferMemoryRequirements(mpDevice->GetDeviceHandle(), mHandle, &memReqs);

	uint32_t typeIndex = 0;
	assert(mpDevice->GetMemoryTypeFromProperties(memReqs.memoryTypeBits, memoryPropertyFlags, typeIndex) == true);

	// Alloc
	mpDevice->GetAllocator()->Alloc(memoryPropertyFlags, typeIndex, memReqs.size, mAllocation);

	// if valid data available, we map it and copy to the buffer
	if (pData)
	{
		VK_CHECK_RESULT(Map(mAllocation.size));

		SetData(pData, bufferCreateInfo.size);

		UnMap();
	}

	VK_CHECK_RESULT(Bind(mAllocation.offset));

	SetDecriptorInfo();
}

void VulkanBuffer::Destroy()
{
	assert(mpDevice != nullptr);

	if (mpMappedData)
	{
		mpMappedData = nullptr;
	}

	if (mSize > 0)
	{
		mSize = 0;
	}

	mDefaultDescriptorInfo = {};

	// Free
	mpDevice->GetAllocator()->Free(mAllocation);

	if (mHandle)
	{
		vkDestroyBuffer(mpDevice->GetDeviceHandle(), mHandle, nullptr);
		mHandle = VK_NULL_HANDLE;
	}

	if (mpDevice)
	{
		mpDevice = nullptr;
	}
}

VkResult VulkanBuffer::Map(VkDeviceSize size, VkDeviceSize offset, VkMemoryMapFlags flags)
{
	assert(mpDevice != nullptr);
	// important to use memory property VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT

	return vkMapMemory(mpDevice->GetDeviceHandle(), mAllocation.handle, offset, size, flags, &mpMappedData);
}

void VulkanBuffer::UnMap()
{
	assert(mpDevice != nullptr);

	// no return as UnMap can not fail
	vkUnmapMemory(mpDevice->GetDeviceHandle(), mAllocation.handle);
}

void VulkanBuffer::SetData(void* pData, VkDeviceSize size)
{
	assert(pData != nullptr);
	assert(size > 0);
	assert(mpMappedData != nullptr);

	::memcpy(mpMappedData, pData, size);
}

VkResult VulkanBuffer::Bind(VkDeviceSize offset)
{
	assert(mpDevice != nullptr);

	return vkBindBufferMemory(mpDevice->GetDeviceHandle(), mHandle, mAllocation.handle, offset);
}

void VulkanBuffer::CopyTo(VulkanBuffer* pDestBuffer, VulkanQueue* pQueue, VkBufferCopy* pCopyRegion)
{
	assert(mpDevice != nullptr);
	assert(pDestBuffer != nullptr);
	assert(pDestBuffer->GetSize() <= mSize);
	assert(pQueue != nullptr);

	VulkanCommandPool copyCommandPool(mpDevice, pQueue->GetFamilyIndex());
	VulkanCommandBuffer copyCommandBuffer(mpDevice, copyCommandPool.GetHandle(), VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

	VkBufferCopy bufferCopy {};
	if (pCopyRegion == nullptr)
	{
		bufferCopy.size = mSize;
	}
	else
	{
		assert(pCopyRegion->size > 0);
		bufferCopy = *pCopyRegion;
	}

	vkCmdCopyBuffer(copyCommandBuffer.GetHandle(), mHandle, pDestBuffer->GetHandle(), 1, &bufferCopy);

	copyCommandBuffer.Flush(pQueue);
}

void VulkanBuffer::SetDecriptorInfo(VkDeviceSize size, VkDeviceSize offset)
{
	mDefaultDescriptorInfo.buffer = mHandle;
	mDefaultDescriptorInfo.range = size;
	mDefaultDescriptorInfo.offset = offset;
}

const VkBuffer& VulkanBuffer::GetHandle() const
{
	return mHandle;
}

VkDeviceSize VulkanBuffer::GetSize() const
{
	return mSize;
}

void* VulkanBuffer::GetData() const
{
	return mpMappedData;
}

const VkDescriptorBufferInfo& VulkanBuffer::GetDescriptorInfo() const
{
	return mDefaultDescriptorInfo;
}