#include "VulkanPoolAllocator.hpp"
#include "VulkanDevice.hpp"
#include "VulkanInitializers.hpp"
#include "VulkanHelpers.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanPoolAllocator::VulkanPoolAllocator()
	: VulkanAllocator(nullptr)
	, mPageSize(0)
	, mMemoryBlockMinSize(0)
{}

VulkanPoolAllocator::VulkanPoolAllocator(VulkanDevice* pDevice)
	: VulkanAllocator(pDevice)
	, mPageSize(0)
	, mMemoryBlockMinSize(0)
{
	assert(mpDevice != nullptr);

	const auto& memoryProperties = mpDevice->GetPhysicalDeviceMemoryProperties();

	mMemPools.resize(memoryProperties.memoryTypeCount);

	const auto& properties = mpDevice->GetPhysicalDeviceProperties();

	mPageSize = properties.limits.bufferImageGranularity;
	mMemoryBlockMinSize = mPageSize * 10;
}

VulkanPoolAllocator::~VulkanPoolAllocator()
{
	assert(mpDevice != nullptr);

	for (auto& pool : mMemPools)
	{
		for (auto& block : pool.blocks)
		{
			vkFreeMemory(mpDevice->GetDeviceHandle(), block.mem.handle, nullptr);
		}
	}

	// base destructor is called automatically
}

void VulkanPoolAllocator::Alloc(VkMemoryPropertyFlags usage, uint32_t memoryTypeIndex, VkDeviceSize size, VulkanAllocator::Allocation& outAllocation)
{
	assert(memoryTypeIndex < mMemoryTypeAllocationSizes.size());
	assert(memoryTypeIndex < mMemPools.size());

	mAllocationCount++;

	MemoryPool& pool = mMemPools[memoryTypeIndex];
	//make sure we always alloc a multiple of pageSize
	VkDeviceSize requestedAllocSize = ((size / mPageSize) + 1) * mPageSize;
	mMemoryTypeAllocationSizes[memoryTypeIndex] += requestedAllocSize;

	BlockSpanIndexPair location;

	// NOTE! If a memory usage flag different from VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT is used, then we need a different page
	bool_t needsOwnPage = usage != VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	bool_t found = FindFreeChunkForAllocation(memoryTypeIndex, requestedAllocSize, needsOwnPage, location);

	if (!found)
	{
		location = { AddBlockToPool(requestedAllocSize, memoryTypeIndex, needsOwnPage), 0 };
	}

	auto& block = pool.blocks[location.blockIdx];

	block.pageReserved = needsOwnPage;

	outAllocation.handle = block.mem.handle;
	outAllocation.size = size;
	outAllocation.offset = block.layout[location.spanIdx].offset;
	outAllocation.typeIndex = memoryTypeIndex;
	outAllocation.id = location.blockIdx;

	MarkChunkOfMemoryBlockUsed(memoryTypeIndex, location, requestedAllocSize);
}

void VulkanPoolAllocator::Free(const VulkanAllocator::Allocation& allocation)
{
	assert(mAllocationCount > 0);

	mAllocationCount--;

	// again - the final size - a multiple of page size
	VkDeviceSize requestedAllocSize = ((allocation.size / mPageSize) + 1) * mPageSize;

	OffsetSize span = { allocation.offset, requestedAllocSize };

	MemoryPool& pool = mMemPools[allocation.typeIndex];
	auto& block = pool.blocks[allocation.id];

	block.pageReserved = false;

	bool_t found = false;

	uint32_t numLayoutMems = block.layout.size();
	for (uint32_t j = 0; j < numLayoutMems; ++j)
	{
		auto& layout = block.layout[j];
		if (layout.offset == requestedAllocSize + allocation.offset)
		{
			layout.offset = allocation.offset;
			layout.size += requestedAllocSize;
			found = true;
			break;
		}
	}

	if (!found)
	{
		block.layout.push_back(span);
		mMemoryTypeAllocationSizes[allocation.typeIndex] -= requestedAllocSize;
	}
}

uint32_t VulkanPoolAllocator::AddBlockToPool(VkDeviceSize size, uint32_t memoryTypeIndex, bool_t fitToAlloc)
{
	assert(mpDevice != nullptr);
	assert(memoryTypeIndex < mMemPools.size());

	VkDeviceSize newPoolSize = size * 2;
	newPoolSize = newPoolSize < mMemoryBlockMinSize ? mMemoryBlockMinSize : newPoolSize;

	VkMemoryAllocateInfo memoryAllocateInfo = VulkanInitializers::MemoryAllocateInfo(newPoolSize, memoryTypeIndex);

	DeviceMemoryBlock newBlock = {};
	VK_CHECK_RESULT(vkAllocateMemory(mpDevice->GetDeviceHandle(), &memoryAllocateInfo, nullptr, &newBlock.mem.handle));

	newBlock.mem.typeIndex = memoryTypeIndex;
	newBlock.mem.size = newPoolSize;

	MemoryPool& pool = mMemPools[memoryTypeIndex];
	pool.blocks.push_back(newBlock);

	pool.blocks[pool.blocks.size() - 1].layout.push_back({ 0, newPoolSize });

	//mAllocationCount++;

	return (uint32_t)(pool.blocks.size() - 1);
}

void VulkanPoolAllocator::MarkChunkOfMemoryBlockUsed(uint32_t memoryTypeIndex, const BlockSpanIndexPair& indices, VkDeviceSize size)
{
	assert(memoryTypeIndex < mMemPools.size());

	MemoryPool& pool = mMemPools[memoryTypeIndex];
	auto& layout = pool.blocks[indices.blockIdx].layout[indices.spanIdx];

	layout.offset += size;
	layout.size -= size;
}

bool VulkanPoolAllocator::FindFreeChunkForAllocation(uint32_t memoryTypeIndex, VkDeviceSize size, bool_t needsWholePage, BlockSpanIndexPair& outIndexPair)
{
	assert(memoryTypeIndex < mMemPools.size());

	MemoryPool& pool = mMemPools[memoryTypeIndex];

	for (uint32_t i = 0; i < pool.blocks.size(); ++i)
	{
		auto& block = pool.blocks[i];
		for (uint32_t j = 0; j < block.layout.size(); ++j)
		{
			auto& layout = block.layout[j];
			bool_t validOffset = (needsWholePage ? (layout.offset == 0) : true);
			if ((layout.size >= size) && validOffset)
			{
				outIndexPair.blockIdx = i;
				outIndexPair.spanIdx = j;
				return true;
			}
		}
	}
	return false;
}