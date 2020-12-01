
#include "VulkanDevice.hpp"
#include "VulkanInstance.hpp"
#include "VulkanPhysicalDevice.hpp"
#include "VulkanLogicalDevice.hpp"
#include "VulkanSurface.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanQueue.hpp"
#include "VulkanFrameBufferAttachment.hpp"
#include "VulkanCommandPool.hpp"
#include "VulkanInitializers.hpp"
#include "VulkanHelpers.hpp"
#include "VulkanDebug.hpp"
#include "VulkanAllocator.hpp"
#include "VulkanPassThroughAllocator.hpp"
#include "VulkanPoolAllocator.hpp"
#include "Foundation/Platform/Platform.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"

#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanDevice::VulkanDevice()
	: mpInstance(nullptr)
	, mpPhysicalDevice(nullptr)
	, mpLogicalDevice(nullptr)
	, mpWindow(nullptr)
	, mpSurface(nullptr)
	, mpSwapChain(nullptr)
	, mSurfaceFormat{}
	, mDepthFormat{}
	, mpMainGraphicsQueue(nullptr)
	, mpPresentQueue(nullptr)
	, mpCommandPool(nullptr)
	, mpAllocator(nullptr)
	, mEnableValidationLayers(false)
{}

VulkanDevice::VulkanDevice(Platform::GE_Window* pWindow, bool_t enableValidation)
	: mpInstance(nullptr)
	, mpPhysicalDevice(nullptr)
	, mpLogicalDevice(nullptr)
	, mpWindow(pWindow)
	, mpSurface(nullptr)
	, mpSwapChain(nullptr)
	, mSurfaceFormat{}
	, mDepthFormat{}
	, mpMainGraphicsQueue(nullptr)
	, mpPresentQueue(nullptr)
	, mpCommandPool(nullptr)
	, mpAllocator(nullptr)
	, mEnableValidationLayers(enableValidation)
{
	Create(pWindow, enableValidation);
}

VulkanDevice::~VulkanDevice()
{
	Destroy();
}

void VulkanDevice::Create(Platform::GE_Window* pWindow, bool_t enableValidation)
{
	assert(mpWindow != nullptr);

	//// DEVICE ////
	mpInstance = GE_ALLOC(VulkanInstance)(mpWindow->pTitle, mEnableValidationLayers);

	mpSurface = GE_ALLOC(VulkanSurface)(this);

	// internally we select the suitable gpu
	mpPhysicalDevice = GE_ALLOC(VulkanPhysicalDevice)(this);

	mpPhysicalDevice->SelectSurfaceFormat(mSurfaceFormat);
	mDepthFormat = mpPhysicalDevice->SelectDepthFormat();

	mpLogicalDevice = GE_ALLOC(VulkanLogicalDevice)(this);

	// Get a graphics queue from the device
	mpMainGraphicsQueue = mpLogicalDevice->GetQueue(VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT);

	if (IsPresentQueueSupported())
	{
		mpPresentQueue = mpLogicalDevice->GetQueue(VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT);
	}

	//// SWAPCHAIN ////
	SetupConnection();

	mpSwapChain = GE_ALLOC(VulkanSwapChain)(this);

	// ALLOCATOR
#ifdef VULKAN_ALLOCATOR
	mpAllocator = GE_ALLOC(VulkanPoolAllocator)(this);
#else // we just use the fall
	mpAllocator = GE_ALLOC(VulkanPassThroughAllocator)(this);
#endif


	//// COMMANDS
	// we create a command pool for graphics commands
	mpCommandPool = GE_ALLOC(VulkanCommandPool)(this, mpLogicalDevice->GetQueueFamilyIndices().graphics);
}

void VulkanDevice::Destroy()
{
	// wait for the device to finish ongoing operations on all owned queues
	WaitIdle();

	GE_FREE(mpCommandPool);

	GE_FREE(mpAllocator);

	if (IsPresentQueueSupported())
	{
		GE_FREE(mpPresentQueue);
	}

	mpMainGraphicsQueue = nullptr;

	mDepthFormat = {};
	mSurfaceFormat = {};

	GE_FREE(mpSwapChain);

	GE_FREE(mpLogicalDevice);
	
	GE_FREE(mpPhysicalDevice);

	GE_FREE(mpSurface);

	GE_FREE(mpInstance);

	mpWindow = nullptr;
}

bool VulkanDevice::GetMemoryTypeFromProperties(uint32_t typeBits, VkMemoryPropertyFlags requirementsMask, uint32_t& typeIndex)
{
	assert(mpPhysicalDevice != nullptr);

	return mpPhysicalDevice->GetMemoryType(typeBits, requirementsMask, typeIndex);
}

void VulkanDevice::SetupConnection()
{
	// platform dependent
	// nothing to do for _WIN32
}

void VulkanDevice::ResetSwapChain()
{
	mpSwapChain->Reset();
}

