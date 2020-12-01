#include "VulkanRenderPass.hpp"
#include "VulkanDevice.hpp"
#include "VulkanRenderPassAttachment.hpp"
#include "VulkanSubPass.hpp"
#include "VulkanInitializers.hpp"
#include "VulkanHelpers.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanRenderPass::VulkanRenderPass()
	: mpDevice(nullptr)
	, mRenderPassHandle(VK_NULL_HANDLE)
{}

VulkanRenderPass::VulkanRenderPass(VulkanDevice* pDevice, const std::vector<VulkanRenderPassAttachment*>& attachments, const std::vector<VulkanSubPass*>& subPasses,
									const std::vector<VkSubpassDependency>& subPassDependencies)
	: mpDevice(pDevice)
	, mRenderPassHandle(VK_NULL_HANDLE)
{
	Create(attachments, subPasses, subPassDependencies);
}

VulkanRenderPass::~VulkanRenderPass()
{
	Destroy();
}

void VulkanRenderPass::Create(const std::vector<VulkanRenderPassAttachment*>& attachments, const std::vector<VulkanSubPass*>& subPasses,
							const std::vector<VkSubpassDependency>& subPassDependencies)
{
	assert(mpDevice != nullptr);
	
	std::vector<VkAttachmentDescription> attachmentDescriptions;
	attachmentDescriptions.resize(attachments.size());
	std::vector<VkSubpassDescription> subpassDescriptions;
	subpassDescriptions.resize(subPasses.size());


	// Collect the needed data
	for (uint32_t i = 0; i < attachments.size(); ++i)
	{
		attachmentDescriptions[i] = attachments[i]->GetAttachmentDescription();
	}

	for (uint32_t i = 0; i < subPasses.size(); ++i)
	{
		subpassDescriptions[i] = subPasses[i]->GetSubpassDescription();
	}

	VkRenderPassCreateInfo renderPassCreateInfo =
		VulkanInitializers::RenderPassCreateInfo(static_cast<uint32_t>(attachmentDescriptions.size()), attachmentDescriptions.data(),
												static_cast<uint32_t>(subpassDescriptions.size()), subpassDescriptions.data(),
												static_cast<uint32_t>(subPassDependencies.size()), subPassDependencies.data());

	VK_CHECK_RESULT(vkCreateRenderPass(mpDevice->GetDeviceHandle(), &renderPassCreateInfo, nullptr, &mRenderPassHandle));
}

void VulkanRenderPass::Destroy()
{
	if (mRenderPassHandle)
	{
		vkDestroyRenderPass(mpDevice->GetDeviceHandle(), mRenderPassHandle, nullptr);
		mRenderPassHandle = VK_NULL_HANDLE;
	}

	if (mpDevice)
	{
		mpDevice = nullptr;
	}
}

void VulkanRenderPass::Begin(VkCommandBuffer commandBufferHandle, VkFramebuffer frameBufferHandle, const VkRect2D& renderArea, 	const std::vector<VkClearValue>& clearValues,
							 VkSubpassContents contents)
{
	VkRenderPassBeginInfo renderPassBeginInfo =
		VulkanInitializers::RenderPassBeginInfo(mRenderPassHandle, frameBufferHandle, renderArea, static_cast<uint32_t>(clearValues.size()), clearValues.data());

	vkCmdBeginRenderPass(commandBufferHandle, &renderPassBeginInfo, contents);
}

void VulkanRenderPass::End(VkCommandBuffer commandBufferHandle)
{
	vkCmdEndRenderPass(commandBufferHandle);
}

const VkRenderPass& VulkanRenderPass::GetHandle() const
{
	return mRenderPassHandle;
}