#ifndef GRAPHICS_CAMERAS_FPS_CAMERA_HPP
#define GRAPHICS_CAMERAS_FPS_CAMERA_HPP

#include "Graphics/Cameras/Camera.hpp"

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

			virtual void UpdateOrientationWithMouse(float32_t dx, float32_t dy) override;
			virtual void UpdatePositionWithKeyboard(float32_t value, const Camera::CAMERA_DIRECTIONS& dir) override;

			virtual float32_t GetPitch() const override;
			virtual float32_t GetYaw() const override;

			virtual void EnableConstraints() override;

		private:
			NO_COPY_NO_MOVE_CLASS(FPSCamera);

			// ORIENTATION - Euler Angles
			float32_t mPitch; // around OX axis
			float32_t mYaw; // around OY axis

			bool_t mUseConstraints;
		};
	}
}

#endif // GRAPHICS_CAMERAS_FPS_CAMERA_HPP