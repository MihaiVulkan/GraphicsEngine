
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanDevice.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanInstance.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanPhysicalDevice.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanSurface.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanSwapChain.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanSwapChainBuffer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanQueue.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanInitializers.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanHelpers.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanDebug.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanAllocator.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanPassThroughAllocator.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanPoolAllocator.hpp"
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
	, mpAllocator(nullptr)
	, mpSwapChain(nullptr)
	, mSurfaceFormat{}
	, mDepthStencilFormat{}
	, mEnableValidationLayers(false)
{}

VulkanDevice::VulkanDevice(Platform::Window* pWindow, bool_t enableValidation)
	: mpInstance(nullptr)
	, mpPhysicalDevice(nullptr)
	, mpLogicalDevice(nullptr)
	, mpWindow(pWindow)
	, mpSurface(nullptr)
	, mpAllocator(nullptr)
	, mpSwapChain(nullptr)
	, mSurfaceFormat{}
	, mDepthStencilFormat{}
	, mEnableValidationLayers(enableValidation)
{
	Create();
}

VulkanDevice::~VulkanDevice()
{
	Destroy();
}

void VulkanDevice::Create()
{
	assert(mpWindow != nullptr);

	//// DEVICE ////
	mpInstance = GE_ALLOC(VulkanInstance)(mpWindow->GetWindowTitle(), mEnableValidationLayers);
	assert(mpInstance != nullptr);

	mpSurface = GE_ALLOC(VulkanSurface)(this);
	assert(mpSurface != nullptr);

	//NOTE! For now we support only one gpu
	// TODO - multigpu support
	// internally we select the suitable gpu
	mpPhysicalDevice = GE_ALLOC(VulkanPhysicalDevice)(this);
	assert(mpPhysicalDevice != nullptr);

	mpPhysicalDevice->SelectSurfaceFormat(mSurfaceFormat);
	mDepthStencilFormat = mpPhysicalDevice->SelectDepthStencilFormat();

	mpLogicalDevice = GE_ALLOC(VulkanLogicalDevice)(this);
	assert(mpLogicalDevice != nullptr);

	// ALLOCATOR
#ifdef VULKAN_POOL_ALLOCATOR
	mpAllocator = GE_ALLOC(VulkanPoolAllocator)(this);
#else // we just use the fallback
	mpAllocator = GE_ALLOC(VulkanPassThroughAllocator)(this);
#endif // VULKAN_POOL_ALLOCATOR
	assert(mpAllocator != nullptr);

	mpSwapChain = GE_ALLOC(VulkanSwapChain)(this);
	assert(mpSwapChain != nullptr);
}

void VulkanDevice::Destroy()
{
	// wait for the device to finish ongoing operations on all owned queues
	WaitIdle();

	mDepthStencilFormat = {};
	mSurfaceFormat = {};

	GE_FREE(mpSwapChain);

	GE_FREE(mpAllocator);

	GE_FREE(mpLogicalDevice);
	
	GE_FREE(mpPhysicalDevice);

	GE_FREE(mpSurface);

	GE_FREE(mpInstance);

	mpWindow = nullptr;
}

void VulkanDevice::SetupConnection()
{
	// platform dependent
	// nothing to do for _WIN32
}

void VulkanDevice::ResetSwapChain()
{
	assert(mpSwapChain != nullptr);

	mpSwapChain->Reset();
}

VkResult VulkanDevice::AcquireNextImage(uint32_t* pImageIndex, VkSemaphore presentCompleteSemaphoreHandle) const
{
	assert(mpSwapChain != nullptr);
	assert(pImageIndex != nullptr);

	return mpSwapChain->AcquireNextImage(pImageIndex, presentCompleteSemaphoreHandle);
}

VkResult VulkanDevice::WaitIdle() const
{
	assert(mpLogicalDevice != nullptr);

	return mpLogicalDevice->WaitIdle();
}

void VulkanDevice::UpdateSurfaceCapabilities()
{
	assert(mpLogicalDevice != nullptr);

	mpPhysicalDevice->UpdateSurfaceCapabilities();
}

const VkInstance& VulkanDevice::GetInstanceHandle() const
{
	assert(mpInstance != nullptr);

	return mpInstance->GetHandle();
}

const VkSurfaceKHR& VulkanDevice::GetSurfaceHandle() const
{
	assert(mpSurface != nullptr);

	return mpSurface->GetHandle();
}

const VkPhysicalDevice& VulkanDevice::GetPhysicalDeviceHandle() const
{
	assert(mpPhysicalDevice != nullptr);

	return mpPhysicalDevice->GetHandle();
}

const VkPhysicalDeviceProperties& VulkanDevice::GetPhysicalDeviceProperties() const
{
	assert(mpPhysicalDevice != nullptr);

	return mpPhysicalDevice->GetProperties();
}
const VkPhysicalDeviceFeatures& VulkanDevice::GetPhysicalDeviceFeatures() const
{
	assert(mpPhysicalDevice != nullptr);

	return mpPhysicalDevice->GetFeatures();
}

const VkPhysicalDeviceMemoryProperties& VulkanDevice::GetPhysicalDeviceMemoryProperties() const
{
	assert(mpPhysicalDevice != nullptr);

	return mpPhysicalDevice->GetMemoryProperties();
}

const std::vector<std::string>& VulkanDevice::GetPhysicalDeviceSupportedExtensions() const
{
	assert(mpPhysicalDevice != nullptr);

	return mpPhysicalDevice->GetSupportedExtensions();
}

void VulkanDevice::SetPhysicalDeviceSupportedExtensions(const std::vector<std::string>& extensions)
{
	assert(mpPhysicalDevice != nullptr);

	mpPhysicalDevice->SetSupportedExtensions(extensions);
}

const VkPhysicalDeviceFeatures& VulkanDevice::GetPhysicaltDeviceEnabledFeatures() const
{
	assert(mpPhysicalDevice != nullptr);

	return mpPhysicalDevice->GetEnabledFeatures();
}

void VulkanDevice::SetPhysicaltDeviceEnabledFeatures(const VkPhysicalDeviceFeatures& features)
{
	assert(mpPhysicalDevice != nullptr);

	mpPhysicalDevice->SetEnabledFeatures(features);
}

const VkSurfaceCapabilitiesKHR& VulkanDevice::GetSurfaceCapabilities() const
{
	assert(mpPhysicalDevice != nullptr);

	return mpPhysicalDevice->GetSurfaceCapabilities();
}

const std::vector<VkSurfaceFormatKHR>& VulkanDevice::GetSurfaceFormats() const
{
	assert(mpPhysicalDevice != nullptr);

	return mpPhysicalDevice->GetSurfaceFormats();
}

const std::vector<VkPresentModeKHR>& VulkanDevice::GetSurfacePresentModes() const
{
	assert(mpPhysicalDevice != nullptr);

	return mpPhysicalDevice->GetSurfacePresentModes();
}

const std::vector<VkQueueFamilyProperties>& VulkanDevice::GetQueueFamilyPropertiesVector() const
{
	assert(mpPhysicalDevice != nullptr);

	return mpPhysicalDevice->GetQueueFamilyPropertiesVector();
}

bool VulkanDevice::GetMemoryTypeFromProperties(uint32_t typeBits, VkMemoryPropertyFlags requirementsMask, uint32_t& typeIndex)
{
	assert(mpPhysicalDevice != nullptr);

	return mpPhysicalDevice->GetMemoryType(typeBits, requirementsMask, typeIndex);
}

const VkSurfaceFormatKHR& VulkanDevice::GetSurfaceFormat() const
{
	return mSurfaceFormat;
}

const VkFormat& VulkanDevice::GetDepthStencilFormat() const
{
	return mDepthStencilFormat;
}


const VkDevice& VulkanDevice::GetDeviceHandle() const
{
	assert(mpLogicalDevice != nullptr);

	return mpLogicalDevice->GetHandle();
}

const VulkanLogicalDevice::QueueFamilyIndices& VulkanDevice::GetQueueFamilyIndices() const
{
	assert(mpLogicalDevice != nullptr);

	return mpLogicalDevice->GetQueueFamilyIndices();
}

const VkSwapchainKHR& VulkanDevice::GetSwapChainHandle() const
{
	assert(mpSwapChain != nullptr);

	return mpSwapChain->GetHandle();
}

const std::vector<VulkanSwapChainBuffer*>& VulkanDevice::GetSwapChainColorBuffers() const
{
	assert(mpSwapChain != nullptr);

	return mpSwapChain->GetSwapChainColorBuffers();
}

uint32_t VulkanDevice::GetSwapChainColorBufferCount() const
{
	assert(mpSwapChain != nullptr);

	return mpSwapChain->GetSwapChainColorBufferCount();
}

VulkanSwapChainBuffer* VulkanDevice::GetSwapChainDepthStencilBuffer() const
{
	assert(mpSwapChain != nullptr);

	return mpSwapChain->GetSwapChainDepthStencilBuffer();
}

Platform::Window* VulkanDevice::GetWindow() const
{
	return mpWindow;
}

bool VulkanDevice::IsGraphicsQueueSupported() const
{
	assert(mpLogicalDevice != nullptr);

	return mpLogicalDevice->IsGraphicsQueueSupported();
}

bool VulkanDevice::IsComputeQueueSupported() const
{
	assert(mpLogicalDevice != nullptr);

	return mpLogicalDevice->IsComputeQueueSupported();
}

bool VulkanDevice::IsPresentQueueSupported() const
{
	assert(mpLogicalDevice != nullptr);

	return mpLogicalDevice->IsPresentQueueSupported();
}

VulkanQueue* VulkanDevice::GetGraphicsQueue() const
{
	assert(mpLogicalDevice != nullptr);

	return mpLogicalDevice->GetQueue(VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT);
}

VulkanQueue* VulkanDevice::GetPresentQueue() const
{
	assert(mpLogicalDevice != nullptr);

	return mpLogicalDevice->GetQueue(VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT);
}

VulkanQueue* VulkanDevice::GetComputeQueue() const
{
	assert(mpLogicalDevice != nullptr);

	return mpLogicalDevice->GetQueue(VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT);
}

VulkanAllocator* VulkanDevice::GetAllocator() const
{
	return mpAllocator;
}