#ifndef GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_PASS_THROUGH_ALLOCATOR_HPP
#define GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_PASS_THROUGH_ALLOCATOR_HPP

#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanAllocator.hpp"

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
			GE_RTTI(GraphicsEngine::Graphics::VulkanPassThroughAllocator)

		public:
			VulkanPassThroughAllocator();
			explicit VulkanPassThroughAllocator(VulkanDevice* pDevice);
			virtual ~VulkanPassThroughAllocator();

			virtual void Alloc(VkMemoryPropertyFlags usage, uint32_t memoryTypeIndex, VkDeviceSize size, VulkanAllocator::Allocation& outAllocation) override;
			virtual void Free(const VulkanAllocator::Allocation& allocation) override;

		private:
		};
	}
}

#endif // GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_PASS_THROUGH_ALLOCATOR_HPP