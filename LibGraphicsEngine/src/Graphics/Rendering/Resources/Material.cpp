#include "Material.hpp"
#include "Graphics/Rendering/Resources/Shader.hpp"
#include "Graphics/Rendering/Resources/Texture.hpp"
#include "Graphics/Rendering/Resources/UniformBuffer.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

Material::Material()
	: mpUniformBuffer(nullptr)
	, mProperties{}
{}

Material::Material(const Material::MaterialProperties& properties, const std::vector<Shader*>& shaders)
	: mpUniformBuffer(nullptr)
	, mProperties(properties)
	, mShaders(shaders)
{
	Create();
}

Material::~Material()
{
	Destroy();
}

void Material::Create()
{
	//TODO
}

void Material::Destroy()
{
	mShaders.clear();
}

void Material::SetShaders(const std::vector<Shader*>& shaders)
{
	mShaders.clear();

	mShaders = shaders;
}

const std::vector<Shader*>& Material::GetShaders() const
{
	return mShaders;
}

void Material::SetUniformBuffer(UniformBuffer* pUniformBuffer)
{
	assert(pUniformBuffer != nullptr);

	mpUniformBuffer = pUniformBuffer;
}

UniformBuffer* Material::GetUniformBuffer() const
{
	return mpUniformBuffer;
}

const Material::MaterialProperties& Material::GetMaterialProperties() const
{
	return mProperties;
}

const Material::TextureArray& Material::GetTextures() const
{
	return mTextures;
}