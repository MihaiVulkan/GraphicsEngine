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
/*
Camera::Camera(const std::string& name, const GlobalConfig& config)
	: Camera(i_Name)
	{
		m_Position = i_Config.Camera.InitialPosition;
		m_Pitch = i_Config.Camera.InitialPitch;
		m_Yaw = i_Config.Camera.InitialYaw;
		m_InitialFOVy = m_FOVy = i_Config.Camera.InitialFieldOfView;
		m_ZNear = i_Config.Camera.InitialZNear;
		m_ZFar = i_Config.Camera.InitialZFar;
		m_UseConstraints = i_Config.Camera.UseConstraints;

		LOG_INFO("Camera %s successfully created!", m_Name.c_str());
	}
*/

Camera::~Camera()
{
	LOG_INFO("%s successfully destroyed!", mName.c_str());
}

void Camera::UpdateViewMatrix()
{
	// mPosition + mForward - creates the target camera is looking at
	mView = glm::lookAt(mPosition, mPosition + mForward, mUp);

	mProjectionView = mProjection * mView;

#ifdef USE_INVERSE_MATRICES
	// update the other matrices too
	// the view matrix is orthogonal so it's inverse is actually it's transpose
	// the transpose operation is much cheaper than inverse one
	// more info about view matrix: http://www.3dgep.com/understanding-the-view-matrix/
//	mInverseView = glm::inverse(m_View);
	mInverseView = glm::transpose(m_View);

	// projection * view matrix is NOT orthogonal
	mInverseProjectionView = glm::inverse(mProjectionView);
#endif // USE_INVERSE_MATRICES
}

void Camera::UpdatePerspectiveProjectionMatrix()
{
	assert(mFOVy > 0);
	assert(mAspectRatio > 0);
	assert(mZNear > 0);
	assert(mZFar > 0);

	float correctionFactor = ComputePerspectiveProjectionCorrectionFactor();

	mProjection = glm::perspective(glm::radians(static_cast<float32_t>(mFOVy)), mAspectRatio,
		mZNear * correctionFactor, mZFar * correctionFactor);

	//#if defined(RIGHT_HAND_COORDINATES)
#if defined(VULKAN_RENDERER)
	// Vulkan clip space has inverted Y and half Z - perspective projection.
	// we use GLM_FORCE_DEPTH_ZERO_TO_ONE to correct depth range (half Z)!
	// the operation below inverts Y to correct it
	mProjection[1][1] *= -1.0f;
#endif // VULKAN_RENDERER
	//#endif // RIGHT_HAND_COORDINATES

	mProjectionView = mProjection * mView;

#ifdef USE_INVERSE_MATRICES
	// update the other matrices too
	// more info about projection matrix: http://www.songho.ca/opengl/gl_projectionmatrix.html
	// projection matrix is NOT orthogonal, so we can't replace inverse operation with transpose one !!!
	mInverseProjection = glm::inverse(mProjection);

	// projection * view matrix is NOT orthogonal
	mInverseProjectionView = glm::inverse(mProjectionView);
#endif // USE_INVERSE_MATRICES
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

//void Camera::UpdatePerspectiveProjectionMatrix(uint32_t windowWidth, uint32_t windowHeight)
//{
//	assert(windowWidth > 0);
//	assert(windowHeight > 0);
//	assert(mFOVy > 0);
//	assert(mZNear > 0);
//	assert(mZFar > 0);
//
//	float correctionFactor = ComputePerspectiveProjectionCorrectionFactor();
//
//	mProjection = glm::perspective(glm::radians(static_cast<float32_t>(mFOVy)), windowWidth /static_cast<float32_t>(windowHeight),
//					               mZNear * correctionFactor, mZFar * correctionFactor);
//
////#if defined(RIGHT_HAND_COORDINATES)
//#if defined(VULKAN_RENDERER)
//	// Vulkan clip space has inverted Y and half Z - perspective projection.
//	// we use GLM_FORCE_DEPTH_ZERO_TO_ONE to correct depth range (half Z)!
//	// the operation below inverts Y to correct it
//	mProjection[1][1] *= -1.0f;
//#endif // VULKAN_RENDERER
////#endif // RIGHT_HAND_COORDINATES
//
//	mProjectionView = mProjection * mView;
//
//#ifdef USE_INVERSE_MATRICES
//	// update the other matrices too
//	// more info about projection matrix: http://www.songho.ca/opengl/gl_projectionmatrix.html
//	// projection matrix is NOT orthogonal, so we can't replace inverse operation with transpose one !!!
//	mInverseProjection = glm::inverse(mProjection);
//
//	// projection * view matrix is NOT orthogonal
//	mInverseProjectionView = glm::inverse(mProjectionView);
//#endif // USE_INVERSE_MATRICES
//}

void Camera::UpdatePerspectiveProjectionMatrix(int32_t fovy, float32_t aspect, float32_t zNear, float32_t zFar)
{
	assert(fovy > 0);
	assert(aspect > 0);
	assert(zNear > 0);
	assert(zFar > 0);

	mFOVy = fovy;
	mAspectRatio = aspect;
	mZNear = zNear;
	mZFar = zFar;

	UpdatePerspectiveProjectionMatrix();
}

void Camera::UpdateOrthographicProjectionMatrix(float32_t left, float32_t right, float32_t bottom, float32_t top, float32_t zNear, float32_t zFar)
{
	assert(zNear > 0);
	assert(zFar > 0);

	// NOTE! No FOV for the ortho projection
	LOG_INFO("No FOV is used for the ortho projection! It remains unchanged!");

	mZNear = zNear;
	mZFar = zFar;

	mProjection = glm::ortho(left, right, bottom, top, zNear, zFar);

//#if defined(RIGHT_HAND_COORDINATES)
#if defined(VULKAN_RENDERER)
	// Vulkan clip space has inverted Y and half Z - perspective projection.
	// we use GLM_FORCE_DEPTH_ZERO_TO_ONE to correct depth range (half Z)!
	// the operation below inverts Y to correct it
	mProjection[1][1] *= -1.0f;
#endif // VULKAN_RENDERER
//#endif // RIGHT_HAND_COORDINATES

	mProjectionView = mProjection * mView;

#ifdef USE_INVERSE_MATRICES
	// update the other matrices too
	// more info about projection matrix: http://www.songho.ca/opengl/gl_projectionmatrix.html
	// projection matrix is NOT orthogonal, so we can't replace inverse operation with transpose one !!!
	mInverseProjection = glm::inverse(mProjection);

	// projection * view matrix is NOT orthogonal
	mInverseProjectionView = glm::inverse(mProjectionView);
#endif // USE_INVERSE_MATRICES
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

int Camera::GetFOV() const
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
#ifdef USE_INVERSE_MATRICES
const glm::mat4& Camera::GetInverseViewMatrix() const
{
	return mInverseView;
}

const glm::mat4& Camera::GetInverseProjectionMatrix() const
{
	return mInverseProjection;
}

const glm::mat4& Camera::GetInverseProjectionViewMatrix() const
{
	return mInverseProjectionView;
}
#endif // USE_INVERSE_MATRICES
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

void Camera::SetFOV(int32_t fov)
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