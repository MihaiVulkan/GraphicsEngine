#include "GeometryNode.hpp"
#include "Graphics/Components/VisualComponent.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <algorithm> // std::remove()
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

GeometryNode::GeometryNode()
	: Node()
{
	Create();
}

GeometryNode::GeometryNode(const std::string& name)
	: Node(name)
{
	Create();
}

GeometryNode::~GeometryNode()
{
	Destroy();
}

void GeometryNode::Create()
{
	// TODO - object lifetime management
	//AttachComponent(GE_ALLOC(VisualComponent));
}

void GeometryNode::Destroy()
{
	mPrimitives.clear();

	// TODO - object lifetime management
	//GE_FREE(mpVisualComponent);
}

void GeometryNode::AttachGeometry(GeometricPrimitive* pPrimitive)
{
	assert(pPrimitive != nullptr);

	mPrimitives.push_back(pPrimitive);
}

void GeometryNode::DettachGeometry(GeometricPrimitive* pPrimitive)
{
	assert(pPrimitive != nullptr);

	// efficiant remove
	mPrimitives.erase(std::remove(mPrimitives.begin(), mPrimitives.end(), pPrimitive), mPrimitives.end());
}

void GeometryNode::DettachAllGeometry()
{
	mPrimitives.clear();
}

void GeometryNode::ForEachPrimitive(std::function< void(GeometricPrimitive*) > callback)
{
	for (auto& primitive : mPrimitives)
	{
		if (primitive)
		{
			if (callback)
				callback(primitive);
		}
	}
}

void GeometryNode::Visit(std::function<void(Node*)> callback)
{
	if (callback)
		callback(this);

	//something else to do
}

const std::vector<GeometricPrimitive*>& GeometryNode::GetGeometricPrimitives() const
{
	return mPrimitives;
}