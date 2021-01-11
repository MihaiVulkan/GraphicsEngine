#ifndef FOUNDATION_PLATFORM_WIN32_PLATFORM_HPP
#define FOUNDATION_PLATFORM_WIN32_PLATFORM_HPP

#include "Foundation/Platform/PlatformInternal.hpp"
#include <windows.h>

namespace GraphicsEngine
{
	namespace Platform
	{
		class KeyMapWin32 : public KeyMap
		{
			GE_RTTI(GraphicsEngine::Platform::KeyMapWin32)

		public:
			static constexpr const uint32_t k_KeyCodeCount = 512;

			typedef struct PlatformData
			{
				int16_t           keycodes[k_KeyCodeCount];
				int16_t           scancodes[GE_KEY_LAST + 1];
			} PlatformData;

			KeyMapWin32();
			virtual ~KeyMapWin32();

			virtual int32_t GetKeyScancode(int32_t key) override;

			const KeyMapWin32::PlatformData& GetPlatformData() const;

		private:
			NO_COPY_NO_MOVE(KeyMapWin32)

			void CreateTable();
			void DestroyTable();

			PlatformData mPlatformData;
		};

		class WindowWin32 : public Window
		{
			GE_RTTI(GraphicsEngine::Platform::WindowWin32)

		public:
			typedef struct PlatformData
			{
				HWND handle;
				HINSTANCE instance;
			} PlatformData;


			WindowWin32();
			// NOTE! By default the window is visible, resizable and focused
			explicit WindowWin32(const std::string& title, uint32_t width, uint32_t height, 
				uint32_t flags = Window::GE_WindowFlags::GE_WF_VISIBLE | Window::GE_WindowFlags::GE_WF_RESIZABLE | Window::GE_WindowFlags::GE_WF_INPUT_GRABBED);
			virtual ~WindowWin32();

			////////////////// WINDOW API //////////////////
			virtual void GetWindowSize(uint32_t* pWidth, uint32_t* pHeight) override;
			virtual void SetWindowSize(uint32_t width, uint32_t height) override;
			virtual void GetWindowPos(int32_t* pXPos, int32_t* pYPos) override;
			virtual void SetWindowPos(int32_t xPos, int32_t yPos) override;
			virtual void SetWindowTitle(const std::string& title) override;

			virtual void MinimizeWindow() override;
			virtual void MaximizeWindow() override;
			virtual void RestoreWindow() override;
			virtual void ShowWindow() override;
			virtual void HideWindow() override;
			virtual void FocusWindow() override;

			virtual bool_t IsWindowVisible() override;
			virtual bool_t IsWindowMinimized() override;
			virtual bool_t IsWindowMaximized() override;
			virtual bool_t IsWindowFocused() override;

			//////////// MOUSE CURSOR API ///////////
			virtual void GetCursorPos(int32_t* pXPos, int32_t* pYPos) override;
			virtual void SetCursorPos(int32_t xPos, int32_t yPos) override;
			virtual void ShowCursor() override;
			virtual void HideCursor() override;

			/////////////// EVENTS API /////////////////
			virtual void PollEvents() override;
			virtual void WaitEvents() override;
			virtual void WaitEventsTimeout(float64_t timeout) override;

			////////////////////////////////

			virtual int32_t GetKeyScancodeNative(int32_t key) override;

			const WindowWin32::PlatformData& GetPlatformData() const;

			KeyMapWin32* GetKeyMap() const;

		private:
			NO_COPY_NO_MOVE(WindowWin32)

			bool_t CreateWindowNative(const std::string& title, uint32_t width, uint32_t height, uint32_t flags);
			void DestroyWindowNative();
			void ResetWindowDataNative();

			bool_t RegisterWindowClass();
			bool_t UnRegisterWindowClass();

			DWORD GetWindowStyle();
			DWORD GetWindowExStyle();

			KeyMapWin32* mpKeyMap;

			PlatformData mPlatformData;
		};

		////////////////////////////////////

		static WCHAR* CreateWideStringFromUTF8(const char_t* pSource);
		static char_t* CreateUTF8FromWideString(const WCHAR* pSource);
	}
}

#endif // FOUNDATION_PLATFORM_WIN32_PLATFORM_HPP