#include "Graphics/SceneGraph/GroupNode.hpp"
#include <algorithm> // std::remove()
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

#include <cassert>

GroupNode::GroupNode()
	: Node()
{
	Create();
}

GroupNode::GroupNode(const std::string& name)
	: Node(name)
{
	Create();
}

GroupNode::~GroupNode()
{
	Destroy();
}

void GroupNode::Create()
{

}

void GroupNode::Destroy()
{
	// TODO - properly destroy all children

	mChildren.clear();
}

bool_t GroupNode::HasChildren() const
{
	return ! mChildren.empty();
}

void GroupNode::AttachNode(Node* pNode)
{
	assert(pNode != nullptr);

	// node to add already is attached to this parent
	if (pNode->GetParent() == this)
		return;

	pNode->SetParent(this);

	mChildren.push_back(pNode);
}

void GroupNode::DettachNode(Node* pNode)
{
	assert(pNode != nullptr);

	// if this node is not the parent of pNaode then we exit
	if (pNode->GetParent() != this)
		return;

	pNode->SetParent(nullptr);

	// efficiant remove
	mChildren.erase(std::remove(mChildren.begin(), mChildren.end(), pNode), mChildren.end());
}

void GroupNode::DettachAllNodes()
{
	for (auto& pNode : mChildren)
	{
		pNode->SetParent(nullptr);
	}

	mChildren.clear();
}

Node* GroupNode::GetNodeAt(uint32_t index)
{
	assert(index < mChildren.size());

	return mChildren[index];
}

Node* GroupNode::GetNode(const std::string& nodeName)
{
	assert(nodeName.empty() == false);

	for (auto& pNode : mChildren)
	{
		if (pNode->GetName() == nodeName)
			return pNode;
	}

	return nullptr;
}

void GroupNode::ForEachNode(std::function< void(Node*) > callback)
{
	//// process the children
	for (auto& pNode : mChildren)
	{
		if (pNode)
			callback(pNode);
	}
}

void GroupNode::Accept(NodeVisitor& visitor)
{
	visitor.Visit(this);
}