#ifndef FOUNDATION_PLATFORM_PLATFORM_INTERNAL_HPP
#define FOUNDATION_PLATFORM_PLATFORM_INTERNAL_HPP

#include "Foundation/Object.hpp"
#include "Foundation/Platform/InputMacros.hpp"
#include <functional>
#include <string>

namespace GraphicsEngine
{
	namespace Platform
	{
		class KeyMap : public Object
		{
			GE_RTTI(GraphicsEngine::Platform::KeyMap)

		public:
			KeyMap();
			virtual ~KeyMap();

			virtual int32_t GetKeyScancode(int32_t key) { return -1; }

		private:
			NO_COPY_NO_MOVE(KeyMap)
		};

		class Window : public Object
		{
			GE_RTTI(GraphicsEngine::Platform::Window)

		public:
			// Flags
			typedef enum GE_WindowFlags
			{
				GE_WF_FULLSCREEN = 0x00000001,         /**< fullscreen window */
				GE_WF_VISIBLE = 0x00000004,            /**< window is visible */
			//	GE_WF_HIDDEN = 0x00000008,             /**< window is not visible */
				GE_WF_BORDERLESS = 0x00000010,         /**< no window decoration */
				GE_WF_RESIZABLE = 0x00000022,		   /**< window can be resized */
				GE_WF_MINIMIZED = 0x00000040,          /**< window is minimized */
				GE_WF_MAXIMIZED = 0x00000080,          /**< window is maximized */
				GE_WF_INPUT_GRABBED = 0x00000100,      /**< window has grabbed input focus */
				GE_WF_COUNT
			} GE_WindowFlags;

			typedef struct WindowState {
				std::string title;
				uint32_t flags;
				uint32_t width;
				uint32_t height;

				bool_t isMinimized;
				bool_t isMaximized;
				bool_t shouldClose;
				bool_t isMouseInsideWindow;

				char_t mouseButtons[GE_MOUSE_BUTTON_LAST + 1];
				char_t keys[GE_KEY_LAST + 1];
			} WindowState;

			// Callbacks
			// Window
			typedef std::function<void(bool_t isFocused)> InputWindowFocusFN;
			typedef std::function<void(int32_t xPos, int32_t yPos)> InputWindowPosFN;
			typedef std::function<void(uint32_t width, uint32_t height)> InputWindowSizeFN;
			typedef std::function<void(bool_t isMinimized)> InputWindowMinimizeFN;
			typedef std::function<void(bool_t isMaximized)> InputWindowMaximizeFN;
			typedef std::function<void()> WindowRefreshFN;
			typedef std::function<void()> WindowCloseFN;

			// Input
			typedef std::function<void(int32_t button, int32_t action)> MouseButtonFN;
			typedef std::function<void(int32_t xPos, int32_t yPos)> MouseMoveFN;
			typedef std::function<void(float64_t xOffset, float64_t yOffset)> MouseScollFN;
			typedef std::function<void(bool_t isEntered)> MouseEnterWindowFN;

			typedef std::function<void(int32_t key, int32_t scancode, int32_t action)> KeyFN;


			Window();
			virtual ~Window();

			////////////////// WINDOW API //////////////////
			virtual void GetWindowPos(int32_t* pXPos, int32_t* pYPos) {}
			virtual void SetWindowPos(int32_t xPos, int32_t yPos) {}
			virtual void GetWindowSize(uint32_t* pWidth, uint32_t* pHeight) {}
			virtual void SetWindowSize(uint32_t width, uint32_t height) {}
			virtual const char_t* GetWindowTitle();
			virtual void SetWindowTitle(const std::string& title) {}

			virtual void MinimizeWindow() {}
			virtual void MaximizeWindow() {}
			virtual void RestoreWindow() {}
			virtual void ShowWindow() {}
			virtual void HideWindow() {}
			virtual void FocusWindow() {}

			virtual bool_t IsWindowVisible() { return false; }
			virtual bool_t IsWindowMinimized() { return false; }
			virtual bool_t IsWindowMaximized() { return false; }
			virtual bool_t IsWindowFocused() { return false; }

			virtual bool_t GetShouldWindowClose();
			virtual void SetShouldWindowClose(bool_t shouldClose);
			virtual bool_t ShouldWindowClose();

