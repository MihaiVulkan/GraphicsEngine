#ifndef GRAPHICS_FPS_CAMERA_HPP
#define GRAPHICS_FPS_CAMERA_HPP

#include "Camera.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class FPSCamera : public Camera
		{
			GE_RTTI(GraphicsEngine::Graphics::FPSCamera)

		public:
			enum class CAMERA_DIRECTIONS : uint8_t
			{
				CD_FORWARD = 0,
				CD_BACKWARD,
				CD_RIGHT,
				CD_LEFT,
				CD_UP,
				CD_DOWN,
				CD_COUNT
			};

			FPSCamera();
			explicit FPSCamera(const std::string& name);
			virtual ~FPSCamera();

			void UpdateOrientationWithMouse(bfloat32_t dx, bfloat32_t dy);
			void UpdatePositionWithKeyboard(bfloat32_t value, const FPSCamera::CAMERA_DIRECTIONS& dir);


			bfloat32_t GetPitch() const;
			bfloat32_t GetYaw() const;


		private:
			NO_COPY_NO_MOVE(FPSCamera);

			// ORIENTATION - Euler Angles
			bfloat32_t mPitch; // around OX axis
			bfloat32_t mYaw; // around OY axis

			bool_t mUseConstraints;
		};
	}
}

#endif // GRAPHICS_FPS_CAMERA_HPP