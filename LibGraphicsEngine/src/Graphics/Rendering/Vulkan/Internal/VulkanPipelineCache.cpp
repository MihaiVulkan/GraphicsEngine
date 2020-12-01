#include "VulkanPipelineCache.hpp"
#include "VulkanDevice.hpp"
#include "VulkanInitializers.hpp"
#include "VulkanHelpers.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanPipelineCache::VulkanPipelineCache()
	: mpDevice(nullptr)
	, mHandle(VK_NULL_HANDLE)
{}

VulkanPipelineCache::VulkanPipelineCache(VulkanDevice* pDevice, std::vector<void*> initialData, VkPipelineCacheCreateFlags flags)
	: mpDevice(pDevice)
	, mHandle(VK_NULL_HANDLE)
{
	VkPipelineCacheCreateInfo pipelineCacheCreateInfo = 
		VulkanInitializers::PipelineCacheCreateInfo(static_cast<uint32_t>(initialData.size()), initialData.data());

	Create(pipelineCacheCreateInfo);
}

VulkanPipelineCache::~VulkanPipelineCache()
{
	Destroy();
}

void VulkanPipelineCache::Create(const VkPipelineCacheCreateInfo& pipelineCacheCreateInfo)
{
	assert(mpDevice != nullptr);

	VK_CHECK_RESULT(vkCreatePipelineCache(mpDevice->GetDeviceHandle(), &pipelineCacheCreateInfo, nullptr, &mHandle));
}

void VulkanPipelineCache::Destroy()
{
	if (mHandle)
	{
		vkDestroyPipelineCache(mpDevice->GetDeviceHandle(), mHandle, nullptr);
		mHandle = VK_NULL_HANDLE;
	}

	if (mpDevice)
	{
		mpDevice = nullptr;
	}
}

VkResult VulkanPipelineCache::GetData(void* pData, size_t& pSize)
{
	return vkGetPipelineCacheData(mpDevice->GetDeviceHandle(), mHandle, &pSize, pData);
}

VkResult VulkanPipelineCache::Merge(const std::vector<VulkanPipelineCache*>& caches)
{
	std::vector<VkPipelineCache> cacheHandles;
	cacheHandles.resize(caches.size());
	for (size_t i = 0; i < cacheHandles.size(); ++i)
	{
		assert(caches[i] != nullptr);

		cacheHandles[i] = caches[i]->GetHandle();
	}

	return vkMergePipelineCaches(mpDevice->GetDeviceHandle(), mHandle, static_cast<uint32_t>(cacheHandles.size()), cacheHandles.data());
}

const VkPipelineCache& VulkanPipelineCache::GetHandle() const
{
	return mHandle;
}