VkResult VulkanDevice::AcquireNextImage(uint32_t* pImageIndex, VkSemaphore presentCompleteSemaphoreHandle) const
{
	return mpSwapChain->AcquireNextImage(pImageIndex, presentCompleteSemaphoreHandle);
}

VkResult VulkanDevice::WaitIdle() const
{
	return mpLogicalDevice->WaitIdle();
}

void VulkanDevice::UpdateSurfaceCapabilities()
{
	mpPhysicalDevice->UpdateSurfaceCapabilities();
}

const VkInstance& VulkanDevice::GetInstanceHandle() const
{
	return mpInstance->GetHandle();
}

const VkPhysicalDevice& VulkanDevice::GetPhysicalDeviceHandle() const
{
	return mpPhysicalDevice->GetHandle();
}

const VkPhysicalDeviceProperties& VulkanDevice::GetPhysicalDeviceProperties() const
{
	return mpPhysicalDevice->GetProperties();
}
const VkPhysicalDeviceFeatures& VulkanDevice::GetPhysicalDeviceFeatures() const
{
	return mpPhysicalDevice->GetFeatures();
}

const VkPhysicalDeviceMemoryProperties& VulkanDevice::GetPhysicalDeviceMemoryProperties() const
{
	return mpPhysicalDevice->GetMemoryProperties();
}

const std::vector<std::string>& VulkanDevice::GetPhysicalDeviceSupportedExtensions() const
{
	return mpPhysicalDevice->GetSupportedExtensions();
}

void VulkanDevice::SetPhysicalDeviceSupportedExtensions(const std::vector<std::string>& extensions)
{
	mpPhysicalDevice->SetSupportedExtensions(extensions);
}

const VkPhysicalDeviceFeatures& VulkanDevice::GetPhysicaltDeviceEnabledFeatures() const
{
	return mpPhysicalDevice->GetEnabledFeatures();
}

void VulkanDevice::SetPhysicaltDeviceEnabledFeatures(const VkPhysicalDeviceFeatures& features)
{
	mpPhysicalDevice->SetEnabledFeatures(features);
}

const VkSurfaceCapabilitiesKHR& VulkanDevice::GetSurfaceCapabilities() const
{
	return mpPhysicalDevice->GetSurfaceCapabilities();
}

const std::vector<VkSurfaceFormatKHR>& VulkanDevice::GetSurfaceFormats() const
{
	return mpPhysicalDevice->GetSurfaceFormats();
}

const std::vector<VkPresentModeKHR>& VulkanDevice::GetSurfacePresentModes() const
{
	return mpPhysicalDevice->GetSurfacePresentModes();
}

const std::vector<VkQueueFamilyProperties>& VulkanDevice::GetQueueFamilyPropertiesVector() const
{
	return mpPhysicalDevice->GetQueueFamilyPropertiesVector();
}

const VulkanLogicalDevice::QueueFamilyIndices& VulkanDevice::GetQueueFamilyIndices() const
{
	return mpLogicalDevice->GetQueueFamilyIndices();
}

const VkDevice& VulkanDevice::GetDeviceHandle() const
{
	return mpLogicalDevice->GetHandle();
}

const VkSurfaceKHR& VulkanDevice::GetSurfaceHandle() const
{
	return mpSurface->GetHandle();
}

const VkSurfaceFormatKHR& VulkanDevice::GetSurfaceFormat() const
{
	return mSurfaceFormat;
}

const VkFormat& VulkanDevice::GetDepthFormat() const
{
	return mDepthFormat;
}

const VkSwapchainKHR& VulkanDevice::GetSwapChainHandle() const
{
	return mpSwapChain->GetHandle();
}

uint32_t VulkanDevice::GetSwapChainImageCount() const
{
	return mpSwapChain->GetSwapChainImageCount();
}

const std::vector<VulkanFrameBufferAttachment*>& VulkanDevice::GetSwapChainBuffers() const
{
	return mpSwapChain->GetSwapChainBuffers();
}

bool VulkanDevice::IsGraphicsQueueSupported() const
{
	return mpLogicalDevice->IsGraphicsQueueSupported();
}

bool VulkanDevice::IsComputeQueueSupported() const
{
	return mpLogicalDevice->IsComputeQueueSupported();
}

bool VulkanDevice::IsPresentQueueSupported() const
{
	return mpLogicalDevice->IsPresentQueueSupported();
}

const Platform::GE_Window* VulkanDevice::GetWindow() const
{
	return mpWindow;
}

VulkanQueue* VulkanDevice::GetMainGraphicsQueue() const
{
	return mpMainGraphicsQueue;
}

VulkanQueue* VulkanDevice::GetPresentQueue() const
{
	return mpPresentQueue;
}

const VkCommandPool& VulkanDevice::GetCommandPoolHandle() const
{
	return mpCommandPool->GetHandle();
}

VulkanAllocator* VulkanDevice::GetAllocator() const
{
	return mpAllocator;
}