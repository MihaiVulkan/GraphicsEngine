#include "VulkanUniformBuffer.hpp"
#include "Graphics/Rendering/Vulkan/VulkanRenderer.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanCommon.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanDevice.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanBuffer.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanInitializers.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanHelpers.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"
#include "Graphics/Rendering/Resources/UniformBuffer.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

GADRUniformBuffer::GADRUniformBuffer()
	: mpVulkanBuffer(nullptr)
	, mpUniformBuffer(nullptr)
{}

GADRUniformBuffer::GADRUniformBuffer(Renderer* pRenderer, UniformBuffer* pUniformBuffer)
	: mpVulkanBuffer(nullptr)
	, mpUniformBuffer(pUniformBuffer)
{
	Create(pRenderer);
}

GADRUniformBuffer::~GADRUniformBuffer()
{
	Destroy();
}

void GADRUniformBuffer::Create(Renderer* pRenderer)
{
	assert(pRenderer != nullptr);
	assert(mpUniformBuffer != nullptr);
	assert(mpUniformBuffer->GetData() != nullptr);

	// pRenderer must be a pointer to VulkanRenderer otherwise the cast will fail!
	VulkanRenderer* pVulkanRenderer = dynamic_cast<VulkanRenderer*>(pRenderer);
	assert(pVulkanRenderer != nullptr);

	VulkanDevice* pDevice = pVulkanRenderer->GetDevice();
	assert(pDevice != nullptr);

	// uniform buffer
	mpVulkanBuffer = GE_ALLOC(VulkanBuffer)(pDevice, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, static_cast<VkDeviceSize>(mpUniformBuffer->GetSize()));
	assert(mpVulkanBuffer != nullptr);

	// Store information in the uniform's descriptor that is used by the descriptor set
	mpVulkanBuffer->SetDecriptorInfo(static_cast<VkDeviceSize>(mpUniformBuffer->GetSize()));

	UpdateData();
}

void GADRUniformBuffer::Destroy()
{
	if (mpUniformBuffer)
	{
		mpUniformBuffer = nullptr;
	}

	GE_FREE(mpVulkanBuffer);
}

void GADRUniformBuffer::UpdateData(UniformBuffer* pUniformBuffer)
{
	if (pUniformBuffer != nullptr)
	{
		mpUniformBuffer = pUniformBuffer;
	}

	assert(mpVulkanBuffer != nullptr);
	assert(mpUniformBuffer != nullptr);

	VK_CHECK_RESULT(mpVulkanBuffer->Map(mpUniformBuffer->GetSize()));
	mpVulkanBuffer->SetData(mpUniformBuffer->GetData(), static_cast<VkDeviceSize>(mpUniformBuffer->GetSize()));
	mpVulkanBuffer->UnMap();
}

void GADRUniformBuffer::Bind() const
{
	if (mpVulkanBuffer)
	{
		//TODO
	}
}

/// TODO
VulkanBuffer* GADRUniformBuffer::GetVKBuffer() const
{
	return mpVulkanBuffer;
}