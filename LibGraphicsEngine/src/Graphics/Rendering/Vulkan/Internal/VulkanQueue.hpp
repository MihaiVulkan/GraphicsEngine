#ifndef GRAPHICS_RENDERING_VULKAN_QUEUE_HPP
#define GRAPHICS_RENDERING_VULKAN_QUEUE_HPP

#include "Foundation/TypeDefs.hpp"
#include "vulkan/vulkan.h"

namespace GraphicsEngine
{
	namespace Graphics
	{
		/*
			*Wrapper for VkQueue object*

			Creating a logical device also creates the queues associated with that device. 
			When the logical device is destroyed the queues associated with it are destroyed as well.

			There are several types of queues: graphics, compute, transfer and sparse memory.

			*Graphics and compute queues both have transfer/present capabilities.
			We also have a separate transfer/present queue in case we want to do the present in paralel
			to avoid blocking the graphics/compute queue.

			Each queue is part of a queue family ahich is identified by an index.
			Each has a index within her queue family.

			Queues can have different priorities.Within the same device, queues with higher priority
			may be allotted more processing time than queues with lower priority.

			Operations on queue: submit work, wait, present to swapchain.
			Usually work is submited to queue via command buffer objects.

		*/
		class VulkanQueue
		{
		public:
			VulkanQueue();
			explicit VulkanQueue(VkQueueFlags typeFlags, uint32_t familyIndex, uint32_t index, bfloat32_t priority, VkDeviceQueueCreateFlags flags = 0);
			virtual ~VulkanQueue();

			// we have to init the queue after the VkDevice has been created!
			void Init(VkDevice deviceHandle);

			VkResult WaitIdle() const;
			VkResult Submit(uint32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fenceHandle) const;
			VkResult Present(const VkPresentInfoKHR& presentInfo) const;

			const VkQueue& GetHandle() const;
			VkQueueFlags GetTypeFlags() const;
			uint32_t GetFamilyIndex() const;
			uint32_t GetIndex() const;
			float GetPriority() const;
			const VkDeviceQueueCreateInfo& GetCreateInfo() const;

		private:

			VkQueue mHandle;
			VkQueueFlags mTypeFlags;
			uint32_t mFamilyIndex;
			uint32_t mIndex;
			bfloat32_t mPriority;
			VkDeviceQueueCreateInfo mCreateInfo;
		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_QUEUE_HPP