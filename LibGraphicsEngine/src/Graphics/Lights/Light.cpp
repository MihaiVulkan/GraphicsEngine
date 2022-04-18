#include "Graphics/Lights/Light.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

Light::Light()
	: mLightType(LightType::GE_LT_DIRECTIONAL)
	, mColor(1.0f) // white by default
{}

Light::Light(LightType type)
	: Light()
{
	mLightType = type;
}

Light::~Light()
{
	mLightType = LightType::GE_PT_COUNT;
}

const Light::LightType& Light::GetLightType() const
{
	return mLightType;
}

void Light::SetLightType(const Light::LightType& type)
{
	mLightType = type;
}

const Color3f& Light::GetColor() const
{
	return mColor;
}

void Light::SetColor(const Color3f& color)
{
	mColor = color;
}

const glm::mat4& Light::GetLightPVM() const
{
	return mLightPVM;
}