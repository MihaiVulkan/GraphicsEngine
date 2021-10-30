#include "Graphics/Rendering/ScenePasses/CompositeScenePass.hpp"
#include "Graphics/Rendering/Renderer.hpp"
#include "Graphics/Rendering/RenderQueue.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

CompositeScenePass::CompositeScenePass()
	: ScenePass(ScenePass::PassType::GE_PT_COMPOSITE)
{}

CompositeScenePass::~CompositeScenePass()
{
	for (auto it = mPassMap.begin(); it != mPassMap.end(); ++it)
	{
		GE_FREE(it->second);
	}
	mPassMap.clear();
}

void CompositeScenePass::AddPass(ScenePass* pPass)
{
	assert(pPass != nullptr);

	// pass data to child pass before adding it to the map
	pPass->Data() = mData;

	mPassMap[pPass->GetPassType()] = pPass;
}

void CompositeScenePass::Render(Renderer* pRenderer, RenderQueue* pRenderQueue, uint32_t currentBufferIdx,
								ScenePass::OnBeginFN OnBegin, ScenePass::OnEndFN OnEnd)
{
	assert(pRenderer != nullptr);
	assert(pRenderQueue != nullptr);

	// BeginRender

	ScenePassMap::const_iterator cIt;
	for (cIt = mPassMap.begin(); cIt != mPassMap.end(); ++cIt)
	{
		if (cIt->second)
		{
			cIt->second->Render(pRenderer, pRenderQueue, currentBufferIdx, OnBegin, OnEnd);
		}
	}

	// EndRender
}

void CompositeScenePass::Update(Renderer* pRenderer, RenderQueue* pRenderQueue, Camera* pCamera, float32_t crrTime)
{
	ScenePassMap::const_iterator cIt;
	for (cIt = mPassMap.begin(); cIt != mPassMap.end(); ++cIt)
	{
		if (cIt->second)
		{
			cIt->second->Update(pRenderer, pRenderQueue, pCamera, crrTime);
		}
	}
}