#ifndef FOUNDATION_PLATFORM_PLATFORM_HPP
#define FOUNDATION_PLATFORM_PLATFORM_HPP

#include "PlatformTypes.hpp"

// Disable mecro to solve name clash
#ifdef CreateWindow
#undef CreateWindow
#endif 

namespace GraphicsEngine
{
	namespace Platform
	{

		//////////////// INIT API //////////////
		void Init();
		void Terminate();

		////////////////// WINDOW API //////////////////////
		// By default the window is visible, resizable and focused
		GE_Window* CreateWindow(const char_t* pTitle, int32_t xPos, int32_t yPos, int32_t width, int32_t height,
			uint32_t flags = Platform::GE_WindowFlags::GE_WINDOW_VISIBLE | Platform::GE_WindowFlags::GE_WINDOW_RESIZABLE | Platform::GE_WindowFlags::GE_WINDOW_INPUT_GRABBED);
		void UpdateWindowFlags(GE_Window* pWindow, uint32_t flags);
		void DestroyWindow(GE_Window* pWindow);

		void GetWindowPos(GE_Window* pWindow, int32_t* pXPos, int32_t* pYPos);
		void SetWindowPos(GE_Window* pWindow, int32_t xPos, int32_t yPos);
		void GetWindowSize(GE_Window* pWindow, uint32_t* pWidth, uint32_t* pHeight);
		void SetWindowSize(GE_Window* pWindow, uint32_t width, uint32_t height);
		void GetWindowTitle(GE_Window* pWindow, const char_t* pTitle);
		void SetWindowTitle(GE_Window* pWindow, const char_t* pTitle);

		void MinimizeWindow(GE_Window* pWindow);
		void MaximizeWindow(GE_Window* pWindow);
		void RestoreWindow(GE_Window* pWindow);
		void ShowWindow(GE_Window* pWindow);
		void HideWindow(GE_Window* pWindow);
		void FocusWindow(GE_Window* pWindow);


		bool IsWindowVisible(GE_Window* pWindow);
		bool IsWindowMinimized(GE_Window* pWindow);
		bool IsWindowMaximized(GE_Window* pWindow);
		bool IsWindowFocused(GE_Window* pWindow);

		bool GetShouldWindowClose(GE_Window* pWindow);
		void SetShouldWindowClose(GE_Window* pWindow, bool_t shouldClose);
		bool ShouldWindowClose(GE_Window* pWindow);

		/////////////

		void GetCursorPos(GE_Window* pWindow, int32_t* pXPos, int32_t* pYPos);
		void SetCursorPos(GE_Window* pWindow, int32_t xPos, int32_t yPos);
		//void SetCursorMode(GE_Window* pWindow, int mode);

		void ShowCursor();
		void HideCursor();

		///////////

		void PollEvents(GE_Window* pWindow);
		void WaitEvents(GE_Window* pWindow);
		void WaitEventsTimeout(GE_Window* pWindow, bfloat64_t timeout);


		//////////////////////
		// Register
		void RegisterWindowFocusCallback(GE_Window* pWindow, GE_InputWindowFocusFN onWindowFocus);
		void RegisterWindowPosCallback(GE_Window* pWindow, GE_InputWindowPosFN onWindowPos);
		void RegisterWindowSizeCallback(GE_Window* pWindow, GE_InputWindowSizeFN onWindowSize);
		void RegisterWindowMinimizeCallback(GE_Window* pWindow, GE_InputWindowMinimizeFN onWindowMinimize);
		void RegisterWindowMaximizeCallback(GE_Window* pWindow, GE_InputWindowMaximizeFN onWindowMaximize);
		void RegisterWindowRefreshCallback(GE_Window* pWindow, GE_WindowRefreshFN onWindowRefresh);
		void RegisterWindowCloseCallback(GE_Window* pWindow, GE_WindowCloseFN onWindowClose);

		void RegisterMouseButtonCallback(GE_Window* pWindow, GE_MouseButtonFN onMouseButton);
		void RegisterMouseMoveCallback(GE_Window* pWindow, GE_MouseMoveFN onMouseMove);
		void RegisterMouseScrollCallback(GE_Window* pWindow, GE_MouseScollFN onMouseScroll);
		void RegisterMouseEnterWindowCallback(GE_Window* pWindow, GE_MouseEnterWindowFN onMouseEnterWindow);
		void RegisterKeyCallback(GE_Window* pWindow, GE_KeyFN onKey);

		// Unregister
		void UnregisterWindowFocusCallback(GE_Window* pWindow);
		void UnregisterWindowPosCallback(GE_Window* pWindow);
		void UnregisterWindowSizeCallback(GE_Window* pWindow);
		void UnregisterWindowMinimizeCallback(GE_Window* pWindow);
		void UnregisterWindowMaximizeCallback(GE_Window* pWindow);
		void UnregisterWindowRefreshCallback(GE_Window* pWindow);
		void UnregisterWindowCloseCallback(GE_Window* pWindow);

		void UnregisterMouseButtonCallback(GE_Window* pWindow);
		void UnregisterMouseMoveCallback(GE_Window* pWindow);
		void UnregisterMouseScrollCallback(GE_Window* pWindow);
		void UnregisterMouseEnterWindowCallback(GE_Window* pWindow);
		void UnregisterKeyCallback(GE_Window* pWindow);
		void UnregisterAllCallbacks(GE_Window* pWindow);
	}
}


#endif // FOUNDATION_PLATFORM_PLATFORM_HPP