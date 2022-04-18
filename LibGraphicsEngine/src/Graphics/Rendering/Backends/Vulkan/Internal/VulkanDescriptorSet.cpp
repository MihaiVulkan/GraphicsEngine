#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanDescriptorSet.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanDevice.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanDescriptorPool.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanDescriptorSetLayout.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanInitializers.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanHelpers.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanDescriptorSet::VulkanDescriptorSet()
	: mpDevice(nullptr)
	, mpDescriptorPool(nullptr)
	, mId(0)
	, mHandle(VK_NULL_HANDLE)
{}

VulkanDescriptorSet::VulkanDescriptorSet(VulkanDevice* pDevice, VulkanDescriptorPool* pDescriptorPool, uint32_t id, const std::vector<VulkanDescriptorSetLayout*>& setLayouts)
	: mpDevice(pDevice)
	, mpDescriptorPool(pDescriptorPool)
	, mId(id)
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
		assert(layouts[i] != nullptr);

		mLayouts[i] = layouts[i];
		layoutHandles[i] = mLayouts[i]->GetHandle();
	}

	VkDescriptorSetAllocateInfo descriptorSetAllocInfo =
		VulkanInitializers::DescriptorSetAllocateInfo(mpDescriptorPool->GetHandle(), static_cast<uint32_t>(layoutHandles.size()), layoutHandles.data());

	VK_CHECK_RESULT(vkAllocateDescriptorSets(mpDevice->GetDeviceHandle(), &descriptorSetAllocInfo, &mHandle));
}

void VulkanDescriptorSet::Destroy()
{
	assert(mpDevice != nullptr);
	assert(mpDescriptorPool != nullptr);

	mLayouts.clear();

	if (mHandle)
	{
		VK_CHECK_RESULT(vkFreeDescriptorSets(mpDevice->GetDeviceHandle(), mpDescriptorPool->GetHandle(), 1, &mHandle));
		mHandle = VK_NULL_HANDLE;
	}

	mId = 0;

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
	assert(mpDevice != nullptr);

	vkUpdateDescriptorSets(mpDevice->GetDeviceHandle(), static_cast<uint32_t>(writeSet.size()), writeSet.data(),
														static_cast<uint32_t>(copySet.size()), copySet.data());
}

void VulkanDescriptorSet::Bind(VkCommandBuffer commandBufferHandle, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout pipelineLayoutHandle)
{
	vkCmdBindDescriptorSets(commandBufferHandle, pipelineBindPoint, pipelineLayoutHandle, mId, 1, &mHandle, 0, nullptr);
}

const uint32_t& VulkanDescriptorSet::GetId() const
{
	return mId;
}

const VkDescriptorSet& VulkanDescriptorSet::GetHandle() const
{
	return mHandle;
}

const std::vector<VulkanDescriptorSetLayout*>& VulkanDescriptorSet::GetLayouts() const
{
	return mLayouts;
}