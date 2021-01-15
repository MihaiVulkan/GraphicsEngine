#ifndef GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_PHYSICAL_DEVICE_HPP
#define GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_PHYSICAL_DEVICE_HPP

#include "Graphics/Rendering/Backends/Vulkan/Common/VulkanObject.hpp"
#include <vector>
#include <string>

#define DEPTH_STENCIL_FORMAT_COUNT 5

namespace GraphicsEngine
{
	namespace Graphics
	{
		class VulkanDevice;

		/*
			*Wrapper for VkPhysicalDevice object*

			Vulkan separates the concept of physical and logical devices. A physical device usually represents a single complete implementation
			of Vulkan (excluding instance-level functionality) available to the host, of which there are a finite number. A logical device represents 
			an instance of that implementation with its own state and resources independent of other logical devices.

			*Usually a physical device means the GPU. Vulkan supports several GPU types. We usually use the discrete GPU type.

			*Extensions - Device extensions add new device-level functionality to the API, outside of the core specification.

			Some extensions are dependent on other extensions to function. To enable extensions with dependencies, such required extensions must also
			be enabled through the same API mechanisms when creating an instance with vkCreateInstance or a device with vkCreateDevice. Each extension 
			which has such dependencies documents them in the appendix summarizing that extension.

			If an extension is supported (as queried by vkEnumerateInstanceExtensionProperties or vkEnumerateDeviceExtensionProperties), then required 
			extensions of that extension must also be supported for the same instance or physical device.

			Any device extension that has an instance extension dependency that is not enabled by vkCreateInstance is considered to be unsupported,
			hence it must not be returned by vkEnumerateDeviceExtensionProperties for any VkPhysicalDevice child of the instance.

			*Features - Features describe functionality which is not supported on all implementations. Features are properties of the physical device. 
			Features are optional, and must be explicitly enabled before use. Support for features is reported and enabled on a per-feature basis.

			*Starting with Vulakn 1.1 we have physical device groups!

			A single logical device can also be created from multiple physical devices, if those physical devices belong to the same device group.
			A device group is a set of physical devices that support accessing each other’s memory and recording a single command buffer that can be
			executed on all the physical devices. Device groups are enumerated by calling vkEnumeratePhysicalDeviceGroups, and a logical device is
			created from a subset of the physical devices in a device group by passing the physical devices through VkDeviceGroupDeviceCreateInfo.
			For two physical devices to be in the same device group, they must support identical extensions, features, and properties.
		*/
		class VulkanPhysicalDevice : public VulkanObject
		{
			GE_RTTI(GraphicsEngine::Graphics::VulkanPhysicalDevice)

		public:
			VulkanPhysicalDevice();
			explicit VulkanPhysicalDevice(VulkanDevice* pDevice, VkPhysicalDeviceType type = VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
			virtual ~VulkanPhysicalDevice();

			bool_t GetMemoryType(uint32_t typeBits, VkMemoryPropertyFlags requirementsMask, uint32_t& typeIndex);

			void SelectSurfaceFormat(VkSurfaceFormatKHR& outSurfaceFormat);
			VkFormat SelectDepthStencilFormat();

			// on surface extent change - window resize
			void UpdateSurfaceCapabilities();


			const VkPhysicalDevice& GetHandle() const;
			const VkPhysicalDeviceProperties& GetProperties() const;
			const VkPhysicalDeviceFeatures& GetFeatures() const;
			const VkPhysicalDeviceMemoryProperties& GetMemoryProperties() const;

			const VkSurfaceCapabilitiesKHR& GetSurfaceCapabilities() const;
			const std::vector<VkSurfaceFormatKHR>& GetSurfaceFormats() const;
			const std::vector<VkPresentModeKHR>& GetSurfacePresentModes() const;
			const std::vector<VkQueueFamilyProperties>& GetQueueFamilyPropertiesVector() const;

			const std::vector<std::string>& GetSupportedExtensions() const;
			void SetSupportedExtensions(const std::vector<std::string>& extensions);

			const VkPhysicalDeviceFeatures& GetEnabledFeatures() const;
			void SetEnabledFeatures(const VkPhysicalDeviceFeatures& features);

		private:
			void SelectPhysicalDevice();
			void Init();
			void Terminate();  
			VkFormat SelectSupportedFormat(const std::array<VkFormat, DEPTH_STENCIL_FORMAT_COUNT>& formats, VkImageTiling tiling, VkFormatFeatureFlags features);

			VulkanDevice* mpDevice;

			// GPU
			VkPhysicalDevice mHandle;
			
			VkPhysicalDeviceType mType;

			VkPhysicalDeviceProperties mProperties;
			
			VkPhysicalDeviceFeatures mFeatures;
			
			VkPhysicalDeviceMemoryProperties mMemoryProperties;

			VkSurfaceCapabilitiesKHR mSurfaceCapabilities;

			std::vector<VkSurfaceFormatKHR> mSurfaceFormats;

			std::vector<VkPresentModeKHR> mSurfacePresentModes;

			std::vector<VkQueueFamilyProperties> mQueueFamilyPropertiesVector;


			std::vector<std::string> mSupportedExtensions;
			VkPhysicalDeviceFeatures mEnabledFeatures;

		};
	}
}

#endif // GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_PHYSICAL_DEVICE_HPP