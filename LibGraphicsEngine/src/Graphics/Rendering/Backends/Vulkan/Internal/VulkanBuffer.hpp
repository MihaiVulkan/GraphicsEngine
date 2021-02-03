#ifndef GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_BUFFER_HPP
#define GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_BUFFER_HPP

#include "Graphics/Rendering/Backends/Vulkan/Common/VulkanObject.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanAllocator.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class VulkanDevice;
		class VulkanQueue;
		class VulkanImage;

		/*
		TO ADD

		vkCmdUpdateBuffer
		vkCmdFillBuffer

		*/


		/* 
			*Wrapper for VkBuffer object*

			Buffers represent linear arrays of data which are used for various purposes by binding them to a graphics or compute pipeline
			via descriptor sets or via certain commands, or by directly specifying them as parameters to certain commands.
		*/
		class VulkanBuffer : public VulkanObject
		{
			GE_RTTI(GraphicsEngine::Graphics::VulkanBuffer)

		public:
			// BufferType is denoted by VkBufferUsageFlags	

			// NOTE! For now we suppose we work only on hardware that supports COHERENT memory via the VK_MEMORY_PROPERTY_HOST_COHERENT_BIT flag
			VulkanBuffer();
			explicit VulkanBuffer(VulkanDevice* pDevice, VkMemoryPropertyFlags memoryPropertyFlags, VkBufferUsageFlags usageFlags, VkDeviceSize size, void* pData = nullptr,
							VkSharingMode sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE, uint32_t queueFamilyIndexCount = 1, const uint32_t * pQueueFamilyIndices = nullptr,
							VkBufferCreateFlags flags = 0);
			virtual ~VulkanBuffer();

			VkResult Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0, VkMemoryMapFlags flags = 0);
			void UnMap();

			void SetData(void* pData, VkDeviceSize size);

			VkResult Bind(VkDeviceSize offset = 0);

			void CopyTo(VulkanBuffer* pDestBuffer, VulkanQueue* pQueue, VkBufferCopy* pCopyRegion = nullptr);
			void CopyTo(VulkanImage* pDestImage, VulkanQueue* pQueue, const std::vector<VkBufferImageCopy>& copyRegions);

			void SetDecriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

			const VkBuffer& GetHandle() const;
			VkDeviceSize GetSize() const;
			void* GetData() const;
			const VkDescriptorBufferInfo& GetDescriptorInfo() const;

		private:
			void Create(const VkBufferCreateInfo& bufferCreateInfo, VkMemoryPropertyFlags memoryPropertyFlags, void* pData);
			void Destroy();

			VulkanDevice* mpDevice;

			VkBuffer mHandle;
			VulkanAllocator::Allocation mAllocation;
			VkDescriptorBufferInfo mDefaultDescriptorInfo;
			VkDeviceSize mSize;
			uint8_t* mpMappedData;
		};
	}
}

#endif // GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_BUFFER_HPP