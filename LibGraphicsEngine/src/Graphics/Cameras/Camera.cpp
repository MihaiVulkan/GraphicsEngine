#include "Graphics/Cameras/Camera.hpp"
#include "Foundation/Logger.hpp"

// glm::vec3, glm::mat4 come from the header
#include "glm/gtc/matrix_transform.hpp" // perspective(), ortho(), lookAt(), transpose(), inverse()
#include "glm/geometric.hpp" // dot(), normalize(), cross()
#include "glm/trigonometric.hpp" // sin(), cos(), radians()
#include "glm/gtc/constants.hpp" // pi(), half_pi()
#include <utility> // std::move()


using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

Camera::Camera()
	: mName("Default"), mPosition(glm::vec3(0.0f))
	, mRight(glm::vec3(+1.0f, 0.0f, 0.0f)), mUp(glm::vec3(0.0f, +1.0f, 0.0f))
#if defined(RIGHT_HAND_COORDINATES)
	, mForward(glm::vec3(0.0f, 0.0f, -1.0f))
#elif defined(LEFT_HAND_COORDINATES)
	, mForward(glm::vec3(0.0f, 0.0f, +1.0f))
#endif // RIGHT_HAND_COORDINATES
	, mFOVy(0), mInitialFOVy(0), mAspectRatio(0.0f), mZNear(0.0f), mZFar(0.0f)
	, mView(glm::mat4(1.0f)), mProjection(glm::mat4(1.0f)), mProjectionView(glm::mat4(1.0f))

{
	LOG_INFO("%s successfully created!", mName.c_str());
}

Camera::Camera(const std::string& name)
	: Camera()
{
	mName = name;

	LOG_INFO("%s successfully created!", mName.c_str());
}

Camera::~Camera()
{
	LOG_INFO("%s successfully destroyed!", mName.c_str());
}

void Camera::UpdateViewMatrix()
{
	// mPosition + mForward - creates the target camera is looking at
	mView = glm::lookAt(mPosition, mPosition + mForward, mUp);

	mProjectionView = mProjection * mView;
}

void Camera::UpdatePerspectiveProjectionMatrix()
{
	assert(mFOVy > 0.0f);
	assert(mAspectRatio > 0.0f);
	assert(mZNear > 0.0f);
	assert(mZFar > 0.0f);

	float correctionFactor = ComputePerspectiveProjectionCorrectionFactor();

	mProjection = glm::perspective(glm::radians(mFOVy), mAspectRatio,
		mZNear * correctionFactor, mZFar * correctionFactor);

	//#if defined(RIGHT_HAND_COORDINATES)
#if defined(VULKAN_RENDERER)
	// Vulkan clip space has inverted Y and half Z - perspective projection.
	//1. we use GLM_FORCE_DEPTH_ZERO_TO_ONE to correct depth range (half Z)!
	//2. the operation below inverts Y to correct it
	mProjection[1][1] *= -1.0f;
#endif // VULKAN_RENDERER
	//#endif // RIGHT_HAND_COORDINATES

	mProjectionView = mProjection * mView;
}

/* NOTE! This function fixes two major problems with the ocean grid projection!
   a) If camera goes really, really high above the ocean then the grid was cut by the far plane
   The issue was getting worse as the altitude got bigger and bigger!
   Also, this would break realism, because of the visible gap between the ocean grid an the sky horizon
   Below sky horizon it's usually dark/black, so the black gap is really visible!
   b) If camera goes further and further away from the center of the world (0.0, 0.0, 0.0)
   it didn't matter in which direction (x, z) or y
   then the projected matrix/grid far points would vary a lot
   A very disturbing visual artifact, as if the grid is shaking a lot !!!
*/
float Camera::ComputePerspectiveProjectionCorrectionFactor() const
{
	float32_t cameraAltitude = mPosition.y;
	if (cameraAltitude > 0.0f)
	{
		if (glm::abs(cameraAltitude - 0.0f) < 1e-6f)
		{
			cameraAltitude = 0.000001f;
		}
		else if (cameraAltitude < 0.0f)
		{
			cameraAltitude *= -1.0f;
		}
	}
	else
	{
		cameraAltitude = 1.0f;
	}

	return cameraAltitude;
}

