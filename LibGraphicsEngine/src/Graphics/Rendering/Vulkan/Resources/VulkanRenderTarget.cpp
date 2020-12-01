#include "VulkanRenderTarget.hpp"
#include "Graphics/Rendering/Vulkan/VulkanRenderer.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanFrameBufferAttachment.hpp"
#include "Graphics/Rendering/Resources/RenderTarget.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

GADRRenderTarget::GADRRenderTarget()
	: mpVulkanFrameBufferAttachment(nullptr)
	, mpRenderTarget(nullptr)
{
}

GADRRenderTarget::GADRRenderTarget(Renderer* pRenderer, RenderTarget* pRenderTarget)
	: mpVulkanFrameBufferAttachment(nullptr)
	, mpRenderTarget(pRenderTarget)
{
	Create(pRenderer);
}

GADRRenderTarget::~GADRRenderTarget()
{
	Destroy();
}

void GADRRenderTarget::Create(Renderer* pRenderer)
{
	//TODO
}

void GADRRenderTarget::Destroy()
{
	//TODO
}