#include "VulkanAllocation.hpp"


VulkanAllocation::VulkanAllocation()
	: mHandle(VK_NULL_HANDLE)
	, mTypeIndex(0)
	, mId(0)
	, mSize(0)
	, mOffset(0)
{}

VulkanAllocation::VulkanAllocation(VkDeviceMemory handle, uint32_t typeIndex, uint32_t id, VkDeviceSize size, VkDeviceSize offset)
	: mHandle(handle)
	, mTypeIndex(typeIndex)
	, mId(id)
	, mSize(size)
	, mOffset(offset)
{}

VulkanAllocation::~VulkanAllocation()
{
	mOffset = 0;
	mSize = 0;
	mId = 0;
	mTypeIndex = 0;

	if (mHandle)
	{
		mHandle = VK_NULL_HANDLE;
	}
}

const VkDeviceMemory& VulkanAllocation::GetHandle() const
{
	return mHandle;
}

uint32_t VulkanAllocation::GetTypeIndex() const
{
	return mTypeIndex;
}

uint32_t VulkanAllocation::GetId() const
{
	return mId;
}

VkDeviceSize VulkanAllocation::GetSize() const
{
	return mSize;
}

VkDeviceSize VulkanAllocation::GetOffset() const
{
	return mOffset;
}

void VulkanAllocation::SetHandle(VkDeviceMemory handle)
{
	mHandle = handle;
}

void VulkanAllocation::SetTypeIndex(uint32_t typeIndex)
{
	mTypeIndex = typeIndex;
}

void VulkanAllocation::SetId(uint32_t id)
{
	mId = id;
}

void VulkanAllocation::SetSize(VkDeviceSize size)
{
	mSize = size;
}

void VulkanAllocation::SetOffset(VkDeviceSize offset)
{
	mOffset = offset;
}