#ifndef GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_POOL_ALLOCATOR_HPP
#define GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_POOL_ALLOCATOR_HPP

/* Implementation of VUlkanPoolAllcoator based on:
   http://kylehalladay.com/blog/tutorial/2017/12/13/Custom-Allocators-Vulkan.html
   https://github.com/khalladay/VkMaterialSystem/blob/material-instances/VkMaterialSystem/vkh_allocator_pool.cpp
*/

#include "VulkanAllocator.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class VulkanDevice;

		/* Usual Vulkan Allocator
			First pools of memory are creataed for each supported memory type.
			Basically allocates big chunks of VkDeviceMemory and then each Alloc() call
			is satisfied by searching for free memory in tht big chunk, if none is found
			a new big chunk is allocated/added.
		*/
		class VulkanPoolAllocator : public VulkanAllocator
		{
			GE_RTTI(GraphicsEngine::Graphics::VulkanPoolAllocator)

		public:
			VulkanPoolAllocator();
			explicit VulkanPoolAllocator(VulkanDevice* pDevice);
			virtual ~VulkanPoolAllocator();

			virtual void Alloc(VkMemoryPropertyFlags usage, uint32_t memoryTypeIndex, VkDeviceSize size, VulkanAllocator::Allocation& outAllocation) override;
			virtual void Free(const VulkanAllocator::Allocation& allocation) override;

		private:
			// used to indentify memory block layout
			struct OffsetSize { uint64_t offset; uint64_t size; };

			struct BlockSpanIndexPair { uint32_t blockIdx; uint32_t spanIdx; };

			// each memory block
			struct DeviceMemoryBlock
			{
				// allocation unit
				VulkanAllocator::Allocation mem;
				// a vector of lyaout info - how many subdivisions each with: size and offset
				std::vector<OffsetSize> layout;
				bool_t pageReserved;
			};

			// each memory pool has a vector of memory blocks
			struct MemoryPool
			{
				std::vector<DeviceMemoryBlock> blocks;
			};

			// adds a new memory block an existing memory pool
			uint32_t AddBlockToPool(VkDeviceSize size, uint32_t memoryTypeIndex, bool_t fitToAlloc);

			void MarkChunkOfMemoryBlockUsed(uint32_t memoryTypeIndex, const BlockSpanIndexPair& indices, VkDeviceSize size);

			bool_t FindFreeChunkForAllocation(uint32_t memoryTypeIndex, VkDeviceSize size, bool_t needsWholePage, BlockSpanIndexPair& outIndexPair);

			// each allocation unit (vkDeviceMmeory) contains several subdivisions/pages
			// in our implementation

			// page size of each subdivion of an allocation unit
			uint64_t mPageSize;
			// allocation unit/mmeory block min size - should a multiple of page size
			VkDeviceSize mMemoryBlockMinSize;

			// a vector of memory pools: a pool for each memory type
			std::vector<MemoryPool> mMemPools;
		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_POOL_ALLOCATOR_HPP