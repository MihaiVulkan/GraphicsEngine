#include "RenderQueue.hpp"
#include "Graphics/Camera/Camera.hpp"
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

void RenderQueue::Push(Node* pNode)
{
	assert(pNode != nullptr);

	if (pNode->GetClassName_() == "GraphicsEngine::Graphics::GeometryNode")
	{

		// TODO - improve this - insert based on renderableType

		GeometryNode* pGeoNode = dynamic_cast<GeometryNode*>(pNode);
		assert(pGeoNode != nullptr);

		Renderable renderable{};
		renderable.pGeometryNode = pGeoNode;
		renderable.pMaterial = pGeoNode->GetComponent<MaterialComponent>()->GetMaterial();

		RenderableList queue;
		queue.push_back(renderable);

		mRenderables[RenderQueue::RenderableType::RT_OPAQUE] = queue;
	}
	else
	{
		// other cases
	}
}

//TODO push light object

RenderQueue::RenderableList* RenderQueue::GetRenderables(const RenderQueue::RenderableType& type)
{
	assert((RenderableType::RT_BACKGROUND <= type) && (type < RenderableType::RT_COUNT));
	assert(mRenderables.size() > 0);

	return &mRenderables.at(type);
}

void RenderQueue::Each(RenderQueue::RenderableList* pRenderableList, std::function< void(RenderQueue::Renderable*) > callback)
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