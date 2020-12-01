#include "Node.hpp"
#include "GroupNode.hpp"
#include "Graphics/Components/NodeComponent.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

Node::Node()
	: mName()
	, mpParent(nullptr)
	, mIsEnabled(false)
{}

Node::Node(const std::string& name)
	: mName(name)
	, mpParent(nullptr)
	, mIsEnabled(false)
{
}

Node::~Node()
{
	if (mpParent)
	{
		mpParent = nullptr;
	}


}

const std::string& Node::GetName() const
{
	return mName;
}

bool_t Node::HasParent() const
{
	return (mpParent != nullptr);
}

Node* Node::GetParent() const
{
	return mpParent;
}

void Node::SetParent(Node* pParent)
{
	mpParent = pParent;
}

NodeComponent* Node::GetComponentWithName(const std::string& name)
{
	assert(name.empty() == false);

	return mComponentMap[name];
}

bool_t Node::HasComponent(NodeComponent* pComponent)
{
	assert(pComponent != nullptr);

	auto it = mComponentMap.find(pComponent->GetName());

	return ((it != mComponentMap.end()) && (it->second != nullptr));
}

void Node::AttachComponent(NodeComponent* pComponent)
{
	assert(pComponent != nullptr);

	if (HasComponent(pComponent))
		return;

	mComponentMap[pComponent->GetName()] = pComponent;
}

Node* Node::DettachFromParent()
{
	auto pNode = this;

	GroupNode* pParent = GetParent< GroupNode >();
	if (pParent != nullptr)
	{
		pParent->DettachNode(pNode);
	}

	return pNode;
}

void Node::DettachComponent(NodeComponent* pComponent)
{
	assert(pComponent != nullptr);

	DettachComponentWithName(pComponent->GetName());
}

void Node::DettachComponentWithName(const std::string& componentName)
{
	assert(componentName.empty() == false);

	auto it = mComponentMap.find(componentName);

	if ((it != mComponentMap.end()) && (it->second != nullptr))
	{
		it->second->SetNode(nullptr);
		mComponentMap.erase(it);
	}
}

void Node::DettachAllComponents()
{
	for (auto& comp : mComponentMap)
	{
		comp.second->SetNode(nullptr);
	}

	mComponentMap.clear();
}

void Node::UpdateComponents(bfloat32_t deltaTime)
{
	ForEachComponent([&deltaTime](NodeComponent* component) { if (component->GetIsEnabled()) component->Update(deltaTime); });
}

void Node::ForEachComponent(std::function< void(NodeComponent*) > callback)
{
	for (auto& comp : mComponentMap)
	{
		if (comp.second)
			callback(comp.second);
	}
}

bool_t Node::GetIsEnabled() const
{
	return mIsEnabled;
}

void Node::SetIsEnabled(bool_t isEnabled)
{
	mIsEnabled = isEnabled;
}


void Node::Visit(std::function<void(Node*)> callback)
{
	if (callback)
		callback(this);
}