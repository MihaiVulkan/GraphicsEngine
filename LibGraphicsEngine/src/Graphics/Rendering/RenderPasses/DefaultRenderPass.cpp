#include "DefaultRenderPass.hpp"
#include "Graphics/Rendering/Renderer.hpp"
#include "Graphics/Rendering/RenderQueue.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

DefaultRenderPass::DefaultRenderPass()
{
	mPassType = RenderPass::PassType::GE_PT_STANDARD;
}

DefaultRenderPass::~DefaultRenderPass()
{}

void DefaultRenderPass::Render(Renderer* pRenderer, RenderQueue* pRenderQueue, uint32_t currentBufferIdx)
{
	assert(pRenderer != nullptr);
	assert(pRenderQueue != nullptr);

	// BeginRender

	BindPipeline(pRenderer, currentBufferIdx);

	RenderOccluders(pRenderer, pRenderQueue, currentBufferIdx);
	RenderOpaqueObjects(pRenderer, pRenderQueue, currentBufferIdx);
	RenderTranslucentObjects(pRenderer, pRenderQueue, currentBufferIdx);

	// EndRender
}

void DefaultRenderPass::BindPipeline(Renderer* pRenderer, uint32_t currentBufferIdx)
{
	assert(pRenderer != nullptr);

	pRenderer->BindPipeline(currentBufferIdx);
}

void DefaultRenderPass::RenderOpaqueObjects(Renderer* pRenderer, RenderQueue* pRenderQueue, uint32_t currentBufferIdx)
{
	auto renderableList = pRenderQueue->GetRenderables(RenderQueue::RenderableType::GE_RT_OPAQUE);

	pRenderQueue->ForEach(renderableList,
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

void DefaultRenderPass::Update(Renderer* pRenderer, RenderQueue* pRenderQueue, Camera* pCamera, float32_t deltaTime)
{
	UpdateOpaqueObjects(pRenderer, pRenderQueue, pCamera, deltaTime);
}

void DefaultRenderPass::UpdateOpaqueObjects(Renderer* pRenderer, RenderQueue* pRenderQueue, Camera* pCamera, float32_t deltaTime)
{

	auto renderableList = pRenderQueue->GetRenderables(RenderQueue::RenderableType::GE_RT_OPAQUE);

	pRenderQueue->ForEach(renderableList,
		[&, this](RenderQueue::Renderable* pRenderable)
		{
			assert(pRenderable != nullptr);

			// update uniform buffers
			pRenderer->UpdateUniformBuffers(pRenderable, pCamera);
		}
	);
}