#include "Graphics/Components/NodeComponent.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

NodeComponent::NodeComponent()
	: mName(GetClassName_())
	, mpNode(nullptr)
	, mIsEnabled(false)
{
	Create();
}

NodeComponent::NodeComponent(const std::string& name)
	: mName(name)
	, mpNode(nullptr)
	, mIsEnabled(false)
{
	Create();
}

NodeComponent::~NodeComponent()
{
	Destroy();
}

void NodeComponent::Create()
{
	//TODO
}

void NodeComponent::Destroy()
{
	mIsEnabled = false;

	if (mpNode)
	{
		mpNode = nullptr;
	}
}

void NodeComponent::Start()
{

}

void NodeComponent::Update(float32_t deltaTime)
{

}

void NodeComponent::OnAttach()
{

}

void NodeComponent::OnDettach()
{

}

const std::string& NodeComponent::GetName() const
{
	return mName;
}

void NodeComponent::SetName(const std::string& name)
{
	mName = name;
}

Node* NodeComponent::GetNode() const
{
	return mpNode;
}

void NodeComponent::SetNode(Node* pNode)
{
	//NOTE pNode can be nullptr

	mpNode = pNode;
}

bool_t NodeComponent::GetIsEnabled() const
{
	return mIsEnabled;
}

void NodeComponent::SetIsEnabled(bool_t isEnabled)
{
	mIsEnabled = isEnabled;
}