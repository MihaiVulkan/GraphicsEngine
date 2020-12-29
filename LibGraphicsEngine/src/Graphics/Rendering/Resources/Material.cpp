#include "Material.hpp"
//#include "Graphics/Rendering/Resources/Shader.hpp"
#include "Graphics/Rendering/Resources/Texture.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

Material::Material()
	: mProperties{}
{}

Material::Material(const Material::MaterialProperties& properties)
	: mProperties(properties)
	//, mShaders(shaders)
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
	//mShaders.clear();
}

//void Material::SetShaders(const std::vector<Shader*>& shaders)
//{
//	mShaders.clear();
//
//	mShaders = shaders;
//}
//
//const std::vector<Shader*>& Material::GetShaders() const
//{
//	return mShaders;
//}

const Material::MaterialProperties& Material::GetMaterialProperties() const
{
	return mProperties;
}

const Material::TextureArray& Material::GetTextures() const
{
	return mTextures;
}