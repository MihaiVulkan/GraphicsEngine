#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanRenderPass.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanDevice.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanInitializers.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanHelpers.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanRenderPass::VulkanRenderPass()
	: mpDevice(nullptr)
	, mHandle(VK_NULL_HANDLE)
{}

VulkanRenderPass::VulkanRenderPass(VulkanDevice* pDevice, const std::vector<VkAttachmentDescription>& attachmentDescriptions,
									const std::vector<VkSubpassDescription>& subPassDescriptions,
									const std::vector<VkSubpassDependency>& subPassDependencies)
	: mpDevice(pDevice)
	, mHandle(VK_NULL_HANDLE)
{
	Create(attachmentDescriptions, subPassDescriptions, subPassDependencies);
}

VulkanRenderPass::~VulkanRenderPass()
{
	Destroy();
}

void VulkanRenderPass::Create(const std::vector<VkAttachmentDescription>& attachmentDescriptions,
							const std::vector<VkSubpassDescription>& subPassDescriptions,
						const std::vector<VkSubpassDependency>& subPassDependencies)
{
	assert(mpDevice != nullptr);
	
	VkRenderPassCreateInfo renderPassCreateInfo =
		VulkanInitializers::RenderPassCreateInfo(static_cast<uint32_t>(attachmentDescriptions.size()), attachmentDescriptions.data(),
												static_cast<uint32_t>(subPassDescriptions.size()), subPassDescriptions.data(),
												static_cast<uint32_t>(subPassDependencies.size()), subPassDependencies.data());

	VK_CHECK_RESULT(vkCreateRenderPass(mpDevice->GetDeviceHandle(), &renderPassCreateInfo, nullptr, &mHandle));
}

void VulkanRenderPass::Destroy()
{
	assert(mpDevice != nullptr);

	if (mHandle)
	{
		vkDestroyRenderPass(mpDevice->GetDeviceHandle(), mHandle, nullptr);
		mHandle = VK_NULL_HANDLE;
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
		VulkanInitializers::RenderPassBeginInfo(mHandle, frameBufferHandle, renderArea, static_cast<uint32_t>(clearValues.size()), clearValues.data());

	vkCmdBeginRenderPass(commandBufferHandle, &renderPassBeginInfo, contents);
}

void VulkanRenderPass::End(VkCommandBuffer commandBufferHandle)
{
	vkCmdEndRenderPass(commandBufferHandle);
}

const VkRenderPass& VulkanRenderPass::GetHandle() const
{
	return mHandle;
}