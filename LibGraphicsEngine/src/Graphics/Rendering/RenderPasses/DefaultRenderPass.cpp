#include "DefaultRenderPass.hpp"
#include "Graphics/Rendering/Renderer.hpp"
#include "Graphics/Rendering/RenderQueue.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

DefaultRenderPass::DefaultRenderPass()
{}

DefaultRenderPass::~DefaultRenderPass()
{}

void DefaultRenderPass::Render(Renderer* pRenderer, RenderQueue* pRenderQueue, uint32_t currentBufferIdx)
{
	assert(pRenderer != nullptr);
	assert(pRenderQueue != nullptr);

	// BeginRender

	RenderOccluders(pRenderer, pRenderQueue, currentBufferIdx);
	RenderOpaqueObjects(pRenderer, pRenderQueue, currentBufferIdx);
	RenderTranslucentObjects(pRenderer, pRenderQueue, currentBufferIdx);

	// EndRender
}

void DefaultRenderPass::RenderOpaqueObjects(Renderer* pRenderer, RenderQueue* pRenderQueue, uint32_t currentBufferIdx)
{
	auto renderableList = pRenderQueue->GetRenderables(RenderQueue::RenderableType::RT_OPAQUE);

	pRenderQueue->Each(renderableList,
		[&, this](RenderQueue::Renderable* pRenderable)
		{
			assert(pRenderable != nullptr);

			pRenderer->DrawObject(pRenderable, currentBufferIdx);
		}
	);
}

void DefaultRenderPass::RenderTranslucentObjects(Renderer* pRenderer, RenderQueue* pRenderQueue, uint32_t currentBufferIdx)
{
	//TODO
}

void DefaultRenderPass::RenderOccluders(Renderer* pRenderer, RenderQueue* pRenderQueue, uint32_t currentBufferIdx)
{
	//TODO
}

void DefaultRenderPass::Update(Renderer* pRenderer, RenderQueue* pRenderQueue, Camera* pCamera, bfloat32_t deltaTime)
{
	UpdateOpaqueObjects(pRenderer, pRenderQueue, pCamera, deltaTime);
}

void DefaultRenderPass::UpdateOpaqueObjects(Renderer* pRenderer, RenderQueue* pRenderQueue, Camera* pCamera, bfloat32_t deltaTime)
{

	auto renderableList = pRenderQueue->GetRenderables(RenderQueue::RenderableType::RT_OPAQUE);

	pRenderQueue->Each(renderableList,
		[&, this](RenderQueue::Renderable* pRenderable)
		{
			assert(pRenderable != nullptr);

			// update uniform buffers
			pRenderer->UpdateUniformBuffers(pRenderable, pCamera);
		}
	);
}