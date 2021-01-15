#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanIndexBuffer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Common/VulkanUtils.hpp"
#include "Graphics/Rendering/Backends/Vulkan/VulkanRenderer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanDevice.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanCommandBuffer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanBuffer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanInitializers.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

GADRIndexBuffer::GADRIndexBuffer()
	: mpVulkanRenderer(nullptr)
	, mpVulkanBuffer(nullptr)
	, mpIndexBuffer(nullptr)
{}

GADRIndexBuffer::GADRIndexBuffer(Renderer* pRenderer, IndexBuffer* pIndexBuffer)
	: mpVulkanRenderer(nullptr)
	, mpVulkanBuffer(nullptr)
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
	mpVulkanRenderer = dynamic_cast<VulkanRenderer*>(pRenderer);
	assert(mpVulkanRenderer != nullptr);

	VulkanDevice* pDevice = mpVulkanRenderer->GetDevice();
	assert(pDevice != nullptr);

	// To copy data from host (CPU) to device (GPU) we use staging buffers
	// host local buffer is efficient for host, device local buffer is efficient for device

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

void GADRIndexBuffer::OnBind(uint32_t currentBufferIdx)
{
	auto cmdBuff = mpVulkanRenderer->GetCommandBuffer(currentBufferIdx);
	assert(cmdBuff != nullptr);

	VkIndexType vkIndexType = VulkanUtils::IndexTypeToVulkanIndexType(mpIndexBuffer->GetIndexType());
	vkCmdBindIndexBuffer(cmdBuff->GetHandle(), mpVulkanBuffer->GetHandle(), 0, vkIndexType);
}

void GADRIndexBuffer::OnUnBind(uint32_t currentBufferIdx)
{

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