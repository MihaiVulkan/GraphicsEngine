
#ifndef FOUNDATION_PLATFORM_PLATFORM_INTERNAL_HPP
#define FOUNDATION_PLATFORM_PLATFORM_INTERNAL_HPP

#include "PlatformTypes.hpp"

namespace GraphicsEngine
{
	namespace Platform
	{
		// Platform API - common for all platforms
		///// INIT ///////
		void Init_Platform();
		void Terminate_Platform();

		///////////// MOUSE CURSOR API ///////////
		void GetCursorPos_Platform(GE_Window* pWindow, int32_t* pXPos, int32_t* pYPos);
		void SetCursorPos_Platform(GE_Window* pWindow, int32_t xPos, int32_t yPos);
		void SetCursorMode_Platform(GE_Window* pWindow, int32_t mode);
		void ShowCursor_Platform();
		void HideCursor_Platform();

		////////////////// WINDOW API //////////////////
		bool CreateWindow_Platform(GE_Window* pWindow, const char_t* pTitle, int32_t xPos, int32_t yPos, uint32_t width, uint32_t height, uint32_t flags);
		void DestroyWindow_Platform(GE_Window* pWindow);
		void ResetWindowData_Platform(GE_Window* pWindow);
		void ResetWindowData(GE_Window* pWindow);

		void GetWindowSize_Platform(GE_Window* pWindow, uint32_t* pWidth, uint32_t* pHeight);
		void SetWindowSize_Platform(GE_Window* pWindow, uint32_t width, uint32_t height);
		void GetWindowPos_Platform(GE_Window* pWindow, int32_t* pXPos, int32_t* pYPos);
		void SetWindowPos_Platform(GE_Window* pWindow, int32_t xPos, int32_t yPos);
		void GetWindowTitle_Platform(GE_Window* pWindow, const char_t* pTitle);
		void SetWindowTitle_Platform(GE_Window* pWindow, const char_t* pTitle);

		void MinimizeWindow_Platform(GE_Window* pWindow);
		void MaximizeWindow_Platform(GE_Window* pWindow);
		void RestoreWindow_Platform(GE_Window* pWindow);
		void ShowWindow_Platform(GE_Window* window);
		void HideWindow_Platform(GE_Window* window);
		void FocusWindow_Platform(GE_Window* pWindow);

		bool IsWindowVisible_Platform(GE_Window* pWindow);
		bool IsWindowMinimized_Platform(GE_Window* pWindow);
		bool IsWindowMaximized_Platform(GE_Window* pWindow);
		bool IsWindowFocused_Platform(GE_Window* pWindow);

		/////////////// EVENTS API /////////////////
		void PollEvents_Platform(GE_Window* pWindow);
		void WaitEvents_Platform(GE_Window* pWindow);
		void WaitEventsTimeout_Platform(GE_Window* pWindow, bfloat64_t timeout);


		///////////// NON PLATFORM API ////////////////
		/////////////////// INPUT API ////////////////
		void InputWindowFocus(GE_Window* pWindow, bool_t isFocused);
		void InputWindowPos(GE_Window* pWindow, int32_t xPos, int32_t yPos);
		void InputWindowSize(GE_Window* pWindow, uint32_t width, uint32_t height);
		void InputWindowMinimize(GE_Window* pWindow, bool_t isMinimized);
		void InputWindowMaximize(GE_Window* pWindow, bool_t isMaximized);
		void InputWindowRefresh(GE_Window* pWindow);
		void InputWindowCloseRequest(GE_Window* pWindow);


		void InputKey(GE_Window* pWindow, int32_t key, int32_t scancode, int32_t action, int32_t mods);
		void InputMouseButton(GE_Window* pWindow, int32_t button, int32_t action, int32_t mods);
		void InputMouseScroll(GE_Window* pWindow, bfloat64_t xOffset, bfloat64_t yOffset);
		void InputMouseMove(GE_Window* pWindow, int32_t xPos, int32_t yPos);
		void InputMouseEnterWindow(GE_Window* pWindow, bool_t isEntered);

		////////////////

		const char_t* GetScancodeName_Platform(int32_t scancode);
		int32_t GetKeyScancode_Platform(int32_t key);

		char_t* DuplicateStr(const char_t* pStr);

		////////////////////////////////////////////
	}
}

#endif // FOUNDATION_PLATFORM_PLATFORM_INTERNAL_HPP