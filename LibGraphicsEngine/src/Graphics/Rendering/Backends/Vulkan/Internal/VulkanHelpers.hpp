#ifndef GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_HELPERS_HPP
#define GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_HELPERS_HPP

#include "Foundation/TypeDefines.hpp"
#include "Foundation/Logger.hpp"
#include "vulkan/vulkan.h"
#include <string>
#include <vector>
#include <iostream>
#include <string>
#include <cassert>

namespace GraphicsEngine
{
	namespace Graphics
	{
		/*
			Wrapper for several Vulkan helper functionalities
		*/
		namespace VulkanHelpers
		{

			// Default fence timeout in nanoseconds
#define DEFAULT_FENCE_TIMEOUT 100000000000

#define OFFSETOF(s,m) ((::size_t)&reinterpret_cast<char const volatile&>((((s*)0)->m)))

			std::string GetVulkanVersionStr(uint32_t version);

			std::string ErrorString(VkResult errorCode);

			/*std::cout << "Fatal : VkResult is \"" << VulkanHelpers::ErrorString(vk_res) << "\" " << __LINE__ << std::endl;	\*/
#define VK_CHECK_RESULT(funCall)																				\
	{																										\
		VkResult vkRes = (funCall);																					\
		if (vkRes != VkResult::VK_SUCCESS)																				\
		{																									\
			LOG_DEBUG("Fatal : VkResult is \"%s\"", VulkanHelpers::ErrorString(vkRes).c_str());		\
			assert(vkRes == VkResult::VK_SUCCESS);																		\
		}																									\
	}
			void ListInstanceSupportedLayers(const std::vector<VkLayerProperties>& supportedInstanceLayers);

			void ListInstanceSupportedExtensions(const std::vector<VkExtensionProperties>& supportedInstanceExtensions);

#if defined(VK_VERSION_1_1)
			void ListPhysicalDeviceGroupProperties(const std::vector<VkPhysicalDeviceGroupProperties>& physicalDeviceGroupPropertiesVector);
#endif // defined(VK_VERSION_1_1)

			void ListPhysicalDeviceSupportedExtensions(const std::vector<VkExtensionProperties>& physicalDeviceSupportedExtensions);

			void ListPhysicalDeviceProperties(const VkPhysicalDeviceProperties& physicalDeviceProperties);

			void ListPhysicalDeviceFeatures(const VkPhysicalDeviceFeatures& physicalDeviceFeatures);

			void ListPhysicalDeviceMemoryProperties(const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties);
#if defined(VK_VERSION_1_1) && defined(VK_VERSION_1_2) //for Vulkan 1.2 and Vulkan 1.1 which ahs the KHR driver extensiosn
			void ListPhysicalDeviceDriverProperties(const VkPhysicalDeviceDriverProperties& physicalDeviceDriverProperties);
#endif // // defined(VK_VERSION_1_1) && defined(VK_VERSION_1_2)

			void ListPhysicalDeviceImageFormatProperties(VkFormat imageFormat, const VkImageFormatProperties& imageFormatProperties);
		}
	}
}

#endif // GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_HELPERS_HPP