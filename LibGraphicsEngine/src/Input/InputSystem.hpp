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
		enum class GE_InputMode : uint8_t
		{
			GE_Standrd = 0,
			GE_LookAt,
			GE_FPS,
			GE_Invalid
		};

		InputSystem();
		virtual ~InputSystem();

		void Init(Platform::GE_Window* pWindow, Graphics::Camera* pCamera, GE_InputMode inputMode);

		void UpdateContinuousInput(bfloat32_t deltaTime);
		void UpdateFPSInput(bfloat32_t deltaTime);

	private:
		NO_COPY_NO_MOVE(InputSystem)

		Platform::GE_Window* mpWindow;
		Graphics::Camera* mpCamera;

		GE_InputMode mInputMode;
		bool_t mIsCursorCapured;
	};
}

#endif /* INPUT_SYSTEM_HPP */