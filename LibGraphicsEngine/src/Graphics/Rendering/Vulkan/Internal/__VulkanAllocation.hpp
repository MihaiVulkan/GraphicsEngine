#ifndef VULKAN_ALLOCATION_HPP
#define VULKAN_ALLOCATION_HPP

#include "Foundation/TypeDefs.hpp"
#include "vulkan/vulkan.h"


class VulkanAllocation
{
public:
	VulkanAllocation();
	VulkanAllocation(VkDeviceMemory handle, uint32_t typeIndex, uint32_t id, VkDeviceSize size, VkDeviceSize offset);
	virtual ~VulkanAllocation();

	const VkDeviceMemory& GetHandle() const;
	uint32_t GetTypeIndex() const;
	uint32_t GetId() const;
	VkDeviceSize GetSize() const;
	VkDeviceSize GetOffset() const;

	void SetHandle(VkDeviceMemory handle);
	void SetTypeIndex(uint32_t typeIndex);
	void SetId(uint32_t id);
	void SetSize(VkDeviceSize size);
	void SetOffset(VkDeviceSize offset);

private:

	VkDeviceMemory mHandle;
	uint32_t mTypeIndex;
	uint32_t mId;
	VkDeviceSize mSize;
	VkDeviceSize mOffset;
};

#endif // VULKAN_ALLOCATION_HPP