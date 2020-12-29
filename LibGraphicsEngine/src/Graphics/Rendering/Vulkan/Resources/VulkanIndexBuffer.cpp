#include "VulkanIndexBuffer.hpp"
#include "Graphics/Rendering/Vulkan/VulkanRenderer.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanDevice.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanBuffer.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanInitializers.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

GADRIndexBuffer::GADRIndexBuffer()
	: mpVulkanBuffer(nullptr)
	, mpIndexBuffer(nullptr)
{}

GADRIndexBuffer::GADRIndexBuffer(Renderer* pRenderer, IndexBuffer* pIndexBuffer)
	: mpVulkanBuffer(nullptr)
	, mpIndexBuffer(pIndexBuffer)
{
	Create(pRenderer);
}

GADRIndexBuffer::~GADRIndexBuffer()
{
	Destroy();
}

void GADRIndexBuffer::Create(Renderer* pRenderer)
{
	assert(pRenderer != nullptr);
	assert(mpIndexBuffer != nullptr);
	assert(mpIndexBuffer->GetData() != nullptr);


	// pRenderer must be a pointer to VulkanRenderer otherwise the cast will fail!
	VulkanRenderer* pVulkanRenderer = dynamic_cast<VulkanRenderer*>(pRenderer);
	assert(pVulkanRenderer != nullptr);

	VulkanDevice* pDevice = pVulkanRenderer->GetDevice();
	assert(pDevice != nullptr);

	// Create a host-visible buffer to copy the vertex data to (staging buffer)
	VulkanBuffer* pStagingIndices = GE_ALLOC(VulkanBuffer)
	(
		pDevice,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		mpIndexBuffer->GetSize(), mpIndexBuffer->GetData()
	);
	assert(pStagingIndices != nullptr);

	// Create a device local buffer to which the (host local) vertex data will be copied and which will be used for rendering
	mpVulkanBuffer = GE_ALLOC(VulkanBuffer)
	(pDevice,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		mpIndexBuffer->GetSize()
		);
	assert(mpVulkanBuffer != nullptr);


	// Buffer copies have to be submitted to a queue, so we need a command buffer for them
	// Note: Some devices offer a dedicated transfer queue (with only the transfer bit set) that may be faster when doing lots of copies

	if (pDevice->IsPresentQueueSupported()) // separate present queue
	{
		pStagingIndices->CopyTo(mpVulkanBuffer, pDevice->GetPresentQueue());
	}
	else // graphics and present queue are the same
	{
		pStagingIndices->CopyTo(mpVulkanBuffer, pDevice->GetGraphicsQueue());
	}
	GE_FREE(pStagingIndices);
}

void GADRIndexBuffer::Destroy()
{
	if (mpIndexBuffer)
	{
		mpIndexBuffer = nullptr;
	}

	GE_FREE(mpVulkanBuffer);
}

const Buffer::BufferUsage& GADRIndexBuffer::GetBufferUsage() const
{
	assert(mpIndexBuffer != nullptr);

	return mpIndexBuffer->GetBufferUsage();
}

const IndexBuffer::IndexType& GADRIndexBuffer::GetIndexType() const
{
	assert(mpIndexBuffer != nullptr);

	return mpIndexBuffer->GetIndexType();
}

VulkanBuffer* GADRIndexBuffer::GetVkBuffer() const
{
	return mpVulkanBuffer;
}

VkIndexType GADRIndexBuffer::IndexTypeToVulkanIndexType(const IndexBuffer::IndexType& indexType)
{
	VkIndexType vulkanIndexType = VkIndexType::VK_INDEX_TYPE_MAX_ENUM;

	switch (indexType)
	{
	case IndexBuffer::IndexType::GE_IT_UINT32:
		vulkanIndexType = VkIndexType::VK_INDEX_TYPE_UINT32;
		break;
	case IndexBuffer::IndexType::GE_IT_UINT16:
		vulkanIndexType = VkIndexType::VK_INDEX_TYPE_UINT16;
		break;
	case IndexBuffer::IndexType::GE_IT_UINT8:
		vulkanIndexType = VkIndexType::VK_INDEX_TYPE_UINT8_EXT; //TOOD - check for extension support - VkPhysicalDeviceIndexTypeUint8FeaturesEXT 
		break;
	case IndexBuffer::IndexType::GE_IT_COUNT:
	default:
		LOG_ERROR("Invalid Vulkan Index Buffer Index Type!");
	}

	return vulkanIndexType;
}