#include "NodeComponent.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

NodeComponent::NodeComponent()
	: mpNode(nullptr)
	, mIsEnabled(false)
{}

NodeComponent::~NodeComponent()
{
	mIsEnabled = false;

	if (mpNode)
	{
		mpNode = nullptr;
	}
}

const char_t* NodeComponent::GetName() const
{
	return GetClassName_();
}

Node* NodeComponent::GetNode() const
{
	return mpNode;
}

void NodeComponent::SetNode(Node* pNode)
{
	assert(pNode != nullptr);

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