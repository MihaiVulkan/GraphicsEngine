#include "VulkanPipelineLayout.hpp"
#include "VulkanDevice.hpp"
#include "VulkanDescriptorSetLayout.hpp"
#include "VulkanInitializers.hpp"
#include "VulkanHelpers.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanPipelineLayout::VulkanPipelineLayout()
	: mpDevice(nullptr)
	, mHandle(VK_NULL_HANDLE)
{}

VulkanPipelineLayout::VulkanPipelineLayout(VulkanDevice* pDevice, const std::vector<VulkanDescriptorSetLayout*>& descriptorSetLayouts, const std::vector<VkPushConstantRange>& pushConstantRanges,
												VkPipelineLayoutCreateFlags flags)
	: mpDevice(pDevice)
	, mHandle(VK_NULL_HANDLE)
{
	std::vector<VkDescriptorSetLayout> descriptorLayoutHandles;
	descriptorLayoutHandles.resize(descriptorSetLayouts.size());
	for (size_t i = 0;i < descriptorLayoutHandles.size(); ++ i)
	{
		assert(descriptorSetLayouts[i] != nullptr);
		descriptorLayoutHandles[i] = descriptorSetLayouts[i]->GetHandle();
	}

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo =
		VulkanInitializers::PipelineLayoutCreateInfo(static_cast<uint32_t>(descriptorLayoutHandles.size()), descriptorLayoutHandles.data(),	
													 static_cast<uint32_t>(pushConstantRanges.size()), pushConstantRanges.data());

	Create(pipelineLayoutCreateInfo);
}

VulkanPipelineLayout::~VulkanPipelineLayout()
{
	Destroy();
}

void VulkanPipelineLayout::Create(const VkPipelineLayoutCreateInfo& pipelineLayoutCreateInfo)
{
	assert(mpDevice != nullptr);

	VK_CHECK_RESULT(vkCreatePipelineLayout(mpDevice->GetDeviceHandle(), &pipelineLayoutCreateInfo, nullptr, &mHandle));
}

void VulkanPipelineLayout::Destroy()
{
	if (mHandle)
	{
		vkDestroyPipelineLayout(mpDevice->GetDeviceHandle(), mHandle, nullptr);
		mHandle = VK_NULL_HANDLE;
	}

	if (mpDevice)
	{
		mpDevice = nullptr;
	}
}

const VkPipelineLayout& VulkanPipelineLayout::GetHandle() const
{
	return mHandle;
}