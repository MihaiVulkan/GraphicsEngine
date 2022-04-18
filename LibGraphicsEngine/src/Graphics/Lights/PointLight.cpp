#include "Graphics/Lights/PointLight.hpp"
#include "Core/AppConfig.hpp" // activate needed GLM defines
#include "glm/gtc/matrix_transform.hpp" // perspective(), ortho(), lookAt(), transpose(), inverse()

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

PointLight::PointLight()
	: Light(Light::LightType::GE_LT_POINT)
	, mPosition(0.0f)
{}

PointLight::PointLight(const glm::vec3& pos, const Color3f& color)
	: Light(Light::LightType::GE_LT_POINT)
	, mPosition(pos)
{
	mColor = color;

	ComputeLightPVM();
}

PointLight::~PointLight()
{}

const glm::vec3& PointLight::GetPosition() const
{
	return mPosition;
}

void PointLight::SetPosition(const glm::vec3& pos)
{
	mPosition = pos;
}

void PointLight::ComputeLightPVM()
{
	// Matrix from light's point of view
	// TODO - avoid hardcoding the light FOV, zNear, zFar and aspect ratio

	float32_t FOV = 45.0f; // in degrees
	float32_t aspectRatio = 1.0f;

	// Keep depth range as small as possible
	// for better shadow map precision
	float32_t zNear = 1.0f;
	float32_t zFar = 96.0f;

	glm::mat4 proj = glm::perspective(glm::radians(FOV), aspectRatio, zNear, zFar);
	glm::mat4 view = glm::lookAt(mPosition, glm::vec3(0.0f), glm::vec3(0.0f, +1.0f, 0.0f));
	// no model matrix

#if defined(VULKAN_RENDERER)
	// Vulkan clip space has inverted Y and half Z - perspective projection.
	//1. we use GLM_FORCE_DEPTH_ZERO_TO_ONE to correct depth range (half Z)!
	//2. the operation below inverts Y to correct it
//	proj[1][1] *= -1.0f;
#endif // VULKAN_RENDERER

	mLightPVM = proj * view;
}