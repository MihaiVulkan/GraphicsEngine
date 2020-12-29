#ifndef INPUT_SYSTEM_HPP
#define INPUT_SYSTEM_HPP

#include "System.hpp"
#include <unordered_map>

namespace GraphicsEngine
{
	namespace Platform
	{
		class GE_Window;
	}

	namespace Graphics
	{
		class Camera;
	}

	/* Manages Input */
	class InputSystem : public System
	{
		GE_RTTI(GraphicsEngine::InputSystem)

	public:
		enum class InputMode : uint8_t
		{
			GE_IM_STANDARD = 0,
			GE_IM_LOOK_AT,
			GE_IM_FPS,
			GE_IM_COUNT
		};

		InputSystem();
		explicit InputSystem(Platform::GE_Window* pWindow, Graphics::Camera* pCamera, InputSystem::InputMode inputMode);
		virtual ~InputSystem();

		void UpdateContinuousInput(float32_t deltaTime);
		void UpdateFPSInput(float32_t deltaTime);

	private:
		NO_COPY_NO_MOVE(InputSystem)

		void Init();
		void Terminate();

		void UpdateCursorPos();

		Platform::GE_Window* mpWindow;
		Graphics::Camera* mpCamera;

		InputMode mInputMode;
		bool_t mIsCursorCapured;
	};
}

#endif /* INPUT_SYSTEM_HPP */