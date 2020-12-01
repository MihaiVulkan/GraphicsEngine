#include "VulkanFrameBuffer.hpp"
#include "VulkanDevice.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanFrameBufferAttachment.hpp"
#include "VulkanInitializers.hpp"
#include "VulkanHelpers.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanFrameBuffer::VulkanFrameBuffer()
	: mpDevice(nullptr)
	, mpRenderPass(nullptr)
	, mFrameBufferHandle(VK_NULL_HANDLE)
{}

VulkanFrameBuffer::VulkanFrameBuffer(VulkanDevice* pDevice, VulkanRenderPass* pRenderPass, const std::vector<VulkanFrameBufferAttachment*>& attachments, uint32_t width, uint32_t height)
	: mpDevice(pDevice)
	, mpRenderPass(pRenderPass)
	, mFrameBufferHandle(VK_NULL_HANDLE)
{
	Create(attachments, width, height);
}

VulkanFrameBuffer::~VulkanFrameBuffer()
{
	Destroy();
}

void VulkanFrameBuffer::Create(const std::vector<VulkanFrameBufferAttachment*>& attachments, uint32_t width, uint32_t height)
{
	assert(mpDevice != nullptr);
	assert(mpRenderPass != nullptr);

	// Get the views only as this is what the framebuffer needs
	std::vector<VkImageView> views;
	views.resize(attachments.size());

	for (size_t i = 0; i < views.size(); ++ i)
	{
		views[i] = attachments[i]->GetImageViewHandle();
	}

	VkFramebufferCreateInfo framebufferCreateInfo = 
		VulkanInitializers::FramebufferCreateInfo(mpRenderPass->GetHandle(), static_cast<uint32_t>(views.size()), views.data(), width, height);

	VK_CHECK_RESULT(vkCreateFramebuffer(mpDevice->GetDeviceHandle(), &framebufferCreateInfo, nullptr, &mFrameBufferHandle));
}

void VulkanFrameBuffer::Destroy()
{
	if (mFrameBufferHandle)
	{
		vkDestroyFramebuffer(mpDevice->GetDeviceHandle(), mFrameBufferHandle, nullptr);
		mFrameBufferHandle = VK_NULL_HANDLE;
	}

	if (mpRenderPass)
	{
		mpRenderPass = nullptr;
	}

	if (mpDevice)
	{
		mpDevice = nullptr;
	}

}

const VkFramebuffer& VulkanFrameBuffer::GetHandle() const
{
	return mFrameBufferHandle;
}