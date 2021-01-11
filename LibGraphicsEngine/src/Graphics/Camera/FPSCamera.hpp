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
			FPSCamera();
			explicit FPSCamera(const std::string& name);
			virtual ~FPSCamera();

			void UpdateOrientationWithMouse(float32_t dx, float32_t dy) override;
			void UpdatePositionWithKeyboard(float32_t value, const Camera::CAMERA_DIRECTIONS& dir) override;

			float32_t GetPitch() const override;
			float32_t GetYaw() const override;


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