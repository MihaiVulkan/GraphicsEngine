#include "VulkanDescriptorSetLayout.hpp"
#include "VulkanDevice.hpp"
#include "VulkanInitializers.hpp"
#include "VulkanHelpers.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout()
	: mpDevice(nullptr)
	, mHandle(VK_NULL_HANDLE)
{}

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanDevice* pDevice, const std::vector<VkDescriptorSetLayoutBinding> bindings,
														VkDescriptorSetLayoutCreateFlags flags)
	: mpDevice(pDevice)
	, mHandle(VK_NULL_HANDLE)
{
	mBindings = bindings;

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo =
		VulkanInitializers::DescriptorSetLayoutCreateInfo(static_cast<uint32_t>(mBindings.size()), mBindings.data());

	Create(descriptorSetLayoutCreateInfo);
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
{
	Destroy();
}

void VulkanDescriptorSetLayout::Create(const VkDescriptorSetLayoutCreateInfo& descriptorSetLayoutCreateInfo)
{
	assert(mpDevice != nullptr);

	VK_CHECK_RESULT(vkCreateDescriptorSetLayout(mpDevice->GetDeviceHandle(), &descriptorSetLayoutCreateInfo, nullptr, &mHandle));
}

void VulkanDescriptorSetLayout::Destroy()
{
	assert(mpDevice != nullptr);

	mBindings.clear();

	if (mHandle)
	{
		vkDestroyDescriptorSetLayout(mpDevice->GetDeviceHandle(), mHandle, nullptr);
		mHandle = VK_NULL_HANDLE;
	}

	if (mpDevice)
	{
		mpDevice = nullptr;
	}
}

const VkDescriptorSetLayout& VulkanDescriptorSetLayout::GetHandle() const
{
	return mHandle;
}

const std::vector<VkDescriptorSetLayoutBinding>& VulkanDescriptorSetLayout::GetBindings() const
{
	return mBindings;
}