			//////////// MOUSE CURSOR API ///////////
			virtual void GetCursorPos(int32_t* pXPos, int32_t* pYPos) {}
			virtual void SetCursorPos(int32_t xPos, int32_t yPos) {}
			virtual void ShowCursor() {}
			virtual void HideCursor() {}

			/////////////// EVENTS API /////////////////
			virtual void PollEvents() {}
			virtual void WaitEvents() {}
			virtual void WaitEventsTimeout(float64_t timeout) {}


			//////////////////////
			// Register
			void RegisterWindowFocusCallback(Window::InputWindowFocusFN onWindowFocus);
			void RegisterWindowPosCallback(Window::InputWindowPosFN onWindowPos);
			void RegisterWindowSizeCallback(Window::InputWindowSizeFN onWindowSize);
			void RegisterWindowMinimizeCallback(Window::InputWindowMinimizeFN onWindowMinimize);
			void RegisterWindowMaximizeCallback(Window::InputWindowMaximizeFN onWindowMaximize);
			void RegisterWindowRefreshCallback(Window::WindowRefreshFN onWindowRefresh);
			void RegisterWindowCloseCallback(Window::WindowCloseFN onWindowClose);

			void RegisterMouseButtonCallback(Window::MouseButtonFN onMouseButton);
			void RegisterMouseMoveCallback(Window::MouseMoveFN onMouseMove);
			void RegisterMouseScrollCallback(Window::MouseScollFN onMouseScroll);
			void RegisterMouseEnterWindowCallback(Window::MouseEnterWindowFN onMouseEnterWindow);
			void RegisterKeyCallback(Window::KeyFN onKey);

			// Unregister
			void UnregisterWindowFocusCallback();
			void UnregisterWindowPosCallback();
			void UnregisterWindowSizeCallback();
			void UnregisterWindowMinimizeCallback();
			void UnregisterWindowMaximizeCallback();
			void UnregisterWindowRefreshCallback();
			void UnregisterWindowCloseCallback();

			void UnregisterMouseButtonCallback();
			void UnregisterMouseMoveCallback();
			void UnregisterMouseScrollCallback();
			void UnregisterMouseEnterWindowCallback();
			void UnregisterKeyCallback();
			void UnregisterAllCallbacks();

			////////////////////////////////

			///////////// NON PLATFORM API ////////////////
			/////////////////// INPUT API ////////////////
			void InputWindowFocus(bool_t isFocused);
			void InputWindowPos(int32_t xPos, int32_t yPos);
			void InputWindowSize(uint32_t width, uint32_t height);
			void InputWindowMinimize(bool_t isMinimized);
			void InputWindowMaximize(bool_t isMaximized);
			void InputWindowRefresh();
			void InputWindowCloseRequest();

			void InputKey(int32_t key, int32_t scancode, int32_t action);
			void InputMouseButton(int32_t button, int32_t action);
			void InputMouseScroll(float64_t xOffset, float64_t yOffset);
			void InputMouseMove(int32_t xPos, int32_t yPos);
			void InputMouseEnterWindow(bool_t isEntered);

			////////////////////////////////

			void SetIsMinimized(bool_t value);
			void SetIsMaximized(bool_t value);
			void SetIsMouseInsideWindow(bool_t value);

			virtual int32_t GetKeyScancodeNative(int32_t key) { return -1; };

			const Window::WindowState& GetState() const;


		protected:
			void UpdateWindowFlags(uint32_t flags);

			virtual void ResetWindowData();

			WindowState mState;

			struct {
				InputWindowFocusFN onWindowFocus;
				InputWindowPosFN onWindowPos;
				InputWindowSizeFN onWindowSize;
				InputWindowMinimizeFN onWindowMinimize;
				InputWindowMaximizeFN onWindowMaximize;
				WindowRefreshFN onWindowRefresh;
				WindowCloseFN onWindowClose;

				MouseButtonFN onMouseButton;
				MouseMoveFN onMouseMove;
				MouseScollFN onMouseScroll;
				MouseEnterWindowFN onMouseEnterWindow;
				KeyFN onKey;
			} mCallbacks;

		private:
			NO_COPY_NO_MOVE(Window)

		};
	}
}

#endif // FOUNDATION_PLATFORM_PLATFORM_INTERNAL_HPP