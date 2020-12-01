#ifndef GRAPHICS_RENDERING_VULKAN_SEMAPHORE_HPP
#define GRAPHICS_RENDERING_VULKAN_SEMAPHORE_HPP

#include "Foundation/TypeDefs.hpp"
#include "vulkan/vulkan.h"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class VulkanDevice;

		/*
			*Wrapper for VkSemaphore object*

			Semaphores are a synchronization primitive that can be used to insert a dependency between queue operations 
			or between a queue operation and the host. Binary semaphores have two states - signaled and unsignaled. 
			Timeline semaphores have a monotonically increasing 64-bit unsigned integer payload and are signaled with
			respect to a particular reference value. A semaphore can be signaled after execution of a queue operation 
			is completed, and a queue operation can wait for a semaphore to become signaled before it begins execution. 
			A timeline semaphore can additionally be signaled from the host with the vkSignalSemaphore command and waited 
			on from the host with the vkWaitSemaphores command.

			As with most objects in Vulkan, semaphores are an interface to internal data which is typically opaque to applications.
			This internal data is referred to as a semaphore’s payload.

			However, in order to enable communication with agents outside of the current device, it is necessary to be able to export 
			that payload to a commonly understood format, and subsequently import from that format as well.

			The internal data of a semaphore may include a reference to any resources and pending work associated with signal or unsignal 
			operations performed on that semaphore object. Mechanisms to import and export that internal data to and from semaphores are 
			provided below. These mechanisms indirectly enable applications to share semaphore state between two or more semaphores and 
			other synchronization primitives across process and API boundaries.
		*/
		class VulkanSemaphore
		{
		public:
			VulkanSemaphore();
			explicit VulkanSemaphore(VulkanDevice* pDevice, VkSemaphoreCreateFlags flags = 0);
			virtual ~VulkanSemaphore();

			const VkSemaphore& GetHandle() const;

		private:
			void Create(VkSemaphoreCreateFlags flags);
			void Destroy();

			VulkanDevice* mpDevice;

			VkSemaphore mHandle;
		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_SEMAPHORE_HPP