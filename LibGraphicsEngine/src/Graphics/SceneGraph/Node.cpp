#include "Graphics/SceneGraph/Node.hpp"
#include "Graphics/SceneGraph/GroupNode.hpp"
#include "Graphics/Components/NodeComponent.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "glm/matrix.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

Node::Node()
	: mName()
	, mpParent(nullptr)
	, mIsEnabled(false)
	, mModelMatrix(glm::mat4(1.0f)) //identity matrix
	, mNormalMatrix(glm::mat3(1.0f))
{
	Create();
}

Node::Node(const std::string& name)
	: mName(name)
	, mpParent(nullptr)
	, mIsEnabled(false)
	, mModelMatrix(glm::mat4(1.0f)) //identity matrix
	, mNormalMatrix(glm::mat3(1.0f))
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
		GE_FREE(pComponent); //TODO - memory management

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

void Node::AddAllowedPass(VisualPass::PassType passType)
{
	mAllowedPasses.insert(passType);
}

bool_t Node::IsPassAllowed(VisualPass::PassType passType) const
{
	assert(passType < VisualPass::PassType::GE_PT_COUNT);

	return mAllowedPasses.find(passType) != mAllowedPasses.end();
}

const glm::mat4& Node::GetModelMatrix() const
{
	return mModelMatrix;
}

void Node::SetModelMatrix(const glm::mat4& transform)
{
	// NOTE! Model matrix will bring the positions/vertices in World Space
	mModelMatrix = transform;

	// every time we set the model matrix we also compute the normal matrix as it depends on the model one!
	// NOTE! Normal matrix will bring the normals in World Space
	ComputeNormalMatrix();
}

const glm::mat4& Node::GetNormalMatrix() const
{
	return mNormalMatrix;
}

void Node::ComputeNormalMatrix()
{
	if (mModelMatrix != glm::mat4(1.0f)) // if Identity matrix, no expensive computation needed
	{
		mNormalMatrix = glm::transpose(glm::inverse(mModelMatrix));
	}
}

void Node::Traverse(NodeVisitor& visitor)
{
	visitor.Traverse(this);
}

void Node::Accept(NodeVisitor& visitor)
{
	visitor.Visit(this);
}
