#include "VulkanFrameBuffer.hpp"
#include "VulkanDevice.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanInitializers.hpp"
#include "VulkanHelpers.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanFrameBuffer::VulkanFrameBuffer()
	: mpDevice(nullptr)
	, mpRenderPass(nullptr)
	, mHandle(VK_NULL_HANDLE)
{}

VulkanFrameBuffer::VulkanFrameBuffer(VulkanDevice* pDevice, VulkanRenderPass* pRenderPass, const std::vector<VkImageView>& attachments,
									uint32_t width, uint32_t height)
	: mpDevice(pDevice)
	, mpRenderPass(pRenderPass)
	, mHandle(VK_NULL_HANDLE)
{
	Create(attachments, width, height);
}

VulkanFrameBuffer::~VulkanFrameBuffer()
{
	Destroy();
}

void VulkanFrameBuffer::Create(const std::vector<VkImageView>& attachments, uint32_t width, uint32_t height)
{
	assert(mpDevice != nullptr);
	assert(mpRenderPass != nullptr);
	assert(attachments.empty() == false);

	VkFramebufferCreateInfo framebufferCreateInfo = 
		VulkanInitializers::FramebufferCreateInfo(mpRenderPass->GetHandle(), static_cast<uint32_t>(attachments.size()), attachments.data(), width, height);

	VK_CHECK_RESULT(vkCreateFramebuffer(mpDevice->GetDeviceHandle(), &framebufferCreateInfo, nullptr, &mHandle));
}

void VulkanFrameBuffer::Destroy()
{
	assert(mpDevice != nullptr);

	if (mHandle)
	{
		vkDestroyFramebuffer(mpDevice->GetDeviceHandle(), mHandle, nullptr);
		mHandle = VK_NULL_HANDLE;
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
	return mHandle;
}