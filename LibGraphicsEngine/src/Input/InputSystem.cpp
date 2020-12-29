/* Author: BAIRAC MIHAI */

#include "InputSystem.hpp"

#include "Foundation/Platform/Platform.hpp"
#include "Graphics/Camera/FPSCamera.hpp"
#include "Foundation/Logger.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

InputSystem::InputSystem()
	: mpWindow(nullptr)
	, mpCamera(nullptr)
	, mInputMode(InputMode::GE_IM_COUNT)
	, mIsCursorCapured(false)
{}


InputSystem::InputSystem(Platform::GE_Window* pWindow, Graphics::Camera* pCamera, InputSystem::InputMode inputMode)
	: mpWindow(pWindow)
	, mpCamera(pCamera)
	, mInputMode(inputMode)
	, mIsCursorCapured(false)
{
	Init();
}

InputSystem::~InputSystem()
{
	Terminate();
}

void InputSystem::Init()
{
	assert(mpWindow != nullptr);
	assert(mpCamera != nullptr);

	// Initial Input Setup
	//mKeyboardKeys

	// Callbacks

	//auto cb = std::bind(&Engine::onMouseButton, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	//Platform::RegisterMouseButtonCallback(mpWindow, (Platform::GE_MouseButtonFN)&cb);

	Platform::RegisterMouseButtonCallback(mpWindow,
		[](Platform::GE_Window* pWindow, int32_t button, int32_t action, int32_t mods)
		{
			;
		});
	Platform::RegisterMouseMoveCallback(mpWindow,
		[](Platform::GE_Window* pWindow, int32_t xPos, int32_t yPos)
		{
			;
		});
	Platform::RegisterKeyCallback(mpWindow,
		[this](Platform::GE_Window* pWindow, int32_t key, int32_t scancode, int32_t action, int32_t mods)
		{
			if (pWindow->keys[GE_KEY_ESCAPE])
			{
				Platform::SetShouldWindowClose(pWindow, true);
			}

			if (mInputMode == InputMode::GE_IM_FPS)
			{
				if (pWindow->keys[GE_KEY_1])
				{
					mIsCursorCapured = !mIsCursorCapured;

					(mIsCursorCapured ? Platform::HideCursor() : Platform::ShowCursor());
				}
			}
		});

	// MOUSE - Setup depending on the Cursor Mode

	switch (mInputMode)
	{
		case InputMode::GE_IM_STANDARD:
			break;
		case InputMode::GE_IM_LOOK_AT:
			break;
		case InputMode::GE_IM_FPS:
		{
			// In First Person Shooter mode
			// The mouse cursor is:
			// - hidden & captured
			// - always set to the center of the screen
			// - orientation is calculated based on its previous position and the center of screen
			// THe keyboard input is also captured
			// The keyboard is used to navigate freely using W, A, S, D or the arraw keys
			// keyboard is in repeat mode - the user can keep pressing a key for a longer period of time

		//	Platform::UpdateWindowFlags(mWindowPtr, Platform::GE_WindowFlags::GE_WINDOW_INPUT_GRABBED);

			mIsCursorCapured = true;

			Platform::HideCursor();

			UpdateCursorPos();
		}
		break;
	}

	// KEYBOARD
	// ...
}

void InputSystem::Terminate()
{
	mInputMode = InputMode::GE_IM_COUNT;
	mIsCursorCapured = false;

	if (mpCamera)
	{
		mpCamera = nullptr;
	}

	if (mpWindow)
	{
		mpWindow = nullptr;
	}
}

void InputSystem::UpdateCursorPos()
{
	// conversion unsigned int -> int
	uint32_t winWidth = 0, winHeight = 0;
	Platform::GetWindowSize(mpWindow, &winWidth, &winHeight);

	Platform::SetCursorPos(mpWindow, winWidth / 2, winHeight / 2);
}

void InputSystem::UpdateContinuousInput(float32_t deltaTime)
{
	switch (mInputMode)
	{
	case InputMode::GE_IM_STANDARD:
		break;
	case InputMode::GE_IM_LOOK_AT:
		break;
	case InputMode::GE_IM_FPS:
		UpdateFPSInput(deltaTime);
		break;
	}

	mpCamera->UpdateViewMatrix();
}

void InputSystem::UpdateFPSInput(float32_t deltaTime)
{
	//TODO - remove dynamic_cast at runtime as it is really slow!!!
	FPSCamera* pFPSCamera = dynamic_cast<FPSCamera*>(mpCamera);
	assert(pFPSCamera != nullptr);

	{
		///////// KEYBOARD ///////////
		const float32_t KeySpeed = 0.01f;
		float32_t deltaVal = KeySpeed * deltaTime;

		if (mpWindow->keys[GE_KEY_W] || mpWindow->keys[GE_KEY_UP])
		{
			pFPSCamera->UpdatePositionWithKeyboard(deltaVal, FPSCamera::CAMERA_DIRECTIONS::GE_CD_FORWARD);
		}

		if (mpWindow->keys[GE_KEY_S] || mpWindow->keys[GE_KEY_DOWN])
		{
			pFPSCamera->UpdatePositionWithKeyboard(deltaVal, FPSCamera::CAMERA_DIRECTIONS::GE_CD_BACKWARD);
		}

		if (mpWindow->keys[GE_KEY_A] || mpWindow->keys[GE_KEY_LEFT])
		{
			pFPSCamera->UpdatePositionWithKeyboard(deltaVal, FPSCamera::CAMERA_DIRECTIONS::GE_CD_LEFT);
		}

		if (mpWindow->keys[GE_KEY_D] || mpWindow->keys[GE_KEY_RIGHT])
		{
			pFPSCamera->UpdatePositionWithKeyboard(deltaVal, FPSCamera::CAMERA_DIRECTIONS::GE_CD_RIGHT);
		}

		if (mpWindow->keys[GE_KEY_U])
		{
			pFPSCamera->UpdatePositionWithKeyboard(deltaVal, FPSCamera::CAMERA_DIRECTIONS::GE_CD_UP);
		}

		if (mpWindow->keys[GE_KEY_B])
		{
			pFPSCamera->UpdatePositionWithKeyboard(deltaVal, FPSCamera::CAMERA_DIRECTIONS::GE_CD_DOWN);
		}
	}

	//////////// MOUSE /////////////////

	if (mIsCursorCapured)
	{
		int32_t cursorXPos = 0, cursorYPos = 0;
		Platform::GetCursorPos(mpWindow, &cursorXPos, &cursorYPos);

		//	LOG_DEBUG("Mouse Pos: x: %d, y: %d", cursorXPos, cursorYPos);

		uint32_t winWidth = 0, winHeight = 0;
		Platform::GetWindowSize(mpWindow, &winWidth, &winHeight);

		// conversion unsigned int -> int
		int32_t halfWidth = winWidth / 2;
		int32_t halfHeight = winHeight / 2;

		const float32_t MouseSpeed = 0.005f;
		float32_t dx = (halfWidth - cursorXPos) * MouseSpeed * deltaTime;
		float32_t dy = (halfHeight - cursorYPos) * MouseSpeed * deltaTime;

		//	LOG_DEBUG("Delta Mouse Pos: dx: %f, dy: %f", dx, dy);

		Platform::SetCursorPos(mpWindow, halfWidth, halfHeight);

		// update camera
		pFPSCamera->UpdateOrientationWithMouse(dx, dy);
	}
}