#ifndef GRAPHICS_RENDERING_VULKAN_FENCE_HPP
#define GRAPHICS_RENDERING_VULKAN_FENCE_HPP

#include "Foundation/TypeDefs.hpp"
#include "vulkan/vulkan.h"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class VulkanDevice;

		/*
			*Wrapper for VkFence object*
		
			Fences are a synchronization primitive that can be used to insert a dependency from a queue to the host.
			Fences have two states - signaled and unsignaled. A fence can be signaled as part of the execution of a 
			queue submission command. Fences can be unsignaled on the host with vkResetFences. Fences can be waited 
			on by the host with the vkWaitForFences command, and the current state can be queried with vkGetFenceStatus.

			As with most objects in Vulkan, fences are an interface to internal data which is typically opaque to applications.
			This internal data is referred to as a fence’s payload.

			However, in order to enable communication with agents outside of the current device, it is necessary to be able to 
			export that payload to a commonly understood format, and subsequently import from that format as well.

			The internal data of a fence may include a reference to any resources and pending work associated with signal or 
			unsignal operations performed on that fence object. Mechanisms to import and export that internal data to and from
			fences are provided below. These mechanisms indirectly enable applications to share fence state between two or 
			more fences and other synchronization primitives across process and API boundaries.
		*/
		class VulkanFence
		{
		public:
			VulkanFence();
			explicit VulkanFence(VulkanDevice* pDevice, VkFenceCreateFlags flags = 0);
			virtual ~VulkanFence();

			VkResult Reset();
			VkResult WaitIdle(VkBool32 waitAll, uint64_t timeout);
			VkResult GetStatus() const;

			const VkFence& GetHandle() const;

		private:
			void Create(VkFenceCreateFlags flags);
			void Destroy();

			VulkanDevice* mpDevice;

			VkFence mHandle;
		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_FENCE_HPP