#include "Graphics/Components/MaterialComponent.hpp"
#include "Graphics/Rendering/Resources/Material.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

MaterialComponent::MaterialComponent()
	: VisualComponent()
	, mpMaterial(nullptr)
{
	Create();
}

MaterialComponent::MaterialComponent(const std::string& name)
	: VisualComponent(name)
	, mpMaterial(nullptr)
{
	Create();
}

MaterialComponent::~MaterialComponent()
{
	Destroy();
}

void MaterialComponent::Create()
{
	SetName(GetClassName_());
}

void MaterialComponent::Destroy()
{
	if (mpMaterial)
	{
		mpMaterial = nullptr;
	}
}

void MaterialComponent::Start()
{
	//TODO
}

void MaterialComponent::Update(float32_t deltaTime)
{
	//TODO
}

Material* MaterialComponent::GetMaterial() const
{
	return mpMaterial;
}

void MaterialComponent::SetMaterial(Material* pMaterial)
{
	assert(pMaterial != nullptr);

	mpMaterial = pMaterial;
}