void Camera::UpdatePerspectiveProjectionMatrix(float32_t fovy, float32_t aspect, float32_t zNear, float32_t zFar)
{
	assert(fovy > 0.0f);
	assert(aspect > 0.0f);
	assert(zNear > 0.0f);
	assert(zFar > 0.0f);

	mFOVy = fovy;
	mAspectRatio = aspect;
	mZNear = zNear;
	mZFar = zFar;

	UpdatePerspectiveProjectionMatrix();
}

void Camera::UpdateOrthographicProjectionMatrix(float32_t left, float32_t right, float32_t bottom, float32_t top, float32_t zNear, float32_t zFar)
{
	assert(zNear > 0.0f);
	assert(zFar > 0.0f);

	// NOTE! No FOV for the ortho projection
	LOG_INFO("No FOV is used for the ortho projection! It remains unchanged!");

	mZNear = zNear;
	mZFar = zFar;

	mProjection = glm::ortho(left, right, bottom, top, zNear, zFar);

//#if defined(RIGHT_HAND_COORDINATES)
#if defined(VULKAN_RENDERER)
	// Vulkan clip space has inverted Y and half Z - perspective projection.
	//1. we use GLM_FORCE_DEPTH_ZERO_TO_ONE to correct depth range (half Z)!
	//2. the operation below inverts Y to correct it
	mProjection[1][1] *= -1.0f;
#endif // VULKAN_RENDERER
//#endif // RIGHT_HAND_COORDINATES

	mProjectionView = mProjection * mView;
}

const std::string& Camera::GetName() const
{
	return mName;
}

const glm::vec3& Camera::GetPosition() const
{
	return mPosition;
}

float Camera::GetAltitude() const
{
	return mPosition.y;
}

const glm::vec3& Camera::GetForward() const
{
	return mForward;
}

const glm::vec3& Camera::GetRight() const
{
	return mRight;
}

const glm::vec3& Camera::GetUp() const
{
	return mUp;
}

float32_t Camera::GetFOV() const
{
	return mFOVy;
}

float32_t Camera::GetAspectRatio() const
{
	return mAspectRatio;
}

float32_t Camera::GetZNear() const
{
	return mZNear;
}

float32_t Camera::GetZFar() const
{
	return mZFar;
}

const glm::mat4& Camera::GetViewMatrix() const
{
	return mView;
}

const glm::mat4& Camera::GetProjectionMatrix() const
{
	return mProjection;
}

const glm::mat4& Camera::GetProjectionViewMatrix() const
{
	return mProjectionView;
}

void Camera::SetPosition(const glm::vec3& position)
{
	mPosition = position;
}

void Camera::SetAltitude(float altitude)
{
	mPosition.y = altitude;
}

void Camera::SetForward(const glm::vec3& forward)
{
	mForward = forward;
}

void Camera::SetRight(const glm::vec3& right)
{
	mRight = right;
}

void Camera::SetUp(const glm::vec3& up)
{
	mUp = up;
}

void Camera::SetFOV(float32_t fov)
{
	mFOVy = fov;
}

void Camera::ResetFOV()
{
	mFOVy = mInitialFOVy;
}

void Camera::SetAspectRatio(float32_t aspectRatio)
{
	mAspectRatio = aspectRatio;
}

void Camera::SetZNear(float32_t zNear)
{
	mZNear = zNear;
}

void Camera::SetZFar(float32_t zFar)
{
	mZFar = zFar;
}

void Camera::SetViewMatrix(const glm::mat4& view)
{
	mView = view;
}

void Camera::SetProjectionMatrix(const glm::mat4& proj)
{
	mProjection = proj;
}