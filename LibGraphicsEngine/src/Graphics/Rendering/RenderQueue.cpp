#include "Graphics/Rendering/RenderQueue.hpp"
#include "Graphics/Cameras/Camera.hpp"
#include "Graphics/SceneGraph/GeometryNode.hpp"
#include "Graphics/Rendering/Resources/Material.hpp"
#include "Graphics/Components/MaterialComponent.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

RenderQueue::RenderQueue()
	: mpCamera(nullptr)
{}

RenderQueue::~RenderQueue()
{
	Destroy();
}

void RenderQueue::Destroy()
{
	for (auto& it : mRenderables)
	{
		it.second.clear();
	}
	mRenderables.clear();

	if (mpCamera)
	{
		mpCamera = nullptr;
	}
}

void RenderQueue::Push(GeometryNode* pGeoNode)
{
	assert(pGeoNode != nullptr);

	// TODO - improve this - insert based on renderableType, for now we use only opaque type
	MaterialComponent* pMatComp = pGeoNode->GetComponent<MaterialComponent>();

	Renderable renderable{};
	renderable.pGeometryNode = pGeoNode;

	if (pMatComp)
	{
		Material* pMaterial = pMatComp->GetMaterial();
		assert(pMaterial != nullptr);

		renderable.pMaterial = pMaterial;
	}

	RenderableList queue;
	queue.push_back(renderable);

	mRenderables[RenderQueue::RenderableType::GE_RT_OPAQUE] = queue;
}

//TODO push light object

RenderQueue::RenderableList* RenderQueue::GetRenderables(const RenderQueue::RenderableType& type)
{
	assert((RenderableType::GE_RT_BACKGROUND <= type) && (type < RenderableType::GE_RT_COUNT));
	assert(mRenderables.size() > 0);

	return &mRenderables.at(type);
}

void RenderQueue::ForEach(RenderQueue::RenderableList* pRenderableList, std::function< void(RenderQueue::Renderable*) > callback)
{
	assert(pRenderableList != nullptr);
	assert(pRenderableList->empty() != true);

	for (auto& renderable : *pRenderableList)
	{
		callback(&renderable);
	}
}


Camera* RenderQueue::GetCamera() const
{
	return mpCamera;
}

void RenderQueue::SetCamera(Camera* pCamera)
{
	assert(pCamera != nullptr);

	mpCamera = pCamera;
}