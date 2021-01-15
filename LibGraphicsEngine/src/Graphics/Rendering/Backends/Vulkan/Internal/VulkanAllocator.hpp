#ifndef GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_ALLOCATOR_HPP
#define GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_ALLOCATOR_HPP

#include "Graphics/Rendering/Backends/Vulkan/Common/VulkanObject.hpp"
#include <vector>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class VulkanDevice;

		/* Vulkan Allocator base abstract class
		   Check VulkanPoolAllocator for more deltails
		*/
		class VulkanAllocator : public VulkanObject
		{
			GE_RTTI(GraphicsEngine::Graphics::VulkanAllocator)

		public:
			struct Allocation
			{
				VkDeviceMemory handle;
				uint32_t typeIndex;
				uint32_t id;
				VkDeviceSize size;
				VkDeviceSize offset;
			};

			virtual ~VulkanAllocator();

			virtual void Alloc(VkMemoryPropertyFlags usage, uint32_t memoryTypeIndex, VkDeviceSize size, VulkanAllocator::Allocation& outAllocation) = 0;
			virtual void Free(const VulkanAllocator::Allocation& allocation) = 0;

			uint32_t GetTotalMemorySize() const;
			uint32_t GetTotalMemoryUsed() const;

			const std::vector<uint32_t>& GetMemoryTypeAllocationSizes() const;
			uint32_t GetAllocatedSize(uint32_t memoryType) const;
			uint32_t GetAllocationCount() const;

		protected:
			// abstract class - constructor to be called only by the derived class
			VulkanAllocator();
			explicit VulkanAllocator(VulkanDevice* pDevice);


			VulkanDevice* mpDevice;

			uint32_t mTotalMemorySize;
			uint32_t mTotalMemoryUsed;

			std::vector<uint32_t> mMemoryTypeAllocationSizes;
			uint32_t mAllocationCount; // count of Alloc()/Free() pairs
		};
	}
}

#endif // GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_ALLOCATOR_HPP