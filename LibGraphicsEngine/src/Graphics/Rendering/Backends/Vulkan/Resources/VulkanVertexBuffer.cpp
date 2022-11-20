#if defined(VULKAN_RENDERER)
#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanVertexBuffer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Common/VulkanCommon.hpp"
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

GADRVertexBuffer::GADRVertexBuffer()
	: mpVulkanRenderer(nullptr)
	, mpVulkanBuffer(nullptr)
	, mpVertexBuffer(nullptr)
{}

GADRVertexBuffer::GADRVertexBuffer(Renderer* pRenderer, VertexBuffer* pVertexBuffer)
	: mpVulkanRenderer(nullptr)
	, mpVulkanBuffer(nullptr)
	, mpVertexBuffer(pVertexBuffer)
{
	Create(pRenderer);
}

GADRVertexBuffer::~GADRVertexBuffer()
{
	Destroy();
}

void GADRVertexBuffer::Create(Renderer* pRenderer)
{
	assert(pRenderer != nullptr);
	assert(mpVertexBuffer != nullptr);
	assert(mpVertexBuffer->GetData() != nullptr);

	// pRenderer must be a pointer to VulkanRenderer otherwise the cast will fail!
	mpVulkanRenderer = dynamic_cast<VulkanRenderer*>(pRenderer);
	assert(mpVulkanRenderer != nullptr);

	VulkanDevice* pDevice = mpVulkanRenderer->GetDevice();
	assert(pDevice != nullptr);

	// To copy data from host (CPU) to device (GPU) we use staging buffers
	// host local buffer is efficient for host, device local buffer is efficient for device

	// Create a host-visible buffer to copy the vertex data to (staging buffer)
	VulkanBuffer* pStagingVertices = GE_ALLOC(VulkanBuffer)
	(
		pDevice,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		mpVertexBuffer->GetSize(), mpVertexBuffer->GetData()
	);

	assert(pStagingVertices != nullptr);

	//TODO - use Buffer::BufferUsage 

	// Create a device local buffer to which the (host local) vertex data will be copied and which will be used for rendering
	mpVulkanBuffer = GE_ALLOC(VulkanBuffer)
	(
		pDevice,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		mpVertexBuffer->GetSize()
	);
	assert(mpVulkanBuffer != nullptr);


	// Buffer copies have to be submitted to a queue, so we need a command buffer for them
	// Note: Some devices offer a dedicated transfer/present queue (with only the transfer bit set) that may be faster when doing lots of copies

	if (pDevice->IsPresentQueueSupported()) // separate present queue
	{
		pStagingVertices->CopyTo(mpVulkanBuffer, pDevice->GetPresentQueue());
	}
	else // graphics and present queue are the same
	{
		pStagingVertices->CopyTo(mpVulkanBuffer, pDevice->GetGraphicsQueue());
	}
	GE_FREE(pStagingVertices);
}

void GADRVertexBuffer::Destroy()
{
	if (mpVulkanRenderer)
	{
		mpVulkanRenderer = nullptr;
	}

	if (mpVertexBuffer)
	{
		mpVertexBuffer = nullptr;
	}

	GE_FREE(mpVulkanBuffer);
}

void GADRVertexBuffer::OnBind(uint32_t currentBufferIdx)
{
	auto cmdBuff = mpVulkanRenderer->GetCommandBuffer(currentBufferIdx);
	assert(cmdBuff != nullptr);

	VkDeviceSize offsets[1] = { 0 };
	vkCmdBindVertexBuffers(cmdBuff->GetHandle(), VERTEX_BUFFER_BIND_ID, 1, &(mpVulkanBuffer->GetHandle()), offsets);
}

void GADRVertexBuffer::OnUnBind(uint32_t currentBufferIdx)
{
	//
}

const Buffer::BufferUsage& GADRVertexBuffer::GetBufferUsage() const
{
	assert(mpVertexBuffer != nullptr);

	return mpVertexBuffer->GetBufferUsage();
}
#endif // VULKAN_RENDERER