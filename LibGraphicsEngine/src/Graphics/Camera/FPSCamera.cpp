#include "FPSCamera.hpp"
#include "glm/gtc/matrix_transform.hpp" // glm::lookAt()
#include "glm/gtc/constants.hpp" // glm::pi(), glm::half_pi()

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

FPSCamera::FPSCamera()
	: Camera()
#if defined(RIGHT_HAND_COORDINATES)
	, mYaw(glm::pi<float32_t>())
#elif defined(LEFT_HAND_COORDINATES)
	, mYaw(0.0f)
#endif // RIGHT_HAND_COORDINATES
	, mPitch(0.0f)
	, mUseConstraints(false)
{}

FPSCamera::FPSCamera(const std::string& name)
	: FPSCamera()
{
	mName = name;
}

FPSCamera::~FPSCamera()
{
	mPitch = 0.0f;
	mYaw = 0.0f;
	mUseConstraints = false;
}

void FPSCamera::UpdateOrientationWithMouse(float32_t dx, float32_t dy)
{
	//pitch & yaw angles are in RADIANS

	// we compute pitch and yaw angles directly in radians, so no convertion is necessary!

#if defined(LEFT_HAND_COORDINATES)
//	dx *= -1.0f;
#endif // LEFT_HAND_COORDINATES

	mPitch += dy;
	mYaw += dx;


	if (mUseConstraints)
	{
		// limit the pitch angle
		if (mPitch > glm::half_pi<float32_t>()) mPitch = glm::half_pi<float32_t>();
		if (mPitch < -glm::half_pi<float32_t>()) mPitch = -glm::half_pi<float32_t>();
	}

	// we convert the polar spherical coordinates to cartesian coordinates
	// https://en.wikipedia.org/wiki/Spherical_coordinate_system
	// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-6-keyboard-and-mouse/ 

	static const glm::vec3 worldUp(0.0f, +1.0f, 0.0f);

#if defined(RIGHT_HAND_COORDINATES)
	mForward.x = glm::cos(mPitch) * glm::sin(mYaw);
	mForward.z = glm::cos(mPitch) * glm::cos(mYaw);
	mForward.y = glm::sin(mPitch);
	mForward = glm::normalize(mForward);

	//mRight.x = glm::sin(mYaw - glm::half_pi<float>());
	//mRight.z = glm::cos(mYaw - glm::half_pi<float>());
	//mRight.y = 0.0f;
	mRight = glm::cross(mForward, worldUp);
	mRight = glm::normalize(mRight);

	mUp = glm::cross(mRight, mForward);
#elif defined(LEFT_HAND_COORDINATES)
	mForward.x = glm::cos(mPitch) * glm::sin(mYaw);
	mForward.z = glm::cos(mPitch) * glm::cos(mYaw);
	mForward.y = glm::sin(mPitch);
	mForward = glm::normalize(mForward);

	//mRight.x = glm::sin(mYaw - glm::half_pi<float>());
	//mRight.z = glm::cos(mYaw - glm::half_pi<float>());
	//mRight.y = 0.0f;
	mRight = glm::cross(worldUp, mForward);
	mRight = glm::normalize(mRight);

	mUp = glm::cross(mForward, mRight);
#endif // RIGHT_HAND_COORDINATES

	//	LOG("FORWARD x: %f y: %f z: %f", mForward.x, mForward.y, mForward.z);
	//	LOG("RIGHT x: %f y: %f z: %f", mRight.x, mRight.y, mRight.z);
	//	LOG("UP x: %f y: %f z: %f", mUp.x, mUp.y, mUp.z);
}

void FPSCamera::UpdatePositionWithKeyboard(float32_t value, const FPSCamera::CAMERA_DIRECTIONS& dir)
{
	glm::vec3 deltaMove(0.0f);

	if (dir == CAMERA_DIRECTIONS::GE_CD_FORWARD) deltaMove = mForward * value;
	if (dir == CAMERA_DIRECTIONS::GE_CD_BACKWARD) deltaMove = mForward * -value;
	if (dir == CAMERA_DIRECTIONS::GE_CD_RIGHT) deltaMove = mRight * value;
	if (dir == CAMERA_DIRECTIONS::GE_CD_LEFT) deltaMove = mRight * -value;
	if (dir == CAMERA_DIRECTIONS::GE_CD_UP) deltaMove = mUp * value;
	if (dir == CAMERA_DIRECTIONS::GE_CD_DOWN) deltaMove = mUp * -value;

	mPosition += deltaMove;

	//	LOG("Camera POS x: %f y: %f z: %f", mPosition.x, mPosition.y, mPosition.z);
}

float32_t FPSCamera::GetPitch() const
{
	return mPitch;
}

float32_t FPSCamera::GetYaw() const
{
	return mYaw;
}