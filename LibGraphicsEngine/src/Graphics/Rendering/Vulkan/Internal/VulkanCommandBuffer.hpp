#ifndef GRAPHICS_RENDERING_VULKAN_COMMAND_BUFFER_HPP
#define GRAPHICS_RENDERING_VULKAN_COMMAND_BUFFER_HPP

#include "Foundation/TypeDefs.hpp"
#include "vulkan/vulkan.h"
#include <vector>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class VulkanDevice;
		class VulkanQueue;

		/*
			*Wrapper for VkCommandBuffer object*

			Command buffers are objects used to record commands which can be subsequently submitted to a device queue for execution.
			There are two levels of command buffers - primary command buffers, which can execute secondary command buffers, and which 
			are submitted to queues, and secondary command buffers, which can be executed by primary command buffers, and which are not
			directly submitted to queues.

			Each command buffer is always in one of the following states:

			*Initial - When a command buffer is allocated, it is in the initial state. Some commands are able to reset a command buffer
			(or a set of command buffers) back to this state from any of the executable, recording or invalid state. Command buffers in 
			the initial state can only be moved to the recording state, or freed.

			*Recording - vkBeginCommandBuffer() changes the state of a command buffer from the initial state to the recording state.
			Once a command buffer is in the recording state, vkCmd* commands can be used to record to the command buffer.

			*Executable - vkEndCommandBuffer() ends the recording of a command buffer, and moves it from the recording state to the executable state.
			Executable command buffers can be submitted, reset, or recorded to another command buffer.

			*Pending - Queue submission of a command buffer changes the state of a command buffer from the executable state to the pending state. 
			Whilst in the pending state, applications must not attempt to modify the command buffer in any way - as the device may be processing 
			the commands recorded to it. Once execution of a command buffer completes, the command buffer either reverts back to the executable state,
			or if it was recorded with VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, it moves to the invalid state. A synchronization command should be 
			used to detect when this occurs.

			*Invalid - Some operations, such as modifying or deleting a resource that was used in a command recorded to a command buffer, will transition 
			the state of that command buffer into the invalid state. Command buffers in the invalid state can only be reset or freed.

			A command buffer can also be flushed or reset.

			When any primary command buffer that is in the recording or executable state is freed, it then becomes invalid.
		*/
		class VulkanCommandBuffer
		{
		public:
			VulkanCommandBuffer();
			explicit VulkanCommandBuffer(VulkanDevice* pDevice, VkCommandBufferLevel level, bool_t begin = false);
			virtual ~VulkanCommandBuffer();

			VkResult Begin(const VkCommandBufferInheritanceInfo* pInheritanceInfo = nullptr);
			VkResult End();

			void Flush(VulkanQueue* pQueue, bool_t free = true);

			VkResult Reset(VkCommandBufferResetFlags flags = 0);

			void ExecuteSecondaryCommandBuffers(const std::vector<VulkanCommandBuffer*>& secondaryCommandBuffers);

			const VkCommandBuffer& GetHandle() const;

		private:
			void Create(VkCommandBufferLevel level, bool_t begin);
			void Destroy();

			VulkanDevice* mpDevice;

			VkCommandBuffer mHandle;
		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_COMMAND_BUFFER_HPP