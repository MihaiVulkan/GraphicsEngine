#include "Graphics/Components/VisualComponent.hpp"
#include "Graphics/Rendering/VisualEffects/VisualEffect.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VisualComponent::VisualComponent()
	: NodeComponent()
	, mpVisualEffect(nullptr)
{
	Create();
}

VisualComponent::VisualComponent(const std::string& name)
	: NodeComponent(name)
	, mpVisualEffect(nullptr)
{
	Create();
}

VisualComponent::~VisualComponent()
{
	Destroy();
}

void VisualComponent::Create()
{
	SetName(GetClassName_());
}

void VisualComponent::Destroy()
{
	GE_FREE(mpVisualEffect);
}

void VisualComponent::Start()
{
	//TODO
}

void VisualComponent::Update(float32_t deltaTime)
{
	//TODO
}

void VisualComponent::OnAttach()
{
	//TODO
}

void VisualComponent::OnDettach()
{
	//TODO
}

VisualEffect* VisualComponent::GetVisualEffect()
{
	return mpVisualEffect;
}

void VisualComponent::SetVisualEffect(VisualEffect* pEffect)
{
	mpVisualEffect = pEffect;
}