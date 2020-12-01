#ifndef FOUNDATION_PLATFORM_WIN32_TYPES_HPP
#define FOUNDATION_PLATFORM_WIN32_TYPES_HPP

#include "Foundation/TypeDefs.hpp"
#include "Foundation/Platform/InputDef.hpp"
#include <windows.h>

#define GE_PLATFORM_WINDOW_STATE GE_WindowWin32 win32
#define GE_PLATFORM_KEY_STATE GE_KeyWin32 win32

// Window
#if !defined(GE_WNDCLASSNAME)
#define GE_WNDCLASSNAME L"GE_WND_CN"
#endif

#define GE_WindowProperty L"GE_W"

// Window Styles
#define GE_WINDOW_STYLE_BASIC         (WS_CLIPSIBLINGS | WS_CLIPCHILDREN)
#define GE_WINDOW_STYLE_FULLSCREEN    (WS_POPUP)
#define GE_WINDOW_STYLE_BORDERLESS    (WS_POPUP)
#define GE_WINDOW_STYLE_NORMAL        (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)
#define GE_WINDOW_STYLE_RESIZABLE     (WS_SIZEBOX | WS_MAXIMIZEBOX)
//#define GE_WINDOW_STYLE_MASK		  (GE_WINDOW_STYLE_FULLSCREEN | GE_WINDOW_STYLE_BORDERLESS | GE_WINDOW_STYLE_NORMAL | GE_WINDOW_STYLE_RESIZABLE)

namespace GraphicsEngine
{
	namespace Platform
	{
		typedef struct GE_KeyWin32
		{
			int16_t           keycodes[512];
			int16_t           scancodes[GE_KEY_LAST + 1];
			char_t            keynames[GE_KEY_LAST + 1][5];
		} GE_KeyWin32;

		typedef struct GE_WindowWin32
		{
			HWND handle;
			HINSTANCE instance;

			bool_t isMouseInsideWindow;

		} GE_WindowWin32;
	}
}

#endif // FOUNDATION_PLATFORM_WIN32_TYPES_HPP