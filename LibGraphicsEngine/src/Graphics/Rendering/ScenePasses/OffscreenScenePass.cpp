#include "Graphics/Rendering/ScenePasses/OffscreenScenePass.hpp"
#include "Graphics/Rendering/Renderer.hpp"
#include "Graphics/Rendering/RenderQueue.hpp"
#include "Graphics/SceneGraph/GeometryNode.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

OffscreenScenePass::OffscreenScenePass()
	: ScenePass(ScenePass::PassType::GE_PT_OFFSCREEN)
{}

OffscreenScenePass::~OffscreenScenePass()
{}

void OffscreenScenePass::Render(Renderer* pRenderer, RenderQueue* pRenderQueue, uint32_t currentBufferIdx,
								ScenePass::OnBeginFN OnBegin, ScenePass::OnEndFN OnEnd)
{
	assert(pRenderer != nullptr);
	assert(pRenderQueue != nullptr);

	// BeginRender
	OnBegin(this, currentBufferIdx);

	// TODO 1 - render occluders

	// 2 - render opaque objects
	RenderOpaqueObjects(pRenderer, pRenderQueue, currentBufferIdx);

	// TODO 3 - render translucent objects

	OnEnd(this, currentBufferIdx);
	// EndRender
}

void OffscreenScenePass::RenderOpaqueObjects(Renderer* pRenderer, RenderQueue* pRenderQueue, uint32_t currentBufferIdx)
{
	auto renderableList = pRenderQueue->GetRenderables(RenderQueue::RenderableType::GE_RT_OPAQUE);

	pRenderQueue->ForEach(renderableList,
		[&, this](const RenderQueue::Renderable* pRenderable)
		{
			if (pRenderable->pGeometryNode->IsPassAllowed(mPassType))
			{
				pRenderer->DrawObject(this, pRenderable, currentBufferIdx);
			}
		}
	);
}

void OffscreenScenePass::Update(Renderer* pRenderer, RenderQueue* pRenderQueue, Camera* pCamera, float32_t crrTime)
{
	UpdateOpaqueObjects(pRenderer, pRenderQueue, pCamera, crrTime);
}

void OffscreenScenePass::UpdateOpaqueObjects(Renderer* pRenderer, RenderQueue* pRenderQueue, Camera* pCamera, float32_t crrTime)
{

	auto renderableList = pRenderQueue->GetRenderables(RenderQueue::RenderableType::GE_RT_OPAQUE);

	pRenderQueue->ForEach(renderableList,
		[&, this](const RenderQueue::Renderable* pRenderable)
		{
			if (pRenderable->pGeometryNode->IsPassAllowed(mPassType))
			{
				// update uniform buffers
				pRenderer->UpdateUniformBuffers(this, pRenderable, pCamera, crrTime);
			}
		}
	);
}