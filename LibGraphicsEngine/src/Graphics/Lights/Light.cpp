#include "Graphics/Lights/Light.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

Light::Light()
	: mLightType(LightType::GE_LT_DIRECTIONAL)
{}

Light::Light(LightType type)
	: mLightType(type)
{}

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
