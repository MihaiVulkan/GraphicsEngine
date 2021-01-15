#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanRenderFrameBuffer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/VulkanRenderer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanFrameBuffer.hpp"
#include "Graphics/Rendering/Resources/RenderFrameBuffer.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

GADRRenderFrameBuffer::GADRRenderFrameBuffer()
	: mpVulkanFrameBuffer(nullptr)
	, mpRenderFrameBuffer(nullptr)
{}

GADRRenderFrameBuffer::GADRRenderFrameBuffer(Renderer* pRenderer, RenderFrameBuffer* pRenderFrameBuffer)
	: mpVulkanFrameBuffer(nullptr)
	, mpRenderFrameBuffer(pRenderFrameBuffer)
{
	Create(pRenderer);
}

GADRRenderFrameBuffer::~GADRRenderFrameBuffer()
{
	Destroy();
}

void GADRRenderFrameBuffer::Create(Renderer* pRenderer)
{
	//TODO
}

void GADRRenderFrameBuffer::Destroy()
{
	//TODO
}