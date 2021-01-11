#ifndef GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_LOGICAL_DEVICE_HPP
#define GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_LOGICAL_DEVICE_HPP

#include "Graphics/Rendering/Vulkan/Common/VulkanObject.hpp"
#include <unordered_map>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class VulkanDevice;
		class VulkanQueue;

		/*
			*Wrapper for VkDevice object*

			Device objects represent logical connections to physical devices. Each device exposes a number of queue families each having one or more queues.
			All queues in a queue family support the same operations.

			Creating a logical device also creates the queues associated with that device.
			When the logical device is destroyed the queues associated with it are destroyed as well.

			As described in Physical Devices, a Vulkan application will first query for all physical devices in a system. Each physical device can then be 
			queried for its capabilities, including its queue and queue family properties. Once an acceptable physical device is identified, an application 
			will create a corresponding logical device. An application must create a separate logical device for each physical device it will use. The created
			logical device is then the primary interface to the physical device.

			*Starting with Vulakn 1.1 we have physical device groups!

			A single logical device can also be created from multiple physical devices, if those physical devices belong to the same device group.
			A device group is a set of physical devices that support accessing each other’s memory and recording a single command buffer that can be
			executed on all the physical devices. Device groups are enumerated by calling vkEnumeratePhysicalDeviceGroups, and a logical device is 
			created from a subset of the physical devices in a device group by passing the physical devices through VkDeviceGroupDeviceCreateInfo. 
			For two physical devices to be in the same device group, they must support identical extensions, features, and properties.

			Operations on logical device: wait for work to finish
		*/
		class VulkanLogicalDevice : public VulkanObject
		{
			GE_RTTI(GraphicsEngine::Graphics::VulkanLogicalDevice)

		public:
			typedef struct
			{
				uint32_t graphics;
				uint32_t compute;
				uint32_t present;
				//uint32_t sparseMemory;
			} QueueFamilyIndices;

			VulkanLogicalDevice();
			explicit VulkanLogicalDevice(VulkanDevice* pDevice);
			virtual ~VulkanLogicalDevice();

			VkResult WaitIdle() const;

			const VkDevice& GetHandle() const;
			VulkanQueue* GetQueue(VkQueueFlagBits queueFlag) const;
			const VulkanLogicalDevice::QueueFamilyIndices& GetQueueFamilyIndices() const;

			bool_t IsGraphicsQueueSupported() const;
			bool_t IsComputeQueueSupported() const;
			bool_t IsPresentQueueSupported() const;

		private:
			void Create();
			void Destroy();

			void SelectSupportedQueueFamilies();

			VulkanDevice* mpDevice;

			// Logical device, application's view of the physical device (GPU)
			VkDevice mHandle;

			// Queues
			std::unordered_map<VkQueueFlagBits, VulkanQueue*> mQueueMap;

			// Queue family indices
			QueueFamilyIndices mQueueFamilyIndices;

			// enabled extensions
			std::vector<const char_t*> mEnabledDeviceExtensions;

		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_LOGICAL_DEVICE_HPP