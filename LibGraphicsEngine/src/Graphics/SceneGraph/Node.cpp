#include "Graphics/SceneGraph/Node.hpp"
#include "Graphics/SceneGraph/GroupNode.hpp"
#include "Graphics/Components/NodeComponent.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

Node::Node()
	: mpParent(nullptr)
	, mIsEnabled(false)
{
	Create();
}

Node::Node(const std::string& name)
	: mpParent(nullptr)
	, mIsEnabled(false)
	, mName(name)
{
	Create();
}

Node::~Node()
{
	Destroy();
}

void Node::Create()
{

}

void Node::Destroy()
{
	DettachAllComponents();

	mName = "";
	mIsEnabled = false;

	if (mpParent)
	{
		mpParent = nullptr;
	}
}

const std::string& Node::GetName() const
{
	return mName;
}

void Node::SetName(const std::string& name)
{
	mName = name;
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

NodeComponent* Node::GetComponentWithName(const std::string& componentName)
{
	assert(componentName.empty() == false);
	if (mComponentMap.empty())
		return nullptr;

	auto it = mComponentMap.find(componentName);

	if (it == mComponentMap.end())
		return nullptr;

	return it->second;
}

bool_t Node::HasComponentWithName(const std::string& componentName)
{
	assert(componentName.empty() == false);
	if (mComponentMap.empty())
		return false;

	auto it = mComponentMap.find(componentName);

	return (it != mComponentMap.end());
}

bool_t Node::HasComponent(NodeComponent* pComponent)
{
	assert(pComponent != nullptr);
	assert(pComponent->GetName().empty() == false);

	return HasComponentWithName(pComponent->GetName());
}

void Node::AttachComponent(NodeComponent* pComponent)
{
	assert(pComponent != nullptr);
	assert(pComponent->GetName().empty() == false);

	if (HasComponent(pComponent))
		return;

	mComponentMap[pComponent->GetName()] = pComponent;
	pComponent->SetNode(this);
	pComponent->OnAttach();
}

Node* Node::DettachFromParent()
{
	auto pNode = this;

	GroupNode* pParent = GetParent< GroupNode >();
	if (pParent)
	{
		pParent->DettachNode(pNode);
	}

	return pNode;
}

void Node::DettachComponent(NodeComponent* pComponent)
{
	assert(pComponent != nullptr);
	assert(pComponent->GetName().empty() == false);

	DettachComponentWithName(pComponent->GetName());
}

void Node::DettachComponentWithName(const std::string& componentName)
{
	assert(componentName.empty() == false);
	if (mComponentMap.empty())
		return;

	auto it = mComponentMap.find(componentName);

	if (it != mComponentMap.end())
	{
		auto pComponent = it->second;
		if (pComponent)
		{
			pComponent->OnDettach();
			pComponent->SetNode(nullptr);
		}
		mComponentMap.erase(it);
	}
}

void Node::DettachAllComponents()
{
	ForEachComponent([](NodeComponent* pComponent)
		{
			if (pComponent)
			{
				pComponent->OnDettach();
				pComponent->SetNode(nullptr);
			}
		});

	mComponentMap.clear();
}

void Node::StartComponents()
{
	ForEachComponent([](NodeComponent* pComponent)
		{
			if (pComponent && pComponent->GetIsEnabled())
				pComponent->Start();
		});
}

void Node::UpdateComponents(float32_t deltaTime)
{
	ForEachComponent([&deltaTime](NodeComponent* pComponent)
		{ 
			if (pComponent && pComponent->GetIsEnabled())
				pComponent->Update(deltaTime);
		});
}

void Node::ForEachComponent(std::function< void(NodeComponent*) > callback)
{
	if (mComponentMap.empty())
		return;

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

void Node::Traverse(NodeVisitor& visitor)
{
	visitor.Traverse(this);
}

void Node::Accept(NodeVisitor& visitor)
{
	visitor.Visit(this);
}
