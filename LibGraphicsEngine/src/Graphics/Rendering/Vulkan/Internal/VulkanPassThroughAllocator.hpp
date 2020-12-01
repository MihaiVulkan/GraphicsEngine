#ifndef GRAPHICS_RENDERING_VULKAN_PASSTHROUGH_ALLOCATOR_HPP
#define GRAPHICS_RENDERING_VULKAN_PASSTHROUGH_ALLOCATOR_HPP

#include "Foundation/TypeDefs.hpp"
#include "VulkanAllocator.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class VulkanDevice;

		/* Debug Vulkan Allocator
			 Doesn't do anything fancy, just allocates a VkDeviceMemory chunk per Alloc() call
			 which is not what we usually want.
			 We usually want to use the VulkanPoolAllocator class.
		*/
		class VulkanPassThroughAllocator : public VulkanAllocator
		{
		public:
			VulkanPassThroughAllocator();
			explicit VulkanPassThroughAllocator(VulkanDevice* pDevice);
			virtual ~VulkanPassThroughAllocator();

			virtual void Alloc(VulkanAllocator::Allocation& outAllocation, VkMemoryPropertyFlags usage, uint32_t memoryTypeIndex, VkDeviceSize size);
			virtual void Free(const VulkanAllocator::Allocation& allocation);

		private:
		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_PASSTHROUGH_ALLOCATOR_HPP