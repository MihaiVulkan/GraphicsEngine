#ifndef GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_COMMAND_POOL_HPP
#define GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_COMMAND_POOL_HPP

#include "Graphics/Rendering/Vulkan/Common/VulkanObject.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class VulkanDevice;

		/*
			*Wrapper for VkCommandPool object*

			Command pools are opaque objects that command buffer memory is allocated from, and which allow the implementation to
			amortize the cost of resource creation across multiple command buffers. Command pools are externally synchronized,
			meaning that a command pool must not be used concurrently in multiple threads. That includes use via recording commands
			on any command buffers allocated from the pool, as well as operations that allocate, free, and reset command buffers or the pool itself.

			When a pool is destroyed, all command buffers allocated from the pool are freed.

			Any primary command buffer allocated from another VkCommandPool that is in the recording or executable state and has a secondary command buffer 
			allocated from commandPool recorded into it, becomes invalid.
		*/
		class VulkanCommandPool : public VulkanObject
		{
			GE_RTTI(GraphicsEngine::Graphics::VulkanCommandPool)

		public:
			VulkanCommandPool();
			explicit VulkanCommandPool(VulkanDevice* pDevice, uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = 0);
			virtual ~VulkanCommandPool();

			/*
				Resetting a command pool recycles all of the resources from all of the command buffers allocated from the command pool back 
				to the command pool. All command buffers that have been allocated from the command pool are put in the initial state.

				Any primary command buffer allocated from another VkCommandPool that is in the recording or executable state and has a 
				secondary command buffer allocated from commandPool recorded into it, becomes invalid.

				Unused memory normally arises from command buffers that have been recorded and later reset, such that they are no 
				longer using the memory. On reset, a command buffer can return memory to its command pool, but the only way to release
				memory from a command pool to the system requires calling vkResetCommandPool(), which cannot be executed while any command 
				buffers from that pool are still in use. Subsequent recording operations into command buffers will re-use this memory but 
				since total memory requirements fluctuate over time, unused memory can accumulate.
			*/
			VkResult Reset(VkCommandPoolResetFlags flags);

			/*
				Trimming a command pool recycles unused memory from the command pool back to the system.
				Command buffers allocated from the pool are not affected by the command.

				In most cases trimming will result in a reduction in allocated but unused memory, but it does not guarantee the “ideal” behavior.

				Trimming may be an expensive operation, and should not be called frequently. Trimming should be treated as a way to relieve memory 
				pressure after application-known points when there exists enough unused memory that the cost of trimming is “worth” it.
			*/
			void Trim(VkCommandPoolTrimFlags flags);

			const VkCommandPool& GetHandle() const;
			uint32_t GetQueueFamilyIndex() const;

		private:
			void Create(const VkCommandPoolCreateInfo& commandPoolCreateInfo);
			void Destroy();

			VulkanDevice* mpDevice;

			VkCommandPool mHandle;
			uint32_t mQueueFamilyIndex;
		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_COMMAND_POOL_HPP