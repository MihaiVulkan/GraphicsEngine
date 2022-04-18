#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanComputePipeline.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanDevice.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanPipelineCache.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanPipelineLayout.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanInitializers.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanHelpers.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanComputePipeline::VulkanComputePipeline()
	: mpDevice(nullptr)
	, mHandle(VK_NULL_HANDLE)
{}

VulkanComputePipeline::VulkanComputePipeline(VulkanDevice* pDevice, VkPipelineCache cacheHandle, const VkPipelineShaderStageCreateInfo& shaderStage,
											 VkPipelineLayout layoutHandle, VkPipeline basePipelineHandle, int32_t basePipelineIndex, VkPipelineCreateFlags flags)
	: mpDevice(pDevice)
	, mHandle(VK_NULL_HANDLE)
{
	assert(mpDevice != nullptr);
	assert(cacheHandle != VK_NULL_HANDLE);
	assert(layoutHandle != VK_NULL_HANDLE);

	VkComputePipelineCreateInfo computePipelineCreateInfo =
			VulkanInitializers::ComputePipelineCreateInfo(shaderStage, layoutHandle, basePipelineHandle, basePipelineIndex, flags);

	Create(cacheHandle, computePipelineCreateInfo);
}

VulkanComputePipeline::~VulkanComputePipeline()
{
	Destroy();
}

void VulkanComputePipeline::Create(VkPipelineCache cacheHandle, const VkComputePipelineCreateInfo& computePipelineCreateInfo)
{
	assert(mpDevice != nullptr);
	assert(cacheHandle != VK_NULL_HANDLE);

	VK_CHECK_RESULT(vkCreateComputePipelines(mpDevice->GetDeviceHandle(), cacheHandle, 1, &computePipelineCreateInfo, nullptr, &mHandle));
}

void VulkanComputePipeline::Destroy()
{
	assert(mpDevice != nullptr);

	vkDestroyPipeline(mpDevice->GetDeviceHandle(), mHandle, nullptr);
}

void VulkanComputePipeline::Bind(VkCommandBuffer commandBufferHandle, VkPipelineBindPoint pipelineBindPoint)
{
	vkCmdBindPipeline(commandBufferHandle, pipelineBindPoint, mHandle);
}

const VkPipeline& VulkanComputePipeline::GetHandle() const
{
	return mHandle;
}