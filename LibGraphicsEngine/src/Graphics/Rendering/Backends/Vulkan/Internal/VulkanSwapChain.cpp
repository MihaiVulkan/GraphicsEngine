#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanSwapChain.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanDevice.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanSwapChainBuffer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanInitializers.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanHelpers.hpp"
#include "Foundation/Platform/Platform.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanSwapChain::VulkanSwapChain()
	: mpDevice(nullptr)
	, mHandle(VK_NULL_HANDLE)
	, mOldHandle(VK_NULL_HANDLE)
	, mpDepthStencilBuffer(nullptr)
{}

VulkanSwapChain::VulkanSwapChain(VulkanDevice* pDevice)
	: mpDevice(pDevice)
	, mHandle(VK_NULL_HANDLE)
	, mOldHandle(VK_NULL_HANDLE)
	, mpDepthStencilBuffer(nullptr)

{
	Create();
}

VulkanSwapChain::~VulkanSwapChain()
{
	Destroy();
}

void VulkanSwapChain::Create()
{
	CreateSwapChain();

	CreateSwapChainBuffers();
}

void VulkanSwapChain::CreateSwapChain()
{
	assert(mpDevice != nullptr);

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
		auto pWindow = mpDevice->GetWindow();
		assert(pWindow != nullptr);

		// If the surface size is undefined, the size is set to
		// the size of the images requested.
		swapchainExtent.width = pWindow->GetState().width;
		swapchainExtent.height = pWindow->GetState().height;
	}
	else
	{
		//TODO
		// If the surface size is defined, the swap chain size must match
		swapchainExtent = surfaceCapabilities.currentExtent;
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

	// cache the old handle
	mOldHandle = mHandle;

	VkSwapchainCreateInfoKHR swapchainCreateInfo =
		VulkanInitializers::SwapchainCreateInfoKHR(mpDevice->GetSurfaceHandle(), desiredNumberOfSwapchainImages, mpDevice->GetSurfaceFormat().format, mpDevice->GetSurfaceFormat().colorSpace,
			swapchainExtent, 1, VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VkSharingMode::VK_SHARING_MODE_EXCLUSIVE,
			0, nullptr, (VkSurfaceTransformFlagBitsKHR)preTransform, compositeAlpha, swapchainPresentMode,
			VK_TRUE, mOldHandle);

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

	// create a new handle
	VK_CHECK_RESULT(vkCreateSwapchainKHR(mpDevice->GetDeviceHandle(), &swapchainCreateInfo, nullptr, &mHandle));
}

void VulkanSwapChain::CreateSwapChainBuffers()
{
	assert(mpDevice != nullptr);

	////// COLOR IMAGE

	// Get swap chain color image count
	uint32_t imageCount = 0;
	VK_CHECK_RESULT(vkGetSwapchainImagesKHR(mpDevice->GetDeviceHandle(), mHandle, &imageCount, nullptr));

	// Get the swap chain color/presentable images
	std::vector<VkImage> swapChainImages(imageCount);
	VK_CHECK_RESULT(vkGetSwapchainImagesKHR(mpDevice->GetDeviceHandle(), mHandle, &imageCount, swapChainImages.data()));

	mSwapChainColorBuffers.resize(imageCount);

	for (uint32_t i = 0; i < mSwapChainColorBuffers.size(); ++i)
	{
		mSwapChainColorBuffers[i] = GE_ALLOC(VulkanSwapChainBuffer)
		(
			mpDevice,
			swapChainImages[i], mpDevice->GetSurfaceFormat().format,
			VulkanSwapChainBuffer::BufferType::GE_BT_COLOR
		);
		assert(mSwapChainColorBuffers[i] != nullptr);
	}

	//////// DEPTH IMAGE
	// TODO - for depth stencil we use only 1 image

	auto& surfaceCapabilities = mpDevice->GetSurfaceCapabilities();

	mpDepthStencilBuffer = GE_ALLOC(VulkanSwapChainBuffer)
	(
		mpDevice,
		mpDevice->GetDepthStencilFormat(),
		VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
		surfaceCapabilities.currentExtent.width, surfaceCapabilities.currentExtent.height,
		VulkanSwapChainBuffer::BufferType::GE_BT_DEPTH_STENCIL
	);
	assert(mpDepthStencilBuffer != nullptr);
}

void VulkanSwapChain::Destroy()
{
	assert(mpDevice != nullptr);

	DestroySwapChainBuffers();
	DestroyOldSwapChain();

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

void VulkanSwapChain::DestroyOldSwapChain()
{
	assert(mpDevice != nullptr);

	// Destroy the old swap chain
	// This also cleans up all the presentable images
	if (mOldHandle)
	{
		vkDestroySwapchainKHR(mpDevice->GetDeviceHandle(), mOldHandle, nullptr);
		mOldHandle = VK_NULL_HANDLE;
	}
}

void VulkanSwapChain::DestroySwapChainBuffers()
{
	if (mSwapChainColorBuffers.empty() == false)
	{
		for (auto& image : mSwapChainColorBuffers)
		{
			GE_FREE(image);
		}
	}

	GE_FREE(mpDepthStencilBuffer);
}

VkResult VulkanSwapChain::AcquireNextImage(uint32_t* pImageIndex, VkSemaphore presentCompleteSemaphoreHandle) const
{
	assert(mpDevice != nullptr);

	return vkAcquireNextImageKHR(mpDevice->GetDeviceHandle(), mHandle, UINT64_MAX, presentCompleteSemaphoreHandle, VK_NULL_HANDLE, pImageIndex);
}

void VulkanSwapChain::Reset()
{
	assert(mpDevice != nullptr);

	// on reset the window size is changed, so we must update surface capabilities which include surface extent
	mpDevice->UpdateSurfaceCapabilities();

	DestroySwapChainBuffers();
	DestroyOldSwapChain();

	Create();
}

const VkSwapchainKHR& VulkanSwapChain::GetHandle() const
{
	return mHandle;
}

const std::vector<VulkanSwapChainBuffer*>& VulkanSwapChain::GetSwapChainColorBuffers() const
{
	return mSwapChainColorBuffers;
}

uint32_t VulkanSwapChain::GetSwapChainColorBufferCount() const
{
	return static_cast<uint32_t>(mSwapChainColorBuffers.size());
}

VulkanSwapChainBuffer* VulkanSwapChain::GetSwapChainDepthStencilBuffer() const
{
	return mpDepthStencilBuffer;
}