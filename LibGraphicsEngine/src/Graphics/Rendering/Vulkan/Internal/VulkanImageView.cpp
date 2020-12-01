#include "VulkanImageView.hpp"
#include "VulkanDevice.hpp"
#include "VulkanInitializers.hpp"
#include "VulkanHelpers.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanImageView::VulkanImageView()
	: mpDevice(nullptr)
	, mHandle(VK_NULL_HANDLE)
{}

VulkanImageView::VulkanImageView(VulkanDevice* pDevice, VkImage imageHandle, VkImageViewType viewType, VkFormat format, const VkComponentMapping& components,
								 const VkImageSubresourceRange& subresourceRange, VkImageViewCreateFlags flags)
	: mpDevice(pDevice)
	, mHandle(VK_NULL_HANDLE)
{
	VkImageViewCreateInfo imageViewCreateInfo = VulkanInitializers::ImageViewCreateInfo(imageHandle, viewType, format, components, subresourceRange, flags);

	Create(imageViewCreateInfo);
}

VulkanImageView::~VulkanImageView()
{
	Destroy();
}

void VulkanImageView::Create(const VkImageViewCreateInfo& imageViewCreateInfo)
{
	assert(mpDevice != nullptr);

	VK_CHECK_RESULT(vkCreateImageView(mpDevice->GetDeviceHandle(), &imageViewCreateInfo, nullptr, &mHandle));
}

void VulkanImageView::Destroy()
{
	if (mHandle)
	{
		vkDestroyImageView(mpDevice->GetDeviceHandle(), mHandle, nullptr);
		mHandle = VK_NULL_HANDLE;
	}

	if (mpDevice)
	{
		mpDevice = nullptr;
	}
}


const VkImageView& VulkanImageView::GetHandle() const
{
	return mHandle;
}