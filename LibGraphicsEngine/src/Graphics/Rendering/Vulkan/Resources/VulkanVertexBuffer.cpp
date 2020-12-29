#include "VulkanVertexBuffer.hpp"
#include "Graphics/Rendering/Vulkan/Common/VulkanCommon.hpp"
#include "Graphics/Rendering/Vulkan/VulkanRenderer.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanDevice.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanBuffer.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanInitializers.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

GADRVertexBuffer::GADRVertexBuffer()
	: mpVulkanBuffer(nullptr)
	, mpVertexBuffer(nullptr)
	, mInputBinding{}
{}

GADRVertexBuffer::GADRVertexBuffer(Renderer* pRenderer, VertexBuffer* pVertexBuffer)
	: mpVulkanBuffer(nullptr)
	, mpVertexBuffer(pVertexBuffer)
	, mInputBinding{}
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

	VkVertexInputRate vulkanInputRate = GADRVertexBuffer::InputRateToVulkanInputRate(mpVertexBuffer->GetInputRate());

	auto format = mpVertexBuffer->GetFormat();
	assert(format != nullptr);

	mInputBinding.binding = VERTEX_BUFFER_BIND_ID;
	mInputBinding.stride = format->GetVertexTotalStride();
	mInputBinding.inputRate = vulkanInputRate;

	// pRenderer must be a pointer to VulkanRenderer otherwise the cast will fail!
	VulkanRenderer* pVulkanRenderer = dynamic_cast<VulkanRenderer*>(pRenderer);
	assert(pVulkanRenderer != nullptr);

	VulkanDevice* pDevice = pVulkanRenderer->GetDevice();
	assert(pDevice != nullptr);


	// Create a host-visible buffer to copy the vertex data to (staging buffer)
	VulkanBuffer* pStagingVertices = GE_ALLOC(VulkanBuffer)
	(
		pDevice,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		mpVertexBuffer->GetSize(), mpVertexBuffer->GetData()
	);

	assert(pStagingVertices != nullptr);

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
	// Note: Some devices offer a dedicated transfer queue (with only the transfer bit set) that may be faster when doing lots of copies

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
	if (mpVertexBuffer)
	{
		mpVertexBuffer = nullptr;
	}

	GE_FREE(mpVulkanBuffer);
}


VulkanBuffer* GADRVertexBuffer::GetVkBuffer() const
{
	return mpVulkanBuffer;
}

const VkVertexInputBindingDescription& GADRVertexBuffer::GetVkInputBinding() const
{
	return mInputBinding;
}

const Buffer::BufferUsage& GADRVertexBuffer::GetBufferUsage() const
{
	assert(mpVertexBuffer != nullptr);

	return mpVertexBuffer->GetBufferUsage();
}

const VertexBuffer::InputRate& GADRVertexBuffer::GetInputRate() const
{
	assert(mpVertexBuffer != nullptr);

	return mpVertexBuffer->GetInputRate();
}

VkVertexInputRate GADRVertexBuffer::InputRateToVulkanInputRate(const VertexBuffer::InputRate& inputRate)
{
	VkVertexInputRate vulkanInputRate = VkVertexInputRate::VK_VERTEX_INPUT_RATE_MAX_ENUM;

	switch (inputRate)
	{
	case VertexBuffer::InputRate::GE_IR_VERTEX:
		vulkanInputRate = VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;
		break;
	case VertexBuffer::InputRate::GE_IR_INSTANCE:
		vulkanInputRate = VkVertexInputRate::VK_VERTEX_INPUT_RATE_INSTANCE;
		break;
	case VertexBuffer::InputRate::GE_IR_COUNT:
	default:
		LOG_ERROR("Invalid Vulkan Vertex Buffer Input Rate!");
	}

	return vulkanInputRate;
}