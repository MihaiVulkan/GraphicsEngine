#ifndef GRAPHICS_FPS_CAMERA_HPP
#define GRAPHICS_FPS_CAMERA_HPP

#include "Camera.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		/* FPS Camera */
		class FPSCamera : public Camera
		{
			GE_RTTI(GraphicsEngine::Graphics::FPSCamera)

		public:
			enum class CAMERA_DIRECTIONS : uint8_t
			{
				GE_CD_FORWARD = 0,
				GE_CD_BACKWARD,
				GE_CD_RIGHT,
				GE_CD_LEFT,
				GE_CD_UP,
				GE_CD_DOWN,
				GE_CD_COUNT
			};

			FPSCamera();
			explicit FPSCamera(const std::string& name);
			virtual ~FPSCamera();

			void UpdateOrientationWithMouse(float32_t dx, float32_t dy);
			void UpdatePositionWithKeyboard(float32_t value, const FPSCamera::CAMERA_DIRECTIONS& dir);


			float32_t GetPitch() const;
			float32_t GetYaw() const;


		private:
			NO_COPY_NO_MOVE(FPSCamera);

			// ORIENTATION - Euler Angles
			float32_t mPitch; // around OX axis
			float32_t mYaw; // around OY axis

			bool_t mUseConstraints;
		};
	}
}

#endif // GRAPHICS_FPS_CAMERA_HPP