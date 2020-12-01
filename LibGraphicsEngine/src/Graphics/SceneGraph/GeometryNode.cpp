#include "GeometryNode.hpp"
#include <algorithm> // std::remove()
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

GeometryNode::GeometryNode()
	: Node()
{}

GeometryNode::GeometryNode(const std::string& name)
	: Node(name)
{

}

GeometryNode::~GeometryNode()
{
	mPrimitives.clear();
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