#include "VulkanComputePipeline.hpp"
#include "VulkanDevice.hpp"
#include "VulkanPipelineCache.hpp"
#include "VulkanPipelineLayout.hpp"
#include "VulkanInitializers.hpp"
#include "VulkanHelpers.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanComputePipeline::VulkanComputePipeline()
	: mpDevice(nullptr)
	, mHandle(VK_NULL_HANDLE)
{}

VulkanComputePipeline::VulkanComputePipeline(VulkanDevice* pDevice, VulkanPipelineCache* pCache, const VkPipelineShaderStageCreateInfo& shaderStage,
											 VulkanPipelineLayout* pLayout, VkPipeline basePipelineHandle, int32_t basePipelineIndex, VkPipelineCreateFlags flags)
	: mpDevice(pDevice)
	, mHandle(VK_NULL_HANDLE)
{
	assert(mpDevice != nullptr);
	assert(pCache != nullptr);
	assert(pLayout != nullptr);

	VkComputePipelineCreateInfo computePipelineCreateInfo =
			VulkanInitializers::ComputePipelineCreateInfo(shaderStage, pLayout->GetHandle(), basePipelineHandle, basePipelineIndex, flags);

	Create(pCache, computePipelineCreateInfo);
}

VulkanComputePipeline::~VulkanComputePipeline()
{
	Destroy();
}

void VulkanComputePipeline::Create(VulkanPipelineCache* pCache, const VkComputePipelineCreateInfo& computePipelineCreateInfo)
{
	VK_CHECK_RESULT(vkCreateComputePipelines(mpDevice->GetDeviceHandle(), pCache->GetHandle(), 1, &computePipelineCreateInfo, nullptr, &mHandle));
}

void VulkanComputePipeline::Destroy()
{
	vkDestroyPipeline(mpDevice->GetDeviceHandle(), mHandle, nullptr);
}

const VkPipeline& VulkanComputePipeline::GetHandle() const
{
	return mHandle;
}