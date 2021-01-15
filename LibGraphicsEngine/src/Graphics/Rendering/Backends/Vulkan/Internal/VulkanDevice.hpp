#ifndef GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_DEVICE_HPP
#define GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_DEVICE_HPP

#include "Core/AppConfig.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Common/VulkanObject.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanLogicalDevice.hpp"
#include <vector>
#include <string>

namespace GraphicsEngine
{
	namespace Platform
	{
		class Window;
	}
	namespace Graphics
	{
		class VulkanInstance;
		class VulkanPhysicalDevice;
		class VulkanAllocator;
		class VulkanSurface;
		class VulkanSwapChain;
		class VulkanSwapChainBuffer;
		class VulkanQueue;

		/*
			Wrapper to contain the objects that compose the Vulkan context.
			Basically contains the: instance, physical and logical devices, swap chains, surface
			and the main graphics queue
		*/
		class VulkanDevice : public VulkanObject
		{
			GE_RTTI(GraphicsEngine::Graphics::VulkanDevice)

		public:
			VulkanDevice();
			explicit VulkanDevice(Platform::Window* pWindow, bool_t enableValidation);
			virtual ~VulkanDevice();

			void ResetSwapChain();

			VkResult AcquireNextImage(uint32_t* pImageIndex, VkSemaphore presentCompleteSemaphoreHandle) const;

			VkResult WaitIdle() const;

			void UpdateSurfaceCapabilities();

			// instance
			const VkInstance& GetInstanceHandle() const;

			// surface
			const VkSurfaceKHR& GetSurfaceHandle() const;

			// physical device
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
			bool_t GetMemoryTypeFromProperties(uint32_t typeBits, VkMemoryPropertyFlags requirementsMask, uint32_t& typeIndex);

			const VkSurfaceFormatKHR& GetSurfaceFormat() const;
			const VkFormat& GetDepthStencilFormat() const;

			// logical device
			const VkDevice& GetDeviceHandle() const;
			const VulkanLogicalDevice::QueueFamilyIndices& GetQueueFamilyIndices() const;

			// swap chain
			const VkSwapchainKHR& GetSwapChainHandle() const;
			const std::vector<VulkanSwapChainBuffer*>& GetSwapChainColorBuffers() const;
			uint32_t GetSwapChainColorBufferCount() const;
			VulkanSwapChainBuffer* GetSwapChainDepthStencilBuffer() const;

			// window
			Platform::Window* GetWindow() const;

			// queues
			bool_t IsGraphicsQueueSupported() const;
			bool_t IsComputeQueueSupported() const;
			bool_t IsPresentQueueSupported() const;

			VulkanQueue* GetGraphicsQueue() const;
			VulkanQueue* GetPresentQueue() const;
			VulkanQueue* GetComputeQueue() const;

			// allocator
			VulkanAllocator* GetAllocator() const;


		private:
			void Create();
			void Destroy();


			//// SwapChain ////
			void SetupConnection();

			////////////// DEVICE //////////////////

			VulkanInstance* mpInstance;

			VulkanPhysicalDevice* mpPhysicalDevice;

			VulkanLogicalDevice* mpLogicalDevice;


			///////////// SURFACE /////////////
			Platform::Window* mpWindow;

			VulkanSurface* mpSurface;

			////////////

			VulkanAllocator* mpAllocator;

			//////////// SWAP CHAIN /////////////
			VulkanSwapChain* mpSwapChain;

			//// Surface format
			VkSurfaceFormatKHR mSurfaceFormat;
			//// Depth Stencil Format
			VkFormat mDepthStencilFormat;

			// DEBUG
			bool_t mEnableValidationLayers;
		};
	}
}

#endif // GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_DEVICE_HPP