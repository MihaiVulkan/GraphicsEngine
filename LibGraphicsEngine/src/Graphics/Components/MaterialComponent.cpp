#include "MaterialComponent.hpp"
#include "Graphics/Rendering/Resources/Material.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

MaterialComponent::MaterialComponent()
	: NodeComponent()
	, mpMaterial(nullptr)
{}

MaterialComponent::~MaterialComponent()
{
	if (mpMaterial)
	{
		mpMaterial = nullptr;
	}
}

void MaterialComponent::Start()
{

}

void MaterialComponent::Update(bfloat32_t deltaTime)
{

}

const char_t* MaterialComponent::GetName() const
{
	return GetClassName_();
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