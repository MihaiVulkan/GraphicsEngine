#ifndef FOUNDATION_PLATFORM_PLATFORM_TYPES_HPP
#define FOUNDATION_PLATFORM_PLATFORM_TYPES_HPP

#include "Foundation/TypeDefs.hpp"

#if defined(_WIN32)
#include "Win32/Win32Types.hpp"
#else
// other platforms
#endif

#include "InputDef.hpp"
#include <functional>

namespace GraphicsEngine
{
	namespace Platform
	{
		// TYPEDEFS AND TYPES WHICH CAN ALSO BE USED PUBLICLY ! //
		class GE_Window;

		// Callbacks
	// Window
		typedef std::function<void(GE_Window* pWindow, bool_t isFocused)> GE_InputWindowFocusFN;
		typedef std::function<void(GE_Window* pWindow, int32_t xPos, int32_t yPos)> GE_InputWindowPosFN;
		typedef std::function<void(GE_Window* pWindow, uint32_t width, uint32_t height)> GE_InputWindowSizeFN;
		typedef std::function<void(GE_Window* pWindow, bool_t isMinimized)> GE_InputWindowMinimizeFN;
		typedef std::function<void(GE_Window* pWindow, bool_t isMaximized)> GE_InputWindowMaximizeFN;
		typedef std::function<void(GE_Window* pWindow)> GE_WindowRefreshFN;
		typedef std::function<void(GE_Window* pWindow)> GE_WindowCloseFN;

		// Input
		typedef std::function<void(GE_Window* pWindow, int32_t button, int32_t action, int32_t mods)> GE_MouseButtonFN;
		typedef std::function<void(GE_Window* pWindow, int32_t xPos, int32_t yPos)> GE_MouseMoveFN;
		typedef std::function<void(GE_Window* pWindow, float64_t xOffset, float64_t yOffset)> GE_MouseScollFN;
		typedef std::function<void(GE_Window* pWindow, bool_t isEntered)> GE_MouseEnterWindowFN;

		typedef std::function<void(GE_Window* pWindow, int32_t key, int32_t scancode, int32_t action, int32_t mods)> GE_KeyFN;

		// Flags

		// Window
		typedef enum GE_WindowFlags
		{
			GE_WINDOW_FULLSCREEN = 0x00000001,         /**< fullscreen window */
			GE_WINDOW_VISIBLE = 0x00000004,            /**< window is visible */
		//	GE_WINDOW_HIDDEN = 0x00000008,             /**< window is not visible */
			GE_WINDOW_BORDERLESS = 0x00000010,         /**< no window decoration */
			GE_WINDOW_RESIZABLE = 0x00000022,		   /**< window can be resized */
			GE_WINDOW_MINIMIZED = 0x00000040,          /**< window is minimized */
			GE_WINDOW_MAXIMIZED = 0x00000080,          /**< window is maximized */
			GE_WINDOW_INPUT_GRABBED = 0x00000100,      /**< window has grabbed input focus */
		//	GE_WINDOW_INPUT_FOCUS = 0x00000200,        /**< window has input focus */
		//	GE_WINDOW_MOUSE_FOCUS = 0x00000400,        /**< window has mouse focus */

		//	GE_WINDOW_MOUSE_CAPTURE = 0x00004000,      /**< window has mouse captured (unrelated to INPUT_GRABBED) */
		//	GE_WINDOW_ALWAYS_ON_TOP = 0x00008000,      /**< window should always be above others */

		//	GE_WINDOW_VULKAN = 0x10000000       /**< window usable for Vulkan surface */
		} GE_WindowFlags;

		// Structs
		class GE_KeyData
		{
		public:
			GE_PLATFORM_KEY_STATE;
		};

		class GE_Window
		{
		public:
			char_t* pTitle;
			uint32_t flags;
			uint32_t width;
			uint32_t height;

			bool_t isMinimized;
			bool_t isMaximized;
			bool_t autoMinimize;
			bool_t shouldClose;

			bool_t           stickyKeys;
			bool_t           stickyMouseButtons;

			char_t           mouseButtons[GE_MOUSE_BUTTON_LAST + 1];
			char_t           keys[GE_KEY_LAST + 1];

			struct {
				GE_InputWindowFocusFN onWindowFocus;
				GE_InputWindowPosFN onWindowPos;
				GE_InputWindowSizeFN onWindowSize;
				GE_InputWindowMinimizeFN onWindowMinimize;
				GE_InputWindowMaximizeFN onWindowMaximize;
				GE_WindowRefreshFN onWindowRefresh;
				GE_WindowCloseFN onWindowClose;

				GE_MouseButtonFN onMouseButton;
				GE_MouseMoveFN onMouseMove;
				GE_MouseScollFN onMouseScroll;
				GE_MouseEnterWindowFN onMouseEnterWindow;
				GE_KeyFN onKey;
			} callbacks;

			GE_PLATFORM_WINDOW_STATE;
		};
	}
}

#endif // FOUNDATION_PLATFORM_PLATFORM_TYPES_HPP