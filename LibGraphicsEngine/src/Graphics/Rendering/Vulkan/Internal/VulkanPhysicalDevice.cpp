#include "VulkanPhysicalDevice.hpp"
#include "VulkanDevice.hpp"
#include "VulkanHelpers.hpp"
#include "VulkanInitializers.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanPhysicalDevice::VulkanPhysicalDevice()
	: mpDevice(nullptr)
	, mHandle(VK_NULL_HANDLE)
	, mType(VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM)
	, mProperties{}
	, mFeatures{}
	, mMemoryProperties{}
	, mSurfaceCapabilities{}
	, mEnabledFeatures{}
{}

VulkanPhysicalDevice::VulkanPhysicalDevice(VulkanDevice* pDevice, VkPhysicalDeviceType type)
	: mpDevice(pDevice)
	, mHandle(VK_NULL_HANDLE)
	, mType(type)
	, mProperties{}
	, mFeatures{}
	, mMemoryProperties{}
	, mSurfaceCapabilities{}
	, mEnabledFeatures{}
{
	SelectPhysicalDevice();

	Init();
}

VulkanPhysicalDevice::~VulkanPhysicalDevice()
{
	Terminate();
}

void VulkanPhysicalDevice::SelectPhysicalDevice()
{
	assert(mpDevice != nullptr);

	bool_t foundSuitableGPU = false;
	VkPhysicalDeviceProperties gpuProperties;

#if defined(VK_VERSION_1_1)
	uint32_t gpuGroupCount = 0;
	std::vector<VkPhysicalDeviceGroupProperties> gpuGroupPropertiesVector;
	VK_CHECK_RESULT(vkEnumeratePhysicalDeviceGroups(mpDevice->GetInstanceHandle(), &gpuGroupCount, nullptr));
	assert(gpuGroupCount > 0);

	gpuGroupPropertiesVector.resize(gpuGroupCount);
	for (VkPhysicalDeviceGroupProperties& gpuGroupProperties : gpuGroupPropertiesVector)
	{
		gpuGroupProperties = VulkanInitializers::PhysicalDeviceGroupProperties();
	}
	VK_CHECK_RESULT(vkEnumeratePhysicalDeviceGroups(mpDevice->GetInstanceHandle(), &gpuGroupCount, gpuGroupPropertiesVector.data()));

	VulkanHelpers::ListPhysicalDeviceGroupProperties(gpuGroupPropertiesVector);


	for (auto& gpuGroupProperties : gpuGroupPropertiesVector)
	{
		assert(gpuGroupProperties.physicalDeviceCount > 0);

		for (auto& gpuHandle : gpuGroupProperties.physicalDevices)
		{
			vkGetPhysicalDeviceProperties(gpuHandle, &gpuProperties);

			if (gpuProperties.deviceType == mType)
			{
				mHandle = gpuHandle;
				mProperties = gpuProperties;
				foundSuitableGPU = true;
				break;
			}
		}

		if (foundSuitableGPU)
			break;
	}

#else
	uint32_t gpuCount = 0;
	VK_CHECK_RESULT(vkEnumeratePhysicalDevices(mpDevice->GetInstanceHandle(), &gpuCount, nullptr));
	assert(gpuCount > 0);

	std::vector<VkPhysicalDevice> gpuHandles(gpuCount);
	VK_CHECK_RESULT(vkEnumeratePhysicalDevices(mpDevice->GetInstanceHandle(), &gpuCount, gpuHandles.data()));

	VkPhysicalDeviceProperties gpuProperties;
	for (auto& gpuHandle : gpuHandles)
	{
		vkGetPhysicalDeviceProperties(gpuHandle, &gpuProperties);

		if (gpuProperties.deviceType == mType)
		{
			mHandle = gpuHandle;
			mProperties = gpuProperties;
			foundSuitableGPU = true;
			break;
		}
	}
#endif

	if (foundSuitableGPU == false)
	{
		LOG_ERROR("Failed to find suitable GPU!");
		return;
	}
}

void VulkanPhysicalDevice::Init()
{
	assert(mpDevice != nullptr);
	assert(mpDevice->GetSurfaceHandle() != VK_NULL_HANDLE);

	// NOTE! Vulkan 1.0 supported Device Layers
// see vkEnumerateDeviceLayerProperties() which is deprecated in Vulkan 1.1 and 1.2
// starting with Vulakn 1.1 device layers are now aprt of the instance layers

//////////// DEVICE EXTENSIONS /////////////
	uint32_t extCount = 0;
	std::vector<VkExtensionProperties> deviceSupportedExtensions;
	VK_CHECK_RESULT(vkEnumerateDeviceExtensionProperties(mHandle, nullptr, &extCount, nullptr));

	if (extCount > 0)
	{
		deviceSupportedExtensions.resize(extCount);

		VK_CHECK_RESULT(vkEnumerateDeviceExtensionProperties(mHandle, nullptr, &extCount, deviceSupportedExtensions.data()));

		VulkanHelpers::ListPhysicalDeviceSupportedExtensions(deviceSupportedExtensions);

		for (auto ext : deviceSupportedExtensions)
		{
			mSupportedExtensions.push_back(ext.extensionName);
		}
	}
	else
	{
		LOG_ERROR("This physical device does not support any extensions! Abort!");
		return;
	}

	////// PROPERTIES BEYOND VULKAN 1.0 USING NEWER VERSIONS OF VULKAN OR EXTENSIONS //////////

	// Query for Driver info
	bool_t isDriverPropertiesSupported = false;
#if defined(VK_VERSION_1_2)
	VkPhysicalDeviceDriverProperties driverProperties = VulkanInitializers::PhysicalDeviceDriverProperties();
	isDriverPropertiesSupported = true;
#else //extension
	bool_t isDriverPropertiesSupported = false;
	for (auto ext : deviceSupportedExtensions)
	{
		if (std::string(ext.extensionName) == VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME)
		{
			isDriverPropertiesSupported = true;
			break;
		}
	}

	VkPhysicalDeviceDriverPropertiesKHR driverProperties = VulkanInitializers::PhysicalDeviceDriverPropertiesKHR();
#endif // defined(VK_VERSION_1_2)

#if defined(VK_VERSION_1_1)
	if (isDriverPropertiesSupported)
	{
		VkPhysicalDeviceProperties2 properties2 = VulkanInitializers::PhysicalDeviceProperties2();

		properties2.pNext = &driverProperties;
		vkGetPhysicalDeviceProperties2(mHandle, &properties2);

		VulkanHelpers::ListPhysicalDeviceDriverProperties(driverProperties);
	}
#endif // defined(VK_VERSION_1_1)

	/////////////////////////////////////////////////////////

	// Properties
	//vkGetPhysicalDeviceProperties(mHandle, &mProperties);
	// we obtained the proprties when we selected the suirable gpu
	VulkanHelpers::ListPhysicalDeviceProperties(mProperties);

	// Features
	vkGetPhysicalDeviceFeatures(mHandle, &mFeatures);
	VulkanHelpers::ListPhysicalDeviceFeatures(mFeatures);

	// Memory Properties
	vkGetPhysicalDeviceMemoryProperties(mHandle, &mMemoryProperties);
	VulkanHelpers::ListPhysicalDeviceMemoryProperties(mMemoryProperties);

	// Surface Capabilities
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mHandle, mpDevice->GetSurfaceHandle(), &mSurfaceCapabilities));

	// Surface Formats
	uint32_t formatCount = 0;
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(mHandle, mpDevice->GetSurfaceHandle(), &formatCount, nullptr));
	assert(formatCount > 0);

	mSurfaceFormats.resize(formatCount);
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(mHandle, mpDevice->GetSurfaceHandle(), &formatCount, mSurfaceFormats.data()));

	// Surface Present Modes
	uint32_t presentModeCount = 0;
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(mHandle, mpDevice->GetSurfaceHandle(), &presentModeCount, nullptr));
	assert(presentModeCount > 0);

	mSurfacePresentModes.resize(presentModeCount);
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(mHandle, mpDevice->GetSurfaceHandle(), &presentModeCount, mSurfacePresentModes.data()));

	// Queue Family Properties
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(mHandle, &queueFamilyCount, nullptr);

	assert(queueFamilyCount > 0);
	mQueueFamilyPropertiesVector.resize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(mHandle, &queueFamilyCount, mQueueFamilyPropertiesVector.data());
}

void VulkanPhysicalDevice::SelectSurfaceFormat(VkSurfaceFormatKHR& outSurfaceFormat)
{
	// If the surface format list only includes one entry with VK_FORMAT_UNDEFINED,
	// there is no preferered format, so we assume VK_FORMAT_B8G8R8A8_UNORM
	if ((mSurfaceFormats.size() == 1) && (mSurfaceFormats[0].format == VkFormat::VK_FORMAT_UNDEFINED))
	{
		outSurfaceFormat.format = VkFormat::VK_FORMAT_B8G8R8A8_UNORM;
		outSurfaceFormat.colorSpace = VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;// surfaceFormats[0].colorSpace;
	}
	else
	{
		// iterate over the list of available surface format and
		// check for the presence of VK_FORMAT_B8G8R8A8_UNORM
		bool_t found_B8G8R8A8_UNORM = false;
		for (auto&& surfaceFormat : mSurfaceFormats)
		{
			if (surfaceFormat.format == VkFormat::VK_FORMAT_B8G8R8A8_UNORM)
			{
				outSurfaceFormat.format = surfaceFormat.format;
				outSurfaceFormat.colorSpace = surfaceFormat.colorSpace;
				found_B8G8R8A8_UNORM = true;
				break;
			}
		}

		// in case VK_FORMAT_B8G8R8A8_UNORM is not available
		// select the first available color format
		if (!found_B8G8R8A8_UNORM)
		{
			outSurfaceFormat.format = mSurfaceFormats[0].format;
			outSurfaceFormat.colorSpace = mSurfaceFormats[0].colorSpace;
		}
	}
}

VkFormat VulkanPhysicalDevice::SelectDepthFormat()
{
	// Since all depth formats may be optional, we need to find a suitable depth format to use
	// Start with the highest precision packed format
	std::vector<VkFormat> depthFormats =
	{
		VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT,
		VkFormat::VK_FORMAT_D32_SFLOAT,
		VkFormat::VK_FORMAT_D24_UNORM_S8_UINT,
		VkFormat::VK_FORMAT_D16_UNORM_S8_UINT,
		VkFormat::VK_FORMAT_D16_UNORM
	};

	return SelectSupportedFormat(depthFormats, VkImageTiling::VK_IMAGE_TILING_OPTIMAL, VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

VkFormat VulkanPhysicalDevice::SelectSupportedFormat(const std::vector<VkFormat>& formats, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	for (auto& format : formats)
	{
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(mHandle, format, &formatProperties);

		if (tiling == VkImageTiling::VK_IMAGE_TILING_LINEAR && (formatProperties.linearTilingFeatures & features) == features)
		{
			return format;
		}
		else if (tiling == VkImageTiling::VK_IMAGE_TILING_OPTIMAL && (formatProperties.optimalTilingFeatures & features) == features)
		{
			return format;
		}
	}

	return VkFormat::VK_FORMAT_UNDEFINED;
}

void VulkanPhysicalDevice::Terminate()
{
	mProperties = {};
	mFeatures = {};
	mMemoryProperties = {};
	mSurfaceCapabilities = {};

	if (mHandle)
	{
		mHandle = VK_NULL_HANDLE;
	}
}

void VulkanPhysicalDevice::UpdateSurfaceCapabilities()
{
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mHandle, mpDevice->GetSurfaceHandle(), &mSurfaceCapabilities));
}

bool VulkanPhysicalDevice::GetMemoryType(uint32_t typeBits, VkMemoryPropertyFlags requirementsMask, uint32_t& typeIndex)
{
	// Search memtypes to find first index with those properties
	for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES; ++i)
	{
		if ((typeBits & 1) == 1)
		{
			// Type is available, does it match user properties?
			if ((mMemoryProperties.memoryTypes[i].propertyFlags & requirementsMask) == requirementsMask)
			{
				typeIndex = i;
				return true;
			}
		}
		typeBits >>= 1;
	}

	// No memory types matched, return failure
	return false;
}

const VkPhysicalDevice& VulkanPhysicalDevice::GetHandle() const
{
	return mHandle;
}

const VkPhysicalDeviceProperties& VulkanPhysicalDevice::GetProperties() const
{
	return mProperties;
}

const VkPhysicalDeviceFeatures& VulkanPhysicalDevice::GetFeatures() const
{
	return mFeatures;
}

const VkPhysicalDeviceMemoryProperties& VulkanPhysicalDevice::GetMemoryProperties() const
{
	return mMemoryProperties;
}

const VkSurfaceCapabilitiesKHR& VulkanPhysicalDevice::GetSurfaceCapabilities() const
{
	return mSurfaceCapabilities;
}

const std::vector<VkSurfaceFormatKHR>& VulkanPhysicalDevice::GetSurfaceFormats() const
{
	return mSurfaceFormats;
}

const std::vector<VkPresentModeKHR>& VulkanPhysicalDevice::GetSurfacePresentModes() const
{
	return mSurfacePresentModes;
}

const std::vector<VkQueueFamilyProperties>& VulkanPhysicalDevice::GetQueueFamilyPropertiesVector() const
{
	return mQueueFamilyPropertiesVector;
}

const std::vector<std::string>& VulkanPhysicalDevice::GetSupportedExtensions() const
{
	return mSupportedExtensions;
}


void VulkanPhysicalDevice::SetSupportedExtensions(const std::vector<std::string>& extensions)
{
	mSupportedExtensions.clear();
	mSupportedExtensions.resize(extensions.size());
	for (size_t i = 0; i < mSupportedExtensions.size(); ++i)
	{
		mSupportedExtensions[i] = extensions[i];
	}
}

const VkPhysicalDeviceFeatures& VulkanPhysicalDevice::GetEnabledFeatures() const
{
	return mEnabledFeatures;
}

void VulkanPhysicalDevice::SetEnabledFeatures(const VkPhysicalDeviceFeatures& features)
{
	mEnabledFeatures = features;
}