#ifndef GRAPHICS_RENDERING_VULKAN_SWAPCHAIN_HPP
#define GRAPHICS_RENDERING_VULKAN_SWAPCHAIN_HPP

#include "Foundation/TypeDefs.hpp"
#include "vulkan/vulkan.h"
#include <vector>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class VulkanDevice;
		class VulkanFrameBufferAttachment;

		/*
			*Wrapper for VkSwapchainKHR object*

			A swapchain object (a.k.a. swapchain) provides the ability to present rendering results to a surface.

			A swapchain is an abstraction for an array of presentable images that are associated with a surface. The presentable 
			images are represented by VkImage objects created by the platform. One image (which can be an array image for 
			multiview/stereoscopic-3D surfaces) is displayed at a time, but multiple images can be queued for presentation. 
			An application renders to the image, and then queues the image for presentation to the surface.

			A native window cannot be associated with more than one non-retired swapchain at a time. Further, swapchains cannot 
			be created for native windows that have a non-Vulkan graphics API surface associated with them.
		*/
		class VulkanSwapChain
		{
		public:
			VulkanSwapChain();
			explicit VulkanSwapChain(VulkanDevice* pDevice);
			virtual ~VulkanSwapChain();

			VkResult AcquireNextImage(uint32_t* pImageIndex, VkSemaphore presentCompleteSemaphoreHandle) const;

			void Reset();

			const VkSwapchainKHR& GetHandle() const;
			uint32_t GetSwapChainImageCount() const;
			const std::vector<VulkanFrameBufferAttachment*>& GetSwapChainBuffers() const;

		private:
			void Create();
			void Destroy();

			VulkanDevice* mpDevice;

			// Handle to the current swap chain, required for recreation 
			VkSwapchainKHR mHandle;

			// Swapchain image count
			uint32_t mSwapChainImageCount;

			// Swapchain buffers
			std::vector<VulkanFrameBufferAttachment*> mSwapChainBuffers;

		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_SWAPCHAIN_HPP