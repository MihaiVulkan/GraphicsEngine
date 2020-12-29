#include "PlatformInternal.hpp"
#include "Platform.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <cassert>

namespace GraphicsEngine
{
	namespace Platform
	{
		GE_KeyData g_KeyData;

		//////////////// INIT ////////////////////////

		void Init()
		{
			Init_Platform();
		}

		void Terminate()
		{
			Terminate_Platform();
		}


		/////////////////// WINDOW /////////////////////

		GE_Window* CreateWindow(const char_t* pTitle, int32_t xPos, int32_t yPos, int32_t width, int32_t height, uint32_t flags)
		{
			GE_Window* pWindow = GE_ALLOC(GE_Window);
			assert(pWindow != nullptr);

			// set all ptr data to nullptr
			ResetWindowData(pWindow);

			if (false == CreateWindow_Platform(pWindow, pTitle, xPos, yPos, width, height, flags))
			{
				DestroyWindow_Platform(pWindow);

				return nullptr;
			}

			UpdateWindowFlags(pWindow, flags);

			return pWindow;
		}

		void UpdateWindowFlags(GE_Window* pWindow, uint32_t flags)
		{
			assert(pWindow != nullptr);

			if (pWindow->flags != flags)
			{
				pWindow->flags |= flags;
			}

			if (pWindow->flags & GE_WindowFlags::GE_WINDOW_VISIBLE)
			{
				ShowWindow_Platform(pWindow);
			}
			// by default window is hidden

			if (pWindow->flags & GE_WindowFlags::GE_WINDOW_MAXIMIZED)
			{
				MaximizeWindow_Platform(pWindow);
			}
			else if (pWindow->flags & GE_WindowFlags::GE_WINDOW_MINIMIZED)
			{
				MinimizeWindow_Platform(pWindow);
			}

			if (pWindow->flags & GE_WindowFlags::GE_WINDOW_INPUT_GRABBED)
			{
				FocusWindow_Platform(pWindow);
			}
		}

		void DestroyWindow(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			DestroyWindow_Platform(pWindow);
		}

		void GetWindowPos(GE_Window* pWindow, int32_t* pXPos, int32_t* pYPos)
		{
			GetWindowPos_Platform(pWindow, pXPos, pYPos);
		}

		void SetWindowPos(GE_Window* pWindow, int32_t xPos, int32_t yPos)
		{
			SetWindowPos_Platform(pWindow, xPos, yPos);
		}

		void GetWindowSize(GE_Window* pWindow, uint32_t* pWidth, uint32_t* pHeight)
		{
			GetWindowSize_Platform(pWindow, pWidth, pHeight);
		}

		void SetWindowSize(GE_Window* pWindow, uint32_t width, uint32_t height)
		{
			SetWindowSize_Platform(pWindow, width, height);
		}

		void GetWindowTitle(GE_Window* pWindow, const char_t* pTitle)
		{
			GetWindowTitle_Platform(pWindow, pTitle);
		}

		void SetWindowTitle(GE_Window* pWindow, const char_t* pTitle)
		{
			SetWindowTitle_Platform(pWindow, pTitle);
		}

		void MinimizeWindow(GE_Window* pWindow)
		{
			MinimizeWindow_Platform(pWindow);
		}

		void MaximizeWindow(GE_Window* pWindow)
		{
			MaximizeWindow_Platform(pWindow);
		}

		void RestoreWindow(GE_Window* pWindow)
		{
			RestoreWindow_Platform(pWindow);
		}

		void ShowWindow(GE_Window* pWindow)
		{
			ShowWindow_Platform(pWindow);
		}

		void HideWindow(GE_Window* pWindow)
		{
			HideWindow_Platform(pWindow);
		}

		void FocusWindow(GE_Window* pWindow)
		{
			FocusWindow_Platform(pWindow);
		}

		bool IsWindowVisible(GE_Window* pWindow)
		{
			return IsWindowVisible_Platform(pWindow);
		}

		bool IsWindowMinimized(GE_Window* pWindow)
		{
			return IsWindowMinimized_Platform(pWindow);
		}

		bool IsWindowMaximized(GE_Window* pWindow)
		{
			return IsWindowMaximized_Platform(pWindow);
		}

		bool IsWindowFocused(GE_Window* pWindow)
		{
			return IsWindowFocused_Platform(pWindow);
		}

		bool GetShouldWindowClose(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			return pWindow->shouldClose;
		}

		void SetShouldWindowClose(GE_Window* pWindow, bool_t shouldClose)
		{
			assert(pWindow != nullptr);

			pWindow->shouldClose = shouldClose;
		}

		bool ShouldWindowClose(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			return (pWindow->shouldClose == true);
		}

		//////////////////// INPUT ///////////////////////

		// Notifies shared code of a physical key event
		//
		void InputKey(GE_Window* pWindow, int32_t key, int32_t scancode, int32_t action, int32_t mods)
		{
			assert(pWindow != nullptr);

			if (key >= 0 && key <= GE_KEY_LAST)
			{
				bool_t repeated = false;

				if (action == GE_KEY_RELEASE && pWindow->keys[key] == GE_KEY_RELEASE)
					return;

				//if (action == GE_KEY_PRESS && pWindow->keys[key] == GE_KEY_PRESS)
				//	repeated = true;

				//if (action == GE_KEY_RELEASE && pWindow->stickyKeys)
				//	pWindow->keys[key] = GE_STICKY_KEY;
				//else
				pWindow->keys[key] = static_cast<char_t>(action);

				//if (repeated)
				//	action = GE_KEY_REPEAT;
			}

			/*if (false == pWindow->lockKeyMods)
				mods &= ~(GE_MOD_CAPS_LOCK | GE_MOD_NUM_LOCK);*/

			if (pWindow->callbacks.onKey)
				pWindow->callbacks.onKey(pWindow, key, scancode, action, mods);
		}

		// Notifies shared code of a Unicode codepoint input event
		// The 'plain' parameter determines whether to emit a regular character event
		//
		//void InputChar(GE_Window* pWindow, unsigned int codepoint, int mods,bool plain)
		//{
		//	assert(pWindow != nullptr);
		//
		//	if (codepoint < 32 || (codepoint > 126 && codepoint < 160))
		//		return;
		//
		//	/*if (!pWindow->lockKeyMods)
		//		mods &= ~(GE_MOD_CAPS_LOCK | GE_MOD_NUM_LOCK);*/
		//
		//	if (pWindow->callbacks.charmods)
		//		pWindow->callbacks.charmods((GLFWwindow*)window, codepoint, mods);
		//
		//	if (plain)
		//	{
		//		if (pWindow->callbacks.character)
		//			pWindow->callbacks.character((GLFWwindow*)window, codepoint);
		//	}
		//}

		// Notifies shared code of a mouse button click event
		//
		void InputMouseButton(GE_Window* pWindow, int32_t button, int32_t action, int32_t mods)
		{
			assert(pWindow != nullptr);

			if (button < 0 || button > GE_MOUSE_BUTTON_LAST)
				return;

			/*if (!window->lockKeyMods)
				mods &= ~(GLFW_MOD_CAPS_LOCK | GLFW_MOD_NUM_LOCK);*/

			if (action == GE_MOUSE_BUTTON_RELEASE && pWindow->stickyMouseButtons)
			{
				pWindow->mouseButtons[button] = GE_STICKY_MOUSE_BTN;
			}
			else
			{
				pWindow->mouseButtons[button] = static_cast<char_t>(action);
			}

			if (pWindow->callbacks.onMouseButton)
				pWindow->callbacks.onMouseButton(pWindow, button, action, mods);
		}

		// Notifies shared code of a scroll event
		//
		void InputMouseScroll(GE_Window* pWindow, float64_t xOffset, float64_t yOffset)
		{
			assert(pWindow != nullptr);

			if (pWindow->callbacks.onMouseScroll)
				pWindow->callbacks.onMouseScroll(pWindow, xOffset, yOffset);
		}

		// Notifies shared code of a cursor motion event
		// The position is specified in content area relative screen coordinates
		//
		void InputMouseMove(GE_Window* pWindow, int32_t xPos, int32_t yPos)
		{
			assert(pWindow != nullptr);

			//if (pWindow->virtualCursorPosX == xPos && pWindow->virtualCursorPosY == yPos)
			//	return;

			//pWindow->virtualCursorPosX = xPos;
			//pWindow->virtualCursorPosY = yPos;

			if (pWindow->callbacks.onMouseMove)
				pWindow->callbacks.onMouseMove(pWindow, xPos, yPos);
		}

		// Notifies shared code of a cursor enter/leave event
		//
		void InputMouseEnterWindow(GE_Window* pWindow, bool_t isEntered)
		{
			assert(pWindow != nullptr);

			if (pWindow->callbacks.onMouseEnterWindow)
				pWindow->callbacks.onMouseEnterWindow(pWindow, isEntered);
		}

		////////////////////////////////

		void InputWindowFocus(GE_Window* pWindow, bool_t isFocused)
		{
			assert(pWindow != nullptr);

			if (pWindow->callbacks.onWindowFocus)
				pWindow->callbacks.onWindowFocus(pWindow, isFocused);

			if (false == isFocused)
			{
				int32_t key, button;

				for (key = 0; key <= GE_KEY_LAST; key++)
				{
					if (pWindow->keys[key] == GE_KEY_PRESS)
					{
						const int32_t scancode = GetKeyScancode_Platform(key);
						InputKey(pWindow, key, scancode, GE_KEY_RELEASE, 0);
					}
				}

				for (button = 0; button <= GE_MOUSE_BUTTON_LAST; button++)
				{
					if (pWindow->mouseButtons[button] == GE_MOUSE_BUTTON_PRESS)
						InputMouseButton(pWindow, button, GE_MOUSE_BUTTON_RELEASE, 0);
				}
			}
		}

		// Notifies shared code that a window has moved
		// The position is specified in content area relative screen coordinates
		//
		void InputWindowPos(GE_Window* pWindow, int32_t xPos, int32_t yPos)
		{
			assert(pWindow != nullptr);

			if (pWindow->callbacks.onWindowPos)
				pWindow->callbacks.onWindowPos(pWindow, xPos, yPos);
		}

		// Notifies shared code that a window has been resized
		// The size is specified in screen coordinates
		//
		void InputWindowSize(GE_Window* pWindow, uint32_t width, uint32_t height)
		{
			assert(pWindow != nullptr);

			if (pWindow->callbacks.onWindowSize)
				pWindow->callbacks.onWindowSize(pWindow, width, height);
		}

		// Notifies shared code that a window has been iconified or restored
		//
		void InputWindowMinimize(GE_Window* pWindow, bool_t isMinimized)
		{
			assert(pWindow != nullptr);

			if (pWindow->callbacks.onWindowMinimize)
				pWindow->callbacks.onWindowMinimize(pWindow, isMinimized);
		}

		// Notifies shared code that a window has been maximized or restored
		//
		void InputWindowMaximize(GE_Window* pWindow, bool_t isMaximized)
		{
			assert(pWindow != nullptr);

			if (pWindow->callbacks.onWindowMaximize)
				pWindow->callbacks.onWindowMaximize(pWindow, isMaximized);
		}

		// Notifies shared code that the window contents needs updating
		//
		void InputWindowRefresh(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			if (pWindow->callbacks.onWindowRefresh)
				pWindow->callbacks.onWindowRefresh(pWindow);
		}

		void InputWindowCloseRequest(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			pWindow->shouldClose = true;

			if (pWindow->callbacks.onWindowClose)
				pWindow->callbacks.onWindowClose(pWindow);
		}

		///////////////////////

		void GetCursorPos(GE_Window* pWindow, int32_t* pXPos, int32_t* pYPos)
		{
			GetCursorPos_Platform(pWindow, pXPos, pYPos);
		}

		void SetCursorPos(GE_Window* pWindow, int32_t xPos, int32_t yPos)
		{
			SetCursorPos_Platform(pWindow, xPos, yPos);
		}

		void ShowCursor()
		{
			ShowCursor_Platform();
		}

		void HideCursor()
		{
			HideCursor_Platform();
		}

		///////////

		void PollEvents(GE_Window* pWindow)
		{
			PollEvents_Platform(pWindow);
		}

		void WaitEvents(GE_Window* pWindow)
		{
			WaitEvents_Platform(pWindow);
		}

		void WaitEventsTimeout(GE_Window* pWindow, float64_t timeout)
		{
			WaitEventsTimeout_Platform(pWindow, timeout);
		}

		///////////////////////////

		void RegisterWindowFocusCallback(GE_Window* pWindow, GE_InputWindowFocusFN onWindowFocus)
		{
			assert(pWindow != nullptr);

			pWindow->callbacks.onWindowFocus = onWindowFocus;
		}

		void RegisterWindowPosCallback(GE_Window* pWindow, GE_InputWindowPosFN onWindowPos)
		{
			assert(pWindow != nullptr);

			pWindow->callbacks.onWindowPos = onWindowPos;
		}

		void RegisterWindowSizeCallback(GE_Window* pWindow, GE_InputWindowSizeFN onWindowSize)
		{
			assert(pWindow != nullptr);

			pWindow->callbacks.onWindowSize = onWindowSize;
		}

		void RegisterWindowMinimizeCallback(GE_Window* pWindow, GE_InputWindowMinimizeFN onWindowMinimize)
		{
			assert(pWindow != nullptr);

			pWindow->callbacks.onWindowMinimize = onWindowMinimize;
		}

		void RegisterWindowMaximizeCallback(GE_Window* pWindow, GE_InputWindowMaximizeFN onWindowMaximize)
		{

			assert(pWindow != nullptr);

			pWindow->callbacks.onWindowMaximize = onWindowMaximize;
		}

		void RegisterWindowRefreshCallback(GE_Window* pWindow, GE_WindowRefreshFN onWindowRefresh)
		{
			assert(pWindow != nullptr);

			pWindow->callbacks.onWindowRefresh = onWindowRefresh;
		}

		void RegisterWindowCloseCallback(GE_Window* pWindow, GE_WindowCloseFN onWindowClose)
		{
			assert(pWindow != nullptr);

			pWindow->callbacks.onWindowClose = onWindowClose;
		}

		void RegisterMouseButtonCallback(GE_Window* pWindow, GE_MouseButtonFN onMouseButton)
		{
			assert(pWindow != nullptr);

			pWindow->callbacks.onMouseButton = onMouseButton;
		}

		void RegisterMouseMoveCallback(GE_Window* pWindow, GE_MouseMoveFN onMouseMove)
		{
			assert(pWindow != nullptr);

			pWindow->callbacks.onMouseMove = onMouseMove;
		}

		void RegisterMouseScrollCallback(GE_Window* pWindow, GE_MouseScollFN onMouseScroll)
		{
			assert(pWindow != nullptr);

			pWindow->callbacks.onMouseScroll = onMouseScroll;
		}

		void RegisterMouseEnterWindowCallback(GE_Window* pWindow, GE_MouseEnterWindowFN onMouseEnterWindow)
		{
			assert(pWindow != nullptr);

			pWindow->callbacks.onMouseEnterWindow = onMouseEnterWindow;
		}

		void RegisterKeyCallback(GE_Window* pWindow, GE_KeyFN onKey)
		{
			assert(pWindow != nullptr);

			pWindow->callbacks.onKey = onKey;
		}

		////

		void UnregisterWindowFocusCallback(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			pWindow->callbacks.onWindowFocus = nullptr;
		}

		void UnregisterWindowPosCallback(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			pWindow->callbacks.onWindowPos = nullptr;
		}
		void UnregisterWindowSizeCallback(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			pWindow->callbacks.onWindowSize = nullptr;
		}

		void UnregisterWindowMinimizeCallback(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			pWindow->callbacks.onWindowMinimize = nullptr;
		}

		void UnregisterWindowMaximizeCallback(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			pWindow->callbacks.onWindowMaximize = nullptr;
		}
		void UnregisterWindowRefreshCallback(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			pWindow->callbacks.onWindowRefresh = nullptr;
		}

		void UnregisterWindowCloseCallback(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			pWindow->callbacks.onWindowClose = nullptr;
		}

		void UnregisterMouseButtonCallback(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			pWindow->callbacks.onMouseButton = nullptr;
		}
		void UnregisterMouseMoveCallback(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			pWindow->callbacks.onMouseMove = nullptr;
		}

		void UnregisterMouseScrollCallback(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			pWindow->callbacks.onMouseScroll = nullptr;
		}

		void UnregisterMouseEnterWindowCallback(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			pWindow->callbacks.onMouseEnterWindow = nullptr;
		}

		void UnregisterKeyCallback(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			pWindow->callbacks.onKey = nullptr;
		}

		void UnregisterAllCallbacks(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			// Clear all callbacks to avoid exposing a half torn-down window object
#ifdef USE_FUNCTOR
			if (pWindow->callbacks.onWindowFocus)
				pWindow->callbacks.onWindowFocus = nullptr;
			if (pWindow->callbacks.onWindowPos)
				pWindow->callbacks.onWindowPos = nullptr;
			if (pWindow->callbacks.onWindowSize)
				pWindow->callbacks.onWindowSize = nullptr;
			if (pWindow->callbacks.onWindowMinimize)
				pWindow->callbacks.onWindowMinimize = nullptr;
			if (pWindow->callbacks.onWindowMaximize)
				pWindow->callbacks.onWindowMaximize = nullptr;
			if (pWindow->callbacks.onWindowRefresh)
				pWindow->callbacks.onWindowRefresh = nullptr;
			if (pWindow->callbacks.onWindowClose)
				pWindow->callbacks.onWindowClose = nullptr;
			if (pWindow->callbacks.onMouseButton)
				pWindow->callbacks.onMouseButton = nullptr;
			if (pWindow->callbacks.onMouseMove)
				pWindow->callbacks.onMouseMove = nullptr;
			if (pWindow->callbacks.onMouseScroll)
				pWindow->callbacks.onMouseScroll = nullptr;
			if (pWindow->callbacks.onMouseEnterWindow)
				pWindow->callbacks.onMouseEnterWindow = nullptr;
			if (pWindow->callbacks.onKey)
				pWindow->callbacks.onKey = nullptr;
#else
			memset(&pWindow->callbacks, 0, sizeof(pWindow->callbacks));
#endif
		}

		void ResetWindowData(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			pWindow->pTitle = nullptr;
			pWindow->flags = 0;
			pWindow->width = 0;
			pWindow->height = 0;

			pWindow->isMinimized = 0;
			pWindow->isMaximized = 0;
			pWindow->autoMinimize = false;
			pWindow->shouldClose = false;

			pWindow->stickyKeys = false;
			pWindow->stickyMouseButtons = false;

			memset(&pWindow->keys, 0, sizeof(pWindow->keys));
			memset(&pWindow->mouseButtons, 0, sizeof(pWindow->mouseButtons));

			UnregisterAllCallbacks(pWindow);

			ResetWindowData_Platform(pWindow);
		}

		// Enable mecro to solve name clash
#define CreateWindow
	}
}