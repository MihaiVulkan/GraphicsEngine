#include "Graphics/Lights/DirectionalLight.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

DirectionalLight::DirectionalLight()
	: Light(Light::LightType::GE_LT_DIRECTIONAL)
	, mDirection(0.0f)
	, mColor(1.0f) //white color by default
{}

DirectionalLight::DirectionalLight(const glm::vec3& dir, const glm::vec3& color)
	: Light(Light::LightType::GE_LT_DIRECTIONAL)
	, mDirection(dir)
	, mColor(color)
{}

DirectionalLight::~DirectionalLight()
{}

const glm::vec3& DirectionalLight::GetDirection() const
{
	return mDirection;
}

void DirectionalLight::SetDirection(const glm::vec3& dir)
{
	mDirection = dir;
}

const glm::vec3& DirectionalLight::GetColor() const
{
	return mColor;
}

void DirectionalLight::SetColor(const glm::vec3& color)
{
	mColor = color;
}