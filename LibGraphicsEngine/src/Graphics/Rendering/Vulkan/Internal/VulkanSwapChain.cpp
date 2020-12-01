#include "VulkanSwapChain.hpp"
#include "VulkanDevice.hpp"
#include "VulkanFrameBufferAttachment.hpp"
#include "VulkanInitializers.hpp"
#include "VulkanHelpers.hpp"
#include "Foundation/Platform/Platform.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanSwapChain::VulkanSwapChain()
	: mpDevice(nullptr)
	, mHandle(VK_NULL_HANDLE)
	, mSwapChainImageCount(0)
{}

VulkanSwapChain::VulkanSwapChain(VulkanDevice* pDevice)
	: mpDevice(pDevice)
	, mHandle(VK_NULL_HANDLE)
	, mSwapChainImageCount(0)
{
	Create();
}

VulkanSwapChain::~VulkanSwapChain()
{
	Destroy();
}

void VulkanSwapChain::Create()
{
	assert(mpDevice != nullptr);

	VkSwapchainKHR oldSwapchainHandle = mHandle;

	auto& surfacePresentModes = mpDevice->GetSurfacePresentModes();

	// The VK_PRESENT_MODE_FIFO_KHR mode must always be present as per spec
	// This mode waits for the vertical blank ("v-sync")
	VkPresentModeKHR swapchainPresentMode = VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;

	// If v-sync is not requested, try to find a mailbox mode
	// It's the lowest latency non-tearing present mode available
//	if (!mSettings.vsync) //TODO - put vsync into config
	{
		for (uint32_t i = 0; i < surfacePresentModes.size(); i++)
		{
			if (surfacePresentModes[i] == VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR)
			{
				swapchainPresentMode = VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR;
				break;
			}
			if ((swapchainPresentMode != VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR) && (surfacePresentModes[i] == VkPresentModeKHR::VK_PRESENT_MODE_IMMEDIATE_KHR))
			{
				swapchainPresentMode = VkPresentModeKHR::VK_PRESENT_MODE_IMMEDIATE_KHR;
			}
		}
	}

	auto& surfaceCapabilities = mpDevice->GetSurfaceCapabilities();

	VkExtent2D swapchainExtent = {};
	// If width (and height) equals the special value 0xFFFFFFFF, the size of the surface will be set by the swapchain
	if (surfaceCapabilities.currentExtent.width == (uint32_t)-1)
	{
		// If the surface size is undefined, the size is set to
		// the size of the images requested.
		swapchainExtent.width = mpDevice->GetWindow()->width;
		swapchainExtent.height = mpDevice->GetWindow()->height;
	}
	else
	{
		//TODO
		// If the surface size is defined, the swap chain size must match
		swapchainExtent = surfaceCapabilities.currentExtent;
		//	mWindowPtr->width = surfCaps.currentExtent.width;
		//	mWindowPtr->height = surfCaps.currentExtent.height;
	}

	// Determine the number of images
	uint32_t desiredNumberOfSwapchainImages = surfaceCapabilities.minImageCount + 1;
	if ((surfaceCapabilities.maxImageCount > 0) && (desiredNumberOfSwapchainImages > surfaceCapabilities.maxImageCount))
	{
		desiredNumberOfSwapchainImages = surfaceCapabilities.maxImageCount;
	}

	// Find the transformation of the surface
	VkSurfaceTransformFlagsKHR preTransform;
	if (surfaceCapabilities.supportedTransforms & VkSurfaceTransformFlagBitsKHR::VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
	{
		// We prefer a non-rotated transform
		preTransform = VkSurfaceTransformFlagBitsKHR::VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	else
	{
		preTransform = surfaceCapabilities.currentTransform;
	}

	// Find a supported composite alpha format (not all devices support alpha opaque)
	VkCompositeAlphaFlagBitsKHR compositeAlpha = VkCompositeAlphaFlagBitsKHR::VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	// Simply select the first composite alpha format available
	std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags =
	{
		VkCompositeAlphaFlagBitsKHR::VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		VkCompositeAlphaFlagBitsKHR::VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
		VkCompositeAlphaFlagBitsKHR::VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
		VkCompositeAlphaFlagBitsKHR::VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
	};
	for (auto& compositeAlphaFlag : compositeAlphaFlags)
	{
		if (surfaceCapabilities.supportedCompositeAlpha & compositeAlphaFlag)
		{
			compositeAlpha = compositeAlphaFlag;
			break;
		};
	}

	VkSwapchainCreateInfoKHR swapchainCreateInfo =
		VulkanInitializers::SwapchainCreateInfoKHR(mpDevice->GetSurfaceHandle(), desiredNumberOfSwapchainImages, mpDevice->GetSurfaceFormat().format, mpDevice->GetSurfaceFormat().colorSpace,
			swapchainExtent, 1, VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VkSharingMode::VK_SHARING_MODE_EXCLUSIVE,
			0, nullptr, (VkSurfaceTransformFlagBitsKHR)preTransform, compositeAlpha, swapchainPresentMode,
			VK_TRUE, oldSwapchainHandle);

	// Enable transfer source on swap chain images if supported
	if (surfaceCapabilities.supportedUsageFlags & VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
	{
		swapchainCreateInfo.imageUsage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	}

	// if we have support for different graphics and present queues
	if (mpDevice->IsPresentQueueSupported())
	{
		uint32_t queueFamilyIndices[2] = { mpDevice->GetQueueFamilyIndices().graphics, mpDevice->GetQueueFamilyIndices().present };

		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapchainCreateInfo.queueFamilyIndexCount = 2;
		swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else //using same queue for both: graphics and present
	{
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	VK_CHECK_RESULT(vkCreateSwapchainKHR(mpDevice->GetDeviceHandle(), &swapchainCreateInfo, nullptr, &mHandle));

	// If an existing swap chain is re-created, destroy the old swap chain
	// This also cleans up all the presentable images
	if (oldSwapchainHandle != VK_NULL_HANDLE)
	{
		for (auto& buffer : mSwapChainBuffers)
		{
			GE_FREE(buffer);
		}
		vkDestroySwapchainKHR(mpDevice->GetDeviceHandle(), oldSwapchainHandle, nullptr);
	}

	// Get swap chain image count
	VK_CHECK_RESULT(vkGetSwapchainImagesKHR(mpDevice->GetDeviceHandle(), mHandle, &mSwapChainImageCount, nullptr));

	// Get the swap chain images
	std::vector<VkImage> swapChainImages;
	swapChainImages.resize(mSwapChainImageCount);
	VK_CHECK_RESULT(vkGetSwapchainImagesKHR(mpDevice->GetDeviceHandle(), mHandle, &mSwapChainImageCount, swapChainImages.data()));

	mSwapChainBuffers.resize(mSwapChainImageCount);

	for (uint32_t i = 0; i < mSwapChainBuffers.size(); ++i)
	{
		mSwapChainBuffers[i] = GE_ALLOC(VulkanFrameBufferAttachment)(mpDevice, swapChainImages[i], mpDevice->GetSurfaceFormat().format, VulkanFrameBufferAttachment::Type::COLOR);
	}
}

void VulkanSwapChain::Destroy()
{
	for (auto& buffer : mSwapChainBuffers)
	{
		GE_FREE(buffer);
	}

	if (mSwapChainImageCount > 0)
	{
		mSwapChainImageCount = 0;
	}

	if (mHandle)
	{
		vkDestroySwapchainKHR(mpDevice->GetDeviceHandle(), mHandle, nullptr);
		mHandle = VK_NULL_HANDLE;
	}

	if (mpDevice)
	{
		mpDevice = nullptr;
	}
}

VkResult VulkanSwapChain::AcquireNextImage(uint32_t* pImageIndex, VkSemaphore presentCompleteSemaphoreHandle) const
{
	return vkAcquireNextImageKHR(mpDevice->GetDeviceHandle(), mHandle, UINT64_MAX, presentCompleteSemaphoreHandle, VK_NULL_HANDLE, pImageIndex);
}

void VulkanSwapChain::Reset()
{
	// on reset the window size is changed, so we must update surface capabilities which include surface extent
	mpDevice->UpdateSurfaceCapabilities();

	Create();
}

const VkSwapchainKHR& VulkanSwapChain::GetHandle() const
{
	return mHandle;
}

uint32_t VulkanSwapChain::GetSwapChainImageCount() const
{
	return mSwapChainImageCount;
}

const std::vector<VulkanFrameBufferAttachment*>& VulkanSwapChain::GetSwapChainBuffers() const
{
	return mSwapChainBuffers;
}

