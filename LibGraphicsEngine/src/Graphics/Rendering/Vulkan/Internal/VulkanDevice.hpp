#ifndef GRAPHICS_RENDERING_VULKAN_DEVICE_HPP
#define GRAPHICS_RENDERING_VULKAN_DEVICE_HPP

#include "AppConfig.hpp"
#include "Foundation/TypeDefs.hpp"
#include "VulkanLogicalDevice.hpp"
#include "vulkan/vulkan.h"
#include <vector>
#include <string>

namespace GraphicsEngine
{
	namespace Platform
	{
		class GE_Window;
	}
	namespace Graphics
	{
		class VulkanInstance;
		class VulkanPhysicalDevice;
		class VulkanLogicalDevice;
		class VulkanSurface;
		class VulkanSwapChain;
		class VulkanFrameBufferAttachment;
		class VulkanCommandPool;
		class VulkanQueue;
		class VulkanAllocator;

		/*
			Wrapper to contain the objects that compose the Vulkan context.
			Basically contains the: instance, physical and logical devices, swap chains, surface
			and the main graphics queue
		*/
		class VulkanDevice
		{
		public:
			VulkanDevice();
			explicit VulkanDevice(Platform::GE_Window* pWindow, bool_t enableValidation);
			virtual ~VulkanDevice();

			VkResult AcquireNextImage(uint32_t* pImageIndex, VkSemaphore presentCompleteSemaphoreHandle) const;

			VkResult WaitIdle() const;

			void UpdateSurfaceCapabilities();

			const VkInstance& GetInstanceHandle() const;
			const VkPhysicalDevice& GetPhysicalDeviceHandle() const;
			const VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() const;
			const VkPhysicalDeviceFeatures& GetPhysicalDeviceFeatures() const;
			const VkPhysicalDeviceMemoryProperties& GetPhysicalDeviceMemoryProperties() const;
			const std::vector<std::string>& GetPhysicalDeviceSupportedExtensions() const;
			void SetPhysicalDeviceSupportedExtensions(const std::vector<std::string>& extensions);
			const VkPhysicalDeviceFeatures& GetPhysicaltDeviceEnabledFeatures() const;
			void SetPhysicaltDeviceEnabledFeatures(const VkPhysicalDeviceFeatures& features);
			const VkSurfaceCapabilitiesKHR& GetSurfaceCapabilities() const;
			const std::vector<VkSurfaceFormatKHR>& GetSurfaceFormats() const;
			const std::vector<VkPresentModeKHR>& GetSurfacePresentModes() const;
			const std::vector<VkQueueFamilyProperties>& GetQueueFamilyPropertiesVector() const;
			const VulkanLogicalDevice::QueueFamilyIndices& GetQueueFamilyIndices() const;
			const VkDevice& GetDeviceHandle() const;
			const VkSurfaceKHR& GetSurfaceHandle() const;
			const VkSurfaceFormatKHR& GetSurfaceFormat() const;
			const VkFormat& GetDepthFormat() const;
			const VkSwapchainKHR& GetSwapChainHandle() const;
			uint32_t GetSwapChainImageCount() const;
			const std::vector<VulkanFrameBufferAttachment*>& GetSwapChainBuffers() const;

			bool_t IsGraphicsQueueSupported() const;
			bool_t IsComputeQueueSupported() const;
			bool_t IsPresentQueueSupported() const;

			const Platform::GE_Window* GetWindow() const;

			VulkanQueue* GetMainGraphicsQueue() const;
			VulkanQueue* GetPresentQueue() const;

			const VkCommandPool& GetCommandPoolHandle() const;

			VulkanAllocator* GetAllocator() const;

			bool_t GetMemoryTypeFromProperties(uint32_t typeBits, VkMemoryPropertyFlags requirementsMask, uint32_t& typeIndex);

			///////
			void ResetSwapChain();

		private:
			void Create(Platform::GE_Window* pWindow, bool_t enableValidation);
			void Destroy();


			//// SwapChain ////
			void SetupConnection();

			////////////// DEVICE //////////////////
			VulkanInstance* mpInstance;

			VulkanPhysicalDevice* mpPhysicalDevice;

			VulkanLogicalDevice* mpLogicalDevice;

			///////////// SURFACE /////////////
			Platform::GE_Window* mpWindow;

			VulkanSurface* mpSurface;

			//////////// SWAP CHAIN /////////////
			VulkanSwapChain* mpSwapChain;

			//// Surface format
			VkSurfaceFormatKHR mSurfaceFormat;
			//// Depth Format
			VkFormat mDepthFormat;

			///// MAIN GRAPHICS QUEUE 
			VulkanQueue* mpMainGraphicsQueue;
			VulkanQueue* mpPresentQueue;

			//// COMMANDS
			VulkanCommandPool* mpCommandPool;

			// ALLOCATOR
			VulkanAllocator* mpAllocator;

			// DEBUG
			bool_t mEnableValidationLayers;
		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_DEVICE_HPP