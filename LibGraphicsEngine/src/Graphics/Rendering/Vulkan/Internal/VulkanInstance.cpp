#include "AppConfig.hpp"
#include "VulkanInstance.hpp"
#include "VulkanDebug.hpp"
#include "VulkanInitializers.hpp"
#include "VulkanHelpers.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

// TODO - improve layer support
#define VK_VALIDATION_LAYER_LUNARG "VK_LAYER_LUNARG_standard_validation"
#define VK_VALIDATION_LAYER_KHRONOS "VK_LAYER_KHRONOS_validation"

VulkanInstance::VulkanInstance()
	: mHandle(VK_NULL_HANDLE)
	, mEnableValidation(false)
{}

VulkanInstance::VulkanInstance(const char_t* pTitle, bool_t enableValidation)
	: mHandle(VK_NULL_HANDLE)
	, mEnableValidation(enableValidation)
{
	Create(pTitle);
}

VulkanInstance::~VulkanInstance()
{
	Destroy();
}

void VulkanInstance::Create(const char_t* pTitle)
{
	////////// VULKAN VERSION /////////
// Vulkan Header Version
	uint32_t vkTargettedInstanceVersion = 0;

#if defined(VK_VERSION_1_2)
	LOG_INFO("Vulkan Header Version: 1.2");
	LOG_INFO("Vulkan Available API Versions:\n%u (v1.0.0)\n%u (v1.1.0)\n%u (v1.2.0)\n", VK_API_VERSION_1_0, VK_API_VERSION_1_1, VK_API_VERSION_1_2);
	vkTargettedInstanceVersion = VK_API_VERSION_1_2;
	LOG_INFO("Vulkan Targetted API Version: %u (v1.2.0)", vkTargettedInstanceVersion);
#elif defined(VK_VERSION_1_1)
	LOG_INFO("Vulkan Header Version: 1.1");
	LOG_INFO("Vulkan Available API Versions:\n%u (v1.0.0)\n%u (v1.1.0)\n", VK_API_VERSION_1_0, VK_API_VERSION_1_1);
	vkTargettedInstanceVersion = VK_API_VERSION_1_1;
	LOG_INFO("Vulkan Targetted API Version: %u (v1.1.0)", vkTargettedInstanceVersion);
#else //defined(VK_VERSION_1_0))
	LOG_INFO("Vulkan Header Version: 1.0.0");
	LOG_INFO("Vulkan Available API Versions:\n%u (v1.0.0)\n", VK_API_VERSION_1_0);
	vkTargettedInstanceVersion = VK_API_VERSION_1_0;
	LOG_INFO("Vulkan Targetted API Version: %u (v1.0.0)", vkTargettedInstanceVersion);
#endif

	/////////// LAYERS /////////////

	std::vector<VkLayerProperties> supportedInstanceLayers;
	uint32_t layerCount = 0;
	VK_CHECK_RESULT(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));
	if (layerCount > 0)
	{
		supportedInstanceLayers.resize(layerCount);
		VK_CHECK_RESULT(vkEnumerateInstanceLayerProperties(&layerCount, supportedInstanceLayers.data()));

		VulkanHelpers::ListInstanceSupportedLayers(supportedInstanceLayers);
	}
	else
	{
		LOG_ERROR("This Vulkan instance does not support any layers! Abort!");
		return;
	}

	// needed instance extensions
	std::vector<const char_t*> neededInstanceLayers;
	bool_t isAvailableLunarGValidationLayer = false;
	bool_t isAvailableKhronosValidationLayer = false;

	for (const VkLayerProperties& layer : supportedInstanceLayers)
	{
		if (mEnableValidation)
		{
			if ((isAvailableKhronosValidationLayer == false) && (std::string(layer.layerName) == VK_VALIDATION_LAYER_LUNARG))
			{
				neededInstanceLayers.push_back(VK_VALIDATION_LAYER_LUNARG);
				isAvailableLunarGValidationLayer = true;
			}
			if ((isAvailableLunarGValidationLayer == false) && (std::string(layer.layerName) == VK_VALIDATION_LAYER_KHRONOS))
			{
				neededInstanceLayers.push_back(VK_VALIDATION_LAYER_KHRONOS);
				isAvailableKhronosValidationLayer = true;
			}
		}

		//TODO - add support for other layers !!!
	}

	if (mEnableValidation)
	{
		if (false == isAvailableLunarGValidationLayer)
		{
			LOG_WARNING("%s layer is not supported!", VK_VALIDATION_LAYER_LUNARG);
		}
		if (false == isAvailableKhronosValidationLayer)
		{
			LOG_WARNING("%s layer is not supported!", VK_VALIDATION_LAYER_KHRONOS);
		}

		if ((false == isAvailableLunarGValidationLayer) && (false == isAvailableKhronosValidationLayer))
		{
			LOG_ERROR("No validation layer is supported! Abort!");
			return;
		}
	}

	/////////// EXTENSIONS ////////////

	// get instance supported extensions
	std::vector<VkExtensionProperties> supportedInstanceExtensions;
	uint32_t extensionCount = 0;
	VK_CHECK_RESULT(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr));

	if (extensionCount > 0)
	{
		supportedInstanceExtensions.resize(extensionCount);
		VK_CHECK_RESULT(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, supportedInstanceExtensions.data()));

		VulkanHelpers::ListInstanceSupportedExtensions(supportedInstanceExtensions);
	}
	else
	{
		LOG_ERROR("This Vulkan instance does not support any extensions! Abort!");
		return;
	}

	// needed instance extensions
	std::vector<const char_t*> neededInstanceExtensions;

	// attempt to add the needed extensions
	bool_t isAvailableKHRSurface = false;
#if defined(_WIN32)
	bool_t isAvailableKHRWin32Surface = false;
#endif //_WIN32
	bool_t isAvailableDebugExtension = false;

	for (const VkExtensionProperties& extension : supportedInstanceExtensions)
	{
		if (std::string(extension.extensionName) == VK_KHR_SURFACE_EXTENSION_NAME)
		{
			neededInstanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
			isAvailableKHRSurface = true;
		}

		// Enable surface extensions depending on os
#if defined(_WIN32)
		if (std::string(extension.extensionName) == VK_KHR_WIN32_SURFACE_EXTENSION_NAME)
		{
			neededInstanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
			isAvailableKHRWin32Surface = true;
		}
#endif // _WIN32

		if (mEnableValidation)
		{
			if (std::string(extension.extensionName) == VK_EXT_DEBUG_REPORT_EXTENSION_NAME)
			{
				neededInstanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
				isAvailableDebugExtension = true;
			}
		}
	}

	// check if the needed extensions are available indeed
	if (false == isAvailableKHRSurface)
	{
		LOG_ERROR("%s extension is not supported! Abort!", VK_KHR_SURFACE_EXTENSION_NAME);
		return;
	}
#if defined(_WIN32)
	if (false == isAvailableKHRWin32Surface)
	{
		LOG_ERROR("%s extension is not supported! Abort!", VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
		return;
	}
#endif //_WIN32
	if (mEnableValidation)
	{
		if (false == isAvailableDebugExtension)
		{
			LOG_ERROR("%s extension is not supported! Abort!", VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
			return;
		}
	}

	// mEnabledInstanceExtensions may be overriden in derived classes
	if (mEnabledInstanceExtensions.size() > 0)
	{
		for (auto enabledExtension : mEnabledInstanceExtensions)
		{
			neededInstanceExtensions.push_back(enabledExtension);
		}
	}

	VkApplicationInfo applicationInfo = VulkanInitializers::ApplicationInfo(pTitle, 1, pTitle, 1, vkTargettedInstanceVersion);

	VkInstanceCreateInfo instanceCreateInfo = VulkanInitializers::InstanceCreateInfo(&applicationInfo);
	if (neededInstanceLayers.size() > 0)
	{
		instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(neededInstanceLayers.size());
		instanceCreateInfo.ppEnabledLayerNames = neededInstanceLayers.data();
	}

	if (neededInstanceExtensions.size() > 0)
	{
		instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(neededInstanceExtensions.size());
		instanceCreateInfo.ppEnabledExtensionNames = neededInstanceExtensions.data();
	}

	// Create VK instance
	VK_CHECK_RESULT(vkCreateInstance(&instanceCreateInfo, nullptr, &mHandle));

	// Vulkan Supported Instance Version
	uint32_t vkInstanceVersion = 0;
	vkEnumerateInstanceVersion(&vkInstanceVersion);
	LOG_INFO("Vulkan Supported Instance Version: %u (v%s)", vkInstanceVersion, VulkanHelpers::GetVulkanVersionStr(vkInstanceVersion).c_str());

	// If requested, we enable the default validation layers for debugging
	if (mEnableValidation)
	{
		// The report flags determine what type of messages for the layers will be displayed
		// For validating (debugging) an appplication the error and warning bits should suffice
		VkDebugReportFlagsEXT debugReportFlags = VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_ERROR_BIT_EXT | VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_WARNING_BIT_EXT;
		// Additional flags include performance info, loader and layer debug messages, etc.
		VulkanDebug::SetupDebugging(mHandle, debugReportFlags, VK_NULL_HANDLE);

		//#if defined(_WIN32)
		//		// Enable console if validation is active
		//		// Debug message callback will output to it
		//		AllocConsole();
		//		AttachConsole(GetCurrentProcessId());
		//		FILE* stream = nullptr;
		//		freopen_s(&stream, "CONOUT$", "w+", stdout);
		//		freopen_s(&stream, "CONOUT$", "w+", stderr);
		//		SetConsoleTitle(TEXT("Vulkan validation output"));
		//#endif //_WIN32
	}
}

void VulkanInstance::Destroy()
{
	if (mEnableValidation)
	{
		VulkanDebug::FreeDebugCallback(mHandle);
		mEnableValidation = false;
	}

	if (mHandle)
	{
		vkDestroyInstance(mHandle, nullptr);
		mHandle = VK_NULL_HANDLE;
	}
}

const VkInstance& VulkanInstance::GetHandle() const
{
	return mHandle;
}

const std::vector<const char_t*>& VulkanInstance::GetEnabledInstanceExtensions() const
{
	return mEnabledInstanceExtensions;
}

void VulkanInstance::SetEnabledInstanceExtensions(const std::vector<const char_t*>& enabledExtensions)
{
	mEnabledInstanceExtensions.clear();
	mEnabledInstanceExtensions.resize(enabledExtensions.size());
	for (size_t i = 0; i < mEnabledInstanceExtensions.size(); ++i)
	{
		mEnabledInstanceExtensions[i] = enabledExtensions[i];
	}
}
