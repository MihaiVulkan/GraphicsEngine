#include "Graphics/SceneGraph/Visitors/ComputeRenderQueueVisitor.hpp"
#include "Graphics/Rendering/RenderQueue.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

#include <cassert>

ComputeRenderQueueVisitor::ComputeRenderQueueVisitor()
	: mpRenderQueue(nullptr)
{}

ComputeRenderQueueVisitor::ComputeRenderQueueVisitor(RenderQueue* pRenderQueue)
	: mpRenderQueue(pRenderQueue)
{
	assert(mpRenderQueue != nullptr);
}

ComputeRenderQueueVisitor::~ComputeRenderQueueVisitor()
{
	if (mpRenderQueue)
	{
		mpRenderQueue = nullptr;
	}
}

void ComputeRenderQueueVisitor::Traverse(Node* pNode)
{
	NodeVisitor::Traverse(pNode);
}

void ComputeRenderQueueVisitor::Visit(Node* pNode)
{
	NodeVisitor::Visit(pNode);
}

void ComputeRenderQueueVisitor::Visit(GroupNode* pNode)
{
	NodeVisitor::Visit(pNode);
}

void ComputeRenderQueueVisitor::Visit(GeometryNode* pNode)
{
	assert(pNode != nullptr);
	assert(mpRenderQueue != nullptr);

	mpRenderQueue->Push(pNode);
}

void ComputeRenderQueueVisitor::Visit(CameraNode* pNode)
{
	assert(pNode != nullptr);

	//TODO
}