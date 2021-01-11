#include "NodeVisitor.hpp"
#include "Graphics/SceneGraph/Node.hpp"
#include "Graphics/SceneGraph/GroupNode.hpp"
#include "Graphics/SceneGraph/GeometryNode.hpp"
#include "Graphics/SceneGraph/CameraNode.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

#include <cassert>

NodeVisitor::NodeVisitor()
{}

NodeVisitor::~NodeVisitor()
{}

void NodeVisitor::Traverse(Node* pNode)
{
	assert(pNode != nullptr);

	pNode->Accept(*this);
}

void NodeVisitor::Visit(Node* pNode)
{
	assert(pNode != nullptr);

	//Nothing to do
}

void NodeVisitor::Visit(GroupNode* pNode)
{
	assert(pNode != nullptr);

	pNode->ForEachNode([this](Node* pNode)
		{
			if (pNode)
				pNode->Accept(*this);
		});
}
void NodeVisitor::Visit(GeometryNode* pNode)
{
	assert(pNode != nullptr);

	pNode->Accept(*this);
}

void NodeVisitor::Visit(CameraNode* pNode)
{
	assert(pNode != nullptr);

	pNode->Accept(*this);
}