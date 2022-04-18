#include "Graphics/Lights/DirectionalLight.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

DirectionalLight::DirectionalLight()
	: Light(Light::LightType::GE_LT_DIRECTIONAL)
	, mDirection(0.0f)
{}

DirectionalLight::DirectionalLight(const glm::vec3& dir, const Color3f& color)
	: Light(Light::LightType::GE_LT_DIRECTIONAL)
	, mDirection(dir)
{
	mColor = color;
}

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

void DirectionalLight::ComputeLightPVM()
{
	// https://stackoverflow.com/questions/29314995/projection-view-matrix-calculation-for-directional-light-shadow-mapping
	// https://gamedev.net/forums/topic/652431-directional-light-shadow-mapping-and-matrices/5127140/

	// TODO
}