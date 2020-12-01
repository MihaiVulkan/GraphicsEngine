#include "VulkanLogicalDevice.hpp"
#include "VulkanDevice.hpp"
#include "VulkanQueue.hpp" 
#include "VulkanInitializers.hpp"
#include "VulkanHelpers.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanLogicalDevice::VulkanLogicalDevice()
	: mpDevice(nullptr)
	, mHandle(VK_NULL_HANDLE)
	, mQueueFamilyIndices{}
{}

VulkanLogicalDevice::VulkanLogicalDevice(VulkanDevice* pDevice)
	: mpDevice(pDevice)
	, mHandle(VK_NULL_HANDLE)
	, mQueueFamilyIndices{}
{
	mQueueFamilyIndices.graphics = UINT32_MAX;
	mQueueFamilyIndices.compute = UINT32_MAX;
	mQueueFamilyIndices.present = UINT32_MAX;

	Create();
}

VulkanLogicalDevice::~VulkanLogicalDevice()
{
	Destroy();
}

void VulkanLogicalDevice::Create()
{
	assert(mpDevice != nullptr);
	assert(mpDevice->GetPhysicalDeviceHandle() != VK_NULL_HANDLE);

	SelectSupportedQueueFamilies();

	// Within each queue family we can have multiple queues, so each has an index
	// TODO - for now we use the just the first queue from each queue family - index = 0
	const uint32_t defaultQueueIndex = 0;

	// Get queue family indices for the requested queue family types
	// Note that the indices may overlap depending on the implementation
	//TODO - for now we just use the highest priority for all queues
	const bfloat32_t defaultQueuePriority(1.0f); //highest priority

	// TODO - for now we consider to use 1 queue per family !!!

	if (IsGraphicsQueueSupported())
	{
		mQueues.push_back(GE_ALLOC(VulkanQueue)(VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT, mQueueFamilyIndices.graphics, defaultQueueIndex, defaultQueuePriority));
	}

	if (IsComputeQueueSupported())
	{
		mQueues.push_back(GE_ALLOC(VulkanQueue)(VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT, mQueueFamilyIndices.compute, defaultQueueIndex, defaultQueuePriority));
	}

	if (IsPresentQueueSupported())
	{
		mQueues.push_back(GE_ALLOC(VulkanQueue)(VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT, mQueueFamilyIndices.present, defaultQueueIndex, defaultQueuePriority));
	}


	// Create the logical device representation
	std::vector<const char_t*> neededDeviceExtensions(mEnabledDeviceExtensions);

	// check if the needed extension is supported
	bool_t isAvailableKHRSwapChain = false;
	const auto& extensions = mpDevice->GetPhysicalDeviceSupportedExtensions();
	for (const std::string& ext : extensions)
	{
		if (ext == VK_KHR_SWAPCHAIN_EXTENSION_NAME)
		{
			// If the device will be used for presenting to a display via a swapchain we need to request the swapchain extension
			neededDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
			isAvailableKHRSwapChain = true;
			break;
		}
	}

	if (false == isAvailableKHRSwapChain)
	{
		LOG_ERROR("%s extension is not supported! Abort!", VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		return;
	}

	// collect queue create data
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	queueCreateInfos.resize(mQueues.size());

	for (uint32_t i = 0;i < mQueues.size(); ++ i)
	{
		queueCreateInfos[i] = mQueues[i]->GetCreateInfo();
	}

	// NOTE! TODO - improve - enable query pool stats
	VkPhysicalDeviceFeatures enabledDeviceFeatures{}; //VK_FALSE for the other features
	enabledDeviceFeatures.pipelineStatisticsQuery = VK_TRUE;
	
	// large point support
	enabledDeviceFeatures.largePoints = VK_TRUE;

	mpDevice->SetPhysicaltDeviceEnabledFeatures(enabledDeviceFeatures);

	//NOTE! If physical device groups are avaialble and there are at least 2 physical devices to create a logical device from
	// then the VkDeviceGroupDeviceCreateInfo must paased to Next in the VkDeviceCreateInfo struct. It is available since Vulkan 1.1

	VkDeviceCreateInfo deviceCreateInfo =
		VulkanInitializers::DeviceCreateInfo(static_cast<uint32_t>(queueCreateInfos.size()), queueCreateInfos.data(), &mpDevice->GetPhysicaltDeviceEnabledFeatures());

	if (neededDeviceExtensions.size() > 0)
	{
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(neededDeviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = neededDeviceExtensions.data();
	}

	// NOTE! The VkQueue(s) are destroyed with the VkDevice logical device

	VK_CHECK_RESULT(vkCreateDevice(mpDevice->GetPhysicalDeviceHandle(), &deviceCreateInfo, nullptr, &mHandle));

	// get the queue handles
	for (auto& queue : mQueues)
	{
		queue->Init(mHandle);
	}
}

void VulkanLogicalDevice::Destroy()
{
	// NOTE! The VkQueue(s) are destroyed with the VkDevice logical device

	for (auto& queue : mQueues)
	{
		GE_FREE(queue);
	}

	if (mHandle)
	{
		vkDestroyDevice(mHandle, nullptr);
		mHandle = VK_NULL_HANDLE;
	}

	if (mpDevice)
	{
		mpDevice = nullptr;
	}
}

void VulkanLogicalDevice::SelectSupportedQueueFamilies()
{
	assert(mpDevice->GetPhysicalDeviceHandle() != VK_NULL_HANDLE);
	assert(mpDevice->GetSurfaceHandle() != VK_NULL_HANDLE);

	auto& queueFamiliyPropertiesVector = mpDevice->GetQueueFamilyPropertiesVector();
	
	// Search for a graphics and a present queue in the array of queue families
	// try to find one that supports both
	for (uint32_t i = 0; i < queueFamiliyPropertiesVector.size(); ++i)
	{
		// Check graphics queue family support
		if ((queueFamiliyPropertiesVector[i].queueFlags & VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT) != 0)
		{
			if (mQueueFamilyIndices.graphics == UINT32_MAX)
			{
				mQueueFamilyIndices.graphics = i;
			}

			// get info about present for the current queue family
			VkBool32 suportsPresent = VK_FALSE;
			VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceSupportKHR(mpDevice->GetPhysicalDeviceHandle(), i, mpDevice->GetSurfaceHandle(), &suportsPresent));

			if (suportsPresent == VK_TRUE)
			{
				mQueueFamilyIndices.graphics = i;
				mQueueFamilyIndices.present = i;
				break;
			}
		}

		// Check compute queue family support
		if ((queueFamiliyPropertiesVector[i].queueFlags & VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT) != 0)
		{
			if (mQueueFamilyIndices.compute == UINT32_MAX)
			{
				mQueueFamilyIndices.compute = i;
			}

			// get info about present for the current queue family
			VkBool32 suportsPresent = VK_FALSE;
			VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceSupportKHR(mpDevice->GetPhysicalDeviceHandle(), i, mpDevice->GetSurfaceHandle(), &suportsPresent));

			if (suportsPresent == VK_TRUE)
			{
				mQueueFamilyIndices.compute = i;
				mQueueFamilyIndices.present = i;
				break;
			}
		}
	}

	// If there's no queue that supports both present and graphics
	// try to find a separate present queue
	if (mQueueFamilyIndices.present == UINT32_MAX)
	{
		for (uint32_t i = 0; i < queueFamiliyPropertiesVector.size(); ++i)
		{
			// Check separate present/transfer queue family support
			if ((queueFamiliyPropertiesVector[i].queueFlags & VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT) != 0)
			{
				VkBool32 suportsPresent = VK_FALSE;
				VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceSupportKHR(mpDevice->GetPhysicalDeviceHandle(), i, mpDevice->GetSurfaceHandle(), &suportsPresent));

				if (suportsPresent == VK_TRUE)
				{
					mQueueFamilyIndices.present = i;
					break;
				}
			}
		}
	}

	// Exit if either a graphics or a presenting queue hasn't been found
	if ((mQueueFamilyIndices.graphics == UINT32_MAX) || (mQueueFamilyIndices.present == UINT32_MAX))
	{
		LOG_ERROR("The system does not support graphics nor present queues! Abort!");
		return;
	}

	if (mQueueFamilyIndices.present == mQueueFamilyIndices.graphics)
	{
		LOG_INFO("The system supports a queue for both graphics and present!");
	}
	else
	{
		LOG_INFO("The system supports separate graphics and present queues!");
	}

	if (mQueueFamilyIndices.present == mQueueFamilyIndices.compute)
	{
		LOG_INFO("The system supports a queue for both compute and present!");
	}
	else
	{
		LOG_INFO("The system supports separate compute and present queues!");
	}
}

VkResult VulkanLogicalDevice::WaitIdle() const
{
	// wait on the host for the completion of outstanding queue operations for all queues on a given logical device
	// wait until logical device is idle (finish processing device commands)
	return vkDeviceWaitIdle(mHandle);
}

const VkDevice& VulkanLogicalDevice::GetHandle() const
{
	return mHandle;
}

VulkanQueue* VulkanLogicalDevice::GetQueue(VkQueueFlagBits queueFlag) const
{
	for (auto& queue : mQueues)
	{
		if (queue->GetTypeFlags() == queueFlag)
		{
			return queue;
		}
	}

	return nullptr;
}

const VulkanLogicalDevice::QueueFamilyIndices& VulkanLogicalDevice::GetQueueFamilyIndices() const
{
	return mQueueFamilyIndices;
}

bool VulkanLogicalDevice::IsGraphicsQueueSupported() const
{
	return (mQueueFamilyIndices.graphics != UINT32_MAX);
}

bool VulkanLogicalDevice::IsComputeQueueSupported() const
{
	return ((mQueueFamilyIndices.compute != UINT32_MAX) && ((mQueueFamilyIndices.compute != mQueueFamilyIndices.graphics)));
}

bool VulkanLogicalDevice::IsPresentQueueSupported() const
{
	return ((mQueueFamilyIndices.present != UINT32_MAX) && (mQueueFamilyIndices.present != mQueueFamilyIndices.graphics) && ((mQueueFamilyIndices.present != mQueueFamilyIndices.compute)));
}