#include "VulkanDescriptorSet.hpp"
#include "VulkanDevice.hpp"
#include "VulkanDescriptorPool.hpp"
#include "VulkanDescriptorSetLayout.hpp"
#include "VulkanInitializers.hpp"
#include "VulkanHelpers.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanDescriptorSet::VulkanDescriptorSet()
	: mpDevice(nullptr)
	, mpDescriptorPool(nullptr)
	, mHandle(VK_NULL_HANDLE)
{}

VulkanDescriptorSet::VulkanDescriptorSet(VulkanDevice* pDevice, VulkanDescriptorPool* pDescriptorPool, const std::vector<VulkanDescriptorSetLayout*>& setLayouts)
	: mpDevice(pDevice)
	, mpDescriptorPool(pDescriptorPool)
	, mHandle(VK_NULL_HANDLE)
{
	Create(setLayouts);
}

VulkanDescriptorSet::~VulkanDescriptorSet()
{
	Destroy();
}

void VulkanDescriptorSet::Create(const std::vector<VulkanDescriptorSetLayout*>& layouts)
{
	assert(mpDevice != nullptr);
	assert(mpDescriptorPool != nullptr);

	mLayouts.resize(layouts.size());
	std::vector<VkDescriptorSetLayout> layoutHandles;
	layoutHandles.resize(mLayouts.size());
	for (size_t i = 0; i < layouts.size(); ++i)
	{
		mLayouts[i] = layouts[i];
		layoutHandles[i] = mLayouts[i]->GetHandle();
	}

	VkDescriptorSetAllocateInfo descriptorSetAllocInfo =
		VulkanInitializers::DescriptorSetAllocateInfo(mpDescriptorPool->GetHandle(), static_cast<uint32_t>(layoutHandles.size()), layoutHandles.data());

	VK_CHECK_RESULT(vkAllocateDescriptorSets(mpDevice->GetDeviceHandle(), &descriptorSetAllocInfo, &mHandle));
}

void VulkanDescriptorSet::Destroy()
{
	mLayouts.clear();

	if (mHandle)
	{
		vkFreeDescriptorSets(mpDevice->GetDeviceHandle(), mpDescriptorPool->GetHandle(), 1, &mHandle);
		mHandle = VK_NULL_HANDLE;
	}

	if (mpDescriptorPool)
	{
		mpDescriptorPool = nullptr;
	}

	if (mpDevice)
	{
		mpDevice = nullptr;
	}
}

void VulkanDescriptorSet::Update(const std::vector<VkWriteDescriptorSet>& writeSet, const std::vector<VkCopyDescriptorSet>& copySet)
{
	vkUpdateDescriptorSets(mpDevice->GetDeviceHandle(), static_cast<uint32_t>(writeSet.size()), writeSet.data(),
														static_cast<uint32_t>(copySet.size()), copySet.data());
}

const VkDescriptorSet& VulkanDescriptorSet::GetHandle() const
{
	return mHandle;
}

const std::vector<VulkanDescriptorSetLayout*>& VulkanDescriptorSet::GetLayouts() const
{
	return mLayouts;
}