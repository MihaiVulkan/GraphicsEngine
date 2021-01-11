#include "Foundation/Platform/PlatformInternal.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace Platform;

KeyMap::KeyMap()
{}

KeyMap::~KeyMap()
{}

///////////////////////////////////////////

Window::Window()
//	: mpKeyMap(nullptr)
{}

Window::~Window()
{
}

void Window::UpdateWindowFlags(uint32_t flags)
{
	if (mState.flags != flags)
	{
		mState.flags |= flags;
	}

	if (mState.flags & GE_WindowFlags::GE_WF_VISIBLE)
	{
		ShowWindow();
	}
	// by default window is hidden

	if (mState.flags & GE_WindowFlags::GE_WF_MAXIMIZED)
	{
		MaximizeWindow();
	}
	else if (mState.flags & GE_WindowFlags::GE_WF_MINIMIZED)
	{
		MinimizeWindow();
	}

	if (mState.flags & GE_WindowFlags::GE_WF_INPUT_GRABBED)
	{
		FocusWindow();
	}
}

const char_t* Window::GetWindowTitle()
{
	return mState.title.c_str();
}

bool Window::GetShouldWindowClose()
{
	return mState.shouldClose;
}

void Window::SetShouldWindowClose(bool_t shouldClose)
{
	mState.shouldClose = shouldClose;
}

bool Window::ShouldWindowClose()
{
	return (mState.shouldClose == true);
}

//////////////////// INPUT ///////////////////////

// Notifies shared code of a physical key event
//
void Window::InputKey(int32_t key, int32_t scancode, int32_t action)
{
	if (key >= 0 && key <= GE_KEY_LAST)
	{
		if (action == GE_KEY_RELEASE && mState.keys[key] == GE_KEY_RELEASE)
			return;

		mState.keys[key] = static_cast<char_t>(action);
	}

	if (mCallbacks.onKey)
		mCallbacks.onKey(key, scancode, action);
}

// Notifies shared code of a mouse button click event
//
void Window::InputMouseButton(int32_t button, int32_t action)
{
	if (button < 0 || button > GE_MOUSE_BUTTON_LAST)
		return;

	mState.mouseButtons[button] = static_cast<char_t>(action);

	if (mCallbacks.onMouseButton)
		mCallbacks.onMouseButton(button, action);
}

// Notifies shared code of a scroll event
//
void Window::InputMouseScroll(float64_t xOffset, float64_t yOffset)
{
	if (mCallbacks.onMouseScroll)
		mCallbacks.onMouseScroll(xOffset, yOffset);
}

// Notifies shared code of a cursor motion event
// The position is specified in content area relative screen coordinates
//
void Window::InputMouseMove(int32_t xPos, int32_t yPos)
{
	if (mCallbacks.onMouseMove)
		mCallbacks.onMouseMove(xPos, yPos);
}

// Notifies shared code of a cursor enter/leave event
//
void Window::InputMouseEnterWindow(bool_t isEntered)
{

	if (mCallbacks.onMouseEnterWindow)
		mCallbacks.onMouseEnterWindow(isEntered);
}

////////////////////////////////

void Window::InputWindowFocus(bool_t isFocused)
{
	if (mCallbacks.onWindowFocus)
		mCallbacks.onWindowFocus(isFocused);

	if (false == isFocused)
	{
		int32_t key = -1, button = -1;

		for (key = 0; key <= GE_KEY_LAST; key++)
		{
			if (mState.keys[key] == GE_KEY_PRESS)
			{
				const int32_t scancode = GetKeyScancodeNative(key);
				InputKey(key, scancode, GE_KEY_RELEASE);
			}
		}

		for (button = 0; button <= GE_MOUSE_BUTTON_LAST; button++)
		{
			if (mState.mouseButtons[button] == GE_MOUSE_BUTTON_PRESS)
				InputMouseButton(button, GE_MOUSE_BUTTON_RELEASE);
		}
	}
}

// Notifies shared code that a window has moved
// The position is specified in content area relative screen coordinates
//
void Window::InputWindowPos(int32_t xPos, int32_t yPos)
{
	if (mCallbacks.onWindowPos)
		mCallbacks.onWindowPos(xPos, yPos);
}

// Notifies shared code that a window has been resized
// The size is specified in screen coordinates
//
void Window::InputWindowSize(uint32_t width, uint32_t height)
{
	if (mCallbacks.onWindowSize)
		mCallbacks.onWindowSize(width, height);
}

// Notifies shared code that a window has been iconified or restored
//
void Window::InputWindowMinimize(bool_t isMinimized)
{
	if (mCallbacks.onWindowMinimize)
		mCallbacks.onWindowMinimize(isMinimized);
}

// Notifies shared code that a window has been maximized or restored
//
void Window::InputWindowMaximize(bool_t isMaximized)
{
	if (mCallbacks.onWindowMaximize)
		mCallbacks.onWindowMaximize(isMaximized);
}

// Notifies shared code that the window contents needs updating
//
void Window::InputWindowRefresh()
{
	if (mCallbacks.onWindowRefresh)
		mCallbacks.onWindowRefresh();
}

void Window::InputWindowCloseRequest()
{
	mState.shouldClose = true;

	if (mCallbacks.onWindowClose)
		mCallbacks.onWindowClose();
}

///////////////////////////

void Window::RegisterWindowFocusCallback(InputWindowFocusFN onWindowFocus)
{
	mCallbacks.onWindowFocus = onWindowFocus;
}

void Window::RegisterWindowPosCallback(InputWindowPosFN onWindowPos)
{
	mCallbacks.onWindowPos = onWindowPos;
}

void Window::RegisterWindowSizeCallback(InputWindowSizeFN onWindowSize)
{
	mCallbacks.onWindowSize = onWindowSize;
}

void Window::RegisterWindowMinimizeCallback(InputWindowMinimizeFN onWindowMinimize)
{
	mCallbacks.onWindowMinimize = onWindowMinimize;
}

void Window::RegisterWindowMaximizeCallback(InputWindowMaximizeFN onWindowMaximize)
{
	mCallbacks.onWindowMaximize = onWindowMaximize;
}

void Window::RegisterWindowRefreshCallback(WindowRefreshFN onWindowRefresh)
{
	mCallbacks.onWindowRefresh = onWindowRefresh;
}

void Window::RegisterWindowCloseCallback(WindowCloseFN onWindowClose)
{
	mCallbacks.onWindowClose = onWindowClose;
}

void Window::RegisterMouseButtonCallback(MouseButtonFN onMouseButton)
{
	mCallbacks.onMouseButton = onMouseButton;
}

void Window::RegisterMouseMoveCallback(MouseMoveFN onMouseMove)
{
	mCallbacks.onMouseMove = onMouseMove;
}

void Window::RegisterMouseScrollCallback(MouseScollFN onMouseScroll)
{
	mCallbacks.onMouseScroll = onMouseScroll;
}

void Window::RegisterMouseEnterWindowCallback(MouseEnterWindowFN onMouseEnterWindow)
{
	mCallbacks.onMouseEnterWindow = onMouseEnterWindow;
}

void Window::RegisterKeyCallback(KeyFN onKey)
{
	mCallbacks.onKey = onKey;
}

////

void Window::UnregisterWindowFocusCallback()
{
	mCallbacks.onWindowFocus = nullptr;
}

void Window::UnregisterWindowPosCallback()
{
	mCallbacks.onWindowPos = nullptr;
}
void Window::UnregisterWindowSizeCallback()
{
	mCallbacks.onWindowSize = nullptr;
}

void Window::UnregisterWindowMinimizeCallback()
{
	mCallbacks.onWindowMinimize = nullptr;
}

void Window::UnregisterWindowMaximizeCallback()
{
	mCallbacks.onWindowMaximize = nullptr;
}
void Window::UnregisterWindowRefreshCallback()
{
	mCallbacks.onWindowRefresh = nullptr;
}

void Window::UnregisterWindowCloseCallback()
{
	mCallbacks.onWindowClose = nullptr;
}

void Window::UnregisterMouseButtonCallback()
{
	mCallbacks.onMouseButton = nullptr;
}
void Window::UnregisterMouseMoveCallback()
{
	mCallbacks.onMouseMove = nullptr;
}

void Window::UnregisterMouseScrollCallback()
{
	mCallbacks.onMouseScroll = nullptr;
}

void Window::UnregisterMouseEnterWindowCallback()
{
	mCallbacks.onMouseEnterWindow = nullptr;
}

void Window::UnregisterKeyCallback()
{
	mCallbacks.onKey = nullptr;
}

void Window::UnregisterAllCallbacks()
{
	// Clear all callbacks to avoid exposing a half torn-down window object
	::memset(&mCallbacks, 0, sizeof(mCallbacks));
}

void Window::ResetWindowData()
{
	UnregisterAllCallbacks();

	mState.title = "";
	mState.flags = 0;
	mState.width = 0;
	mState.height = 0;

	mState.isMinimized = 0;
	mState.isMaximized = 0;
	mState.shouldClose = 0;
	mState.isMouseInsideWindow = 0;

	::memset(mState.mouseButtons, 0, sizeof(mState.mouseButtons));
	::memset(mState.keys, 0, sizeof(mState.keys));
}

void Window::SetIsMinimized(bool_t value)
{
	mState.isMinimized = value;
}

void Window::SetIsMaximized(bool_t value)
{
	mState.isMaximized = value;
}

void Window::SetIsMouseInsideWindow(bool_t value)
{
	mState.isMouseInsideWindow = value;
}

const Window::WindowState& Window::GetState() const
{
	return mState;
}