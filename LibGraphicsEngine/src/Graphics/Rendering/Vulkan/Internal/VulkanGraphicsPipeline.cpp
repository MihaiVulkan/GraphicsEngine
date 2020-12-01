#include "VulkanGraphicsPipeline.hpp"
#include "VulkanDevice.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanPipelineCache.hpp"
#include "VulkanPipelineLayout.hpp"
#include "VulkanInitializers.hpp"
#include "VulkanHelpers.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanGraphicsPipeline::VulkanGraphicsPipeline()
	: mpDevice(nullptr)
	, mHandle(VK_NULL_HANDLE)
{}

VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanDevice* pDevice, VulkanPipelineCache* pCache,
	const std::vector<VkPipelineShaderStageCreateInfo>& shaderStages,
	const VkPipelineVertexInputStateCreateInfo& vertexInputState,
	const VkPipelineInputAssemblyStateCreateInfo& inputAssemblyState,
	const VkPipelineTessellationStateCreateInfo& tessellationState,
	const VkPipelineViewportStateCreateInfo& viewportState,
	const VkPipelineRasterizationStateCreateInfo& rasterizationState,
	const VkPipelineMultisampleStateCreateInfo& multisampleState,
	const VkPipelineDepthStencilStateCreateInfo& depthStencilState,
	const VkPipelineColorBlendStateCreateInfo& colorBlendState,
	const VkPipelineDynamicStateCreateInfo& dynamicState,
	VulkanPipelineLayout*				  pLayout,
	VulkanRenderPass*					  pRenderPass,
	uint32_t                              subpassId,
	VkPipeline                            basePipelineHandle,
	int32_t                               basePipelineIndex,
	VkPipelineCreateFlags                 flags)
	: mpDevice(pDevice)
	, mHandle(VK_NULL_HANDLE)
{
	assert(mpDevice != nullptr);
	assert(pCache != nullptr);
	assert(pLayout != nullptr);
	assert(pRenderPass != nullptr);

	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo =
			VulkanInitializers::GraphicsPipelineCreateInfo(static_cast<uint32_t>(shaderStages.size()), shaderStages.data(), &vertexInputState, &inputAssemblyState,
												&tessellationState, &viewportState, &rasterizationState, &multisampleState, &depthStencilState,
												&colorBlendState, &dynamicState, pLayout->GetHandle(), pRenderPass->GetHandle(), subpassId, basePipelineHandle, basePipelineIndex, flags);

	Create(pCache, graphicsPipelineCreateInfo);
}

VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
{
	Destroy();
}

void VulkanGraphicsPipeline::Create(VulkanPipelineCache* pCache, const VkGraphicsPipelineCreateInfo& graphicsPipelineCreateInfo)
{
	VK_CHECK_RESULT(vkCreateGraphicsPipelines(mpDevice->GetDeviceHandle(), pCache->GetHandle(), 1, &graphicsPipelineCreateInfo, nullptr, &mHandle));
}

void VulkanGraphicsPipeline::Destroy()
{
	vkDestroyPipeline(mpDevice->GetDeviceHandle(), mHandle, nullptr);
}

const VkPipeline& VulkanGraphicsPipeline::GetHandle() const
{
	return mHandle;
}