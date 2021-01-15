#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanPassThroughAllocator.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanDevice.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanInitializers.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanHelpers.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanPassThroughAllocator::VulkanPassThroughAllocator()
	: VulkanAllocator()
{}

VulkanPassThroughAllocator::VulkanPassThroughAllocator(VulkanDevice* pDevice)
	: VulkanAllocator(pDevice)
{}


VulkanPassThroughAllocator::~VulkanPassThroughAllocator()
{
	// base destructor is called automatically
}

void VulkanPassThroughAllocator::Alloc(VkMemoryPropertyFlags usage, uint32_t memoryTypeIndex, VkDeviceSize size, VulkanAllocator::Allocation& outAllocation)
{
	assert(mpDevice != nullptr);
	assert(memoryTypeIndex < mMemoryTypeAllocationSizes.size());

	mAllocationCount++;

	mMemoryTypeAllocationSizes[memoryTypeIndex] += size;

	VkMemoryAllocateInfo memoryAllocateInfo = VulkanInitializers::MemoryAllocateInfo(size, memoryTypeIndex);

	VK_CHECK_RESULT(vkAllocateMemory(mpDevice->GetDeviceHandle(), &memoryAllocateInfo, nullptr, &outAllocation.handle));

	outAllocation.typeIndex = (memoryTypeIndex);
	outAllocation.size = size;
	outAllocation.offset = 0;
}

void VulkanPassThroughAllocator::Free(const VulkanAllocator::Allocation& allocation)
{
	assert(mpDevice != nullptr);
	assert(mAllocationCount > 0);

	mAllocationCount--;

	mMemoryTypeAllocationSizes[allocation.typeIndex] -= allocation.size;

	vkFreeMemory(mpDevice->GetDeviceHandle(), allocation.handle, nullptr);
}