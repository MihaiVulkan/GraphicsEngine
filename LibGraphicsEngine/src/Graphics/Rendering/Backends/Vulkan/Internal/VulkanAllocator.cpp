#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanAllocator.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanDevice.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanAllocator::VulkanAllocator()
	: mpDevice(nullptr)
	, mTotalMemorySize(0)
	, mTotalMemoryUsed(0)
	, mAllocationCount(0)
{

}

VulkanAllocator::VulkanAllocator(VulkanDevice* pDevice)
		: mpDevice(pDevice)
		, mTotalMemorySize(0)
		, mTotalMemoryUsed(0)
		, mAllocationCount(0)
{
	assert(mpDevice != nullptr);

	const auto& memoryProperties = mpDevice->GetPhysicalDeviceMemoryProperties();

	mMemoryTypeAllocationSizes.resize(memoryProperties.memoryTypeCount);
}

VulkanAllocator::~VulkanAllocator()
{
	mAllocationCount = 0;
	mMemoryTypeAllocationSizes.clear();
	mTotalMemoryUsed = 0;
	mTotalMemorySize = 0;

	if (mpDevice)
	{
		mpDevice = nullptr;
	}
}

uint32_t VulkanAllocator::GetTotalMemorySize() const
{
	return mTotalMemorySize;
}

uint32_t VulkanAllocator::GetTotalMemoryUsed() const
{
	return mTotalMemoryUsed;
}

const std::vector<uint32_t>& VulkanAllocator::GetMemoryTypeAllocationSizes() const
{ 
	return mMemoryTypeAllocationSizes;
}

uint32_t VulkanAllocator::GetAllocatedSize(uint32_t memoryType) const
{
	assert(memoryType < mMemoryTypeAllocationSizes.size());

	return mMemoryTypeAllocationSizes[memoryType];
}

uint32_t VulkanAllocator::GetAllocationCount() const
{
	return mAllocationCount; 
}