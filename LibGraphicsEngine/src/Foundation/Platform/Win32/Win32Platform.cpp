#include "Foundation/Platform/Win32/Win32Platform.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"
#include <windows.h>
#include <cassert>
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

using namespace GraphicsEngine;
using namespace Platform;

KeyMapWin32::KeyMapWin32()
	: KeyMap()
{
	CreateTable();
}

KeyMapWin32::~KeyMapWin32()
{
	DestroyTable();
}

void KeyMapWin32::CreateTable()
{
	::memset(mPlatformData.keycodes, -1, sizeof(mPlatformData.keycodes));
	::memset(mPlatformData.scancodes, -1, sizeof(mPlatformData.scancodes));

	mPlatformData.keycodes[0x00B] = GE_KEY_0;
	mPlatformData.keycodes[0x002] = GE_KEY_1;
	mPlatformData.keycodes[0x003] = GE_KEY_2;
	mPlatformData.keycodes[0x004] = GE_KEY_3;
	mPlatformData.keycodes[0x005] = GE_KEY_4;
	mPlatformData.keycodes[0x006] = GE_KEY_5;
	mPlatformData.keycodes[0x007] = GE_KEY_6;
	mPlatformData.keycodes[0x008] = GE_KEY_7;
	mPlatformData.keycodes[0x009] = GE_KEY_8;
	mPlatformData.keycodes[0x00A] = GE_KEY_9;
	mPlatformData.keycodes[0x01E] = GE_KEY_A;
	mPlatformData.keycodes[0x030] = GE_KEY_B;
	mPlatformData.keycodes[0x02E] = GE_KEY_C;
	mPlatformData.keycodes[0x020] = GE_KEY_D;
	mPlatformData.keycodes[0x012] = GE_KEY_E;
	mPlatformData.keycodes[0x021] = GE_KEY_F;
	mPlatformData.keycodes[0x022] = GE_KEY_G;
	mPlatformData.keycodes[0x023] = GE_KEY_H;
	mPlatformData.keycodes[0x017] = GE_KEY_I;
	mPlatformData.keycodes[0x024] = GE_KEY_J;
	mPlatformData.keycodes[0x025] = GE_KEY_K;
	mPlatformData.keycodes[0x026] = GE_KEY_L;
	mPlatformData.keycodes[0x032] = GE_KEY_M;
	mPlatformData.keycodes[0x031] = GE_KEY_N;
	mPlatformData.keycodes[0x018] = GE_KEY_O;
	mPlatformData.keycodes[0x019] = GE_KEY_P;
	mPlatformData.keycodes[0x010] = GE_KEY_Q;
	mPlatformData.keycodes[0x013] = GE_KEY_R;
	mPlatformData.keycodes[0x01F] = GE_KEY_S;
	mPlatformData.keycodes[0x014] = GE_KEY_T;
	mPlatformData.keycodes[0x016] = GE_KEY_U;
	mPlatformData.keycodes[0x02F] = GE_KEY_V;
	mPlatformData.keycodes[0x011] = GE_KEY_W;
	mPlatformData.keycodes[0x02D] = GE_KEY_X;
	mPlatformData.keycodes[0x015] = GE_KEY_Y;
	mPlatformData.keycodes[0x02C] = GE_KEY_Z;

	mPlatformData.keycodes[0x028] = GE_KEY_APOSTROPHE;
	mPlatformData.keycodes[0x02B] = GE_KEY_BACKSLASH;
	mPlatformData.keycodes[0x033] = GE_KEY_COMMA;
	mPlatformData.keycodes[0x00D] = GE_KEY_EQUAL;
	mPlatformData.keycodes[0x029] = GE_KEY_GRAVE_ACCENT;
	mPlatformData.keycodes[0x01A] = GE_KEY_LEFT_BRACKET;
	mPlatformData.keycodes[0x00C] = GE_KEY_MINUS;
	mPlatformData.keycodes[0x034] = GE_KEY_PERIOD;
	mPlatformData.keycodes[0x01B] = GE_KEY_RIGHT_BRACKET;
	mPlatformData.keycodes[0x027] = GE_KEY_SEMICOLON;
	mPlatformData.keycodes[0x035] = GE_KEY_SLASH;
	mPlatformData.keycodes[0x056] = GE_KEY_WORLD_2;

	mPlatformData.keycodes[0x00E] = GE_KEY_BACKSPACE;
	mPlatformData.keycodes[0x153] = GE_KEY_DELETE;
	mPlatformData.keycodes[0x14F] = GE_KEY_END;
	mPlatformData.keycodes[0x01C] = GE_KEY_ENTER;
	mPlatformData.keycodes[0x001] = GE_KEY_ESCAPE;
	mPlatformData.keycodes[0x147] = GE_KEY_HOME;
	mPlatformData.keycodes[0x152] = GE_KEY_INSERT;
	mPlatformData.keycodes[0x15D] = GE_KEY_MENU;
	mPlatformData.keycodes[0x151] = GE_KEY_PAGE_DOWN;
	mPlatformData.keycodes[0x149] = GE_KEY_PAGE_UP;
	mPlatformData.keycodes[0x045] = GE_KEY_PAUSE;
	mPlatformData.keycodes[0x146] = GE_KEY_PAUSE;
	mPlatformData.keycodes[0x039] = GE_KEY_SPACE;
	mPlatformData.keycodes[0x00F] = GE_KEY_TAB;
	mPlatformData.keycodes[0x03A] = GE_KEY_CAPS_LOCK;
	mPlatformData.keycodes[0x145] = GE_KEY_NUM_LOCK;
	mPlatformData.keycodes[0x046] = GE_KEY_SCROLL_LOCK;
	mPlatformData.keycodes[0x03B] = GE_KEY_F1;
	mPlatformData.keycodes[0x03C] = GE_KEY_F2;
	mPlatformData.keycodes[0x03D] = GE_KEY_F3;
	mPlatformData.keycodes[0x03E] = GE_KEY_F4;
	mPlatformData.keycodes[0x03F] = GE_KEY_F5;
	mPlatformData.keycodes[0x040] = GE_KEY_F6;
	mPlatformData.keycodes[0x041] = GE_KEY_F7;
	mPlatformData.keycodes[0x042] = GE_KEY_F8;
	mPlatformData.keycodes[0x043] = GE_KEY_F9;
	mPlatformData.keycodes[0x044] = GE_KEY_F10;
	mPlatformData.keycodes[0x057] = GE_KEY_F11;
	mPlatformData.keycodes[0x058] = GE_KEY_F12;
	mPlatformData.keycodes[0x064] = GE_KEY_F13;
	mPlatformData.keycodes[0x065] = GE_KEY_F14;
	mPlatformData.keycodes[0x066] = GE_KEY_F15;
	mPlatformData.keycodes[0x067] = GE_KEY_F16;
	mPlatformData.keycodes[0x068] = GE_KEY_F17;
	mPlatformData.keycodes[0x069] = GE_KEY_F18;
	mPlatformData.keycodes[0x06A] = GE_KEY_F19;
	mPlatformData.keycodes[0x06B] = GE_KEY_F20;
	mPlatformData.keycodes[0x06C] = GE_KEY_F21;
	mPlatformData.keycodes[0x06D] = GE_KEY_F22;
	mPlatformData.keycodes[0x06E] = GE_KEY_F23;
	mPlatformData.keycodes[0x076] = GE_KEY_F24;
	mPlatformData.keycodes[0x038] = GE_KEY_LEFT_ALT;
	mPlatformData.keycodes[0x01D] = GE_KEY_LEFT_CONTROL;
	mPlatformData.keycodes[0x02A] = GE_KEY_LEFT_SHIFT;
	mPlatformData.keycodes[0x15B] = GE_KEY_LEFT_SUPER;
	mPlatformData.keycodes[0x137] = GE_KEY_PRINT_SCREEN;
	mPlatformData.keycodes[0x138] = GE_KEY_RIGHT_ALT;
	mPlatformData.keycodes[0x11D] = GE_KEY_RIGHT_CONTROL;
	mPlatformData.keycodes[0x036] = GE_KEY_RIGHT_SHIFT;
	mPlatformData.keycodes[0x15C] = GE_KEY_RIGHT_SUPER;
	mPlatformData.keycodes[0x150] = GE_KEY_DOWN;
	mPlatformData.keycodes[0x14B] = GE_KEY_LEFT;
	mPlatformData.keycodes[0x14D] = GE_KEY_RIGHT;
	mPlatformData.keycodes[0x148] = GE_KEY_UP;

	mPlatformData.keycodes[0x052] = GE_KEY_KP_0;
	mPlatformData.keycodes[0x04F] = GE_KEY_KP_1;
	mPlatformData.keycodes[0x050] = GE_KEY_KP_2;
	mPlatformData.keycodes[0x051] = GE_KEY_KP_3;
	mPlatformData.keycodes[0x04B] = GE_KEY_KP_4;
	mPlatformData.keycodes[0x04C] = GE_KEY_KP_5;
	mPlatformData.keycodes[0x04D] = GE_KEY_KP_6;
	mPlatformData.keycodes[0x047] = GE_KEY_KP_7;
	mPlatformData.keycodes[0x048] = GE_KEY_KP_8;
	mPlatformData.keycodes[0x049] = GE_KEY_KP_9;
	mPlatformData.keycodes[0x04E] = GE_KEY_KP_ADD;
	mPlatformData.keycodes[0x053] = GE_KEY_KP_DECIMAL;
	mPlatformData.keycodes[0x135] = GE_KEY_KP_DIVIDE;
	mPlatformData.keycodes[0x11C] = GE_KEY_KP_ENTER;
	mPlatformData.keycodes[0x059] = GE_KEY_KP_EQUAL;
	mPlatformData.keycodes[0x037] = GE_KEY_KP_MULTIPLY;
	mPlatformData.keycodes[0x04A] = GE_KEY_KP_SUBTRACT;

	for (int32_t scancode = 0; scancode < k_KeyCodeCount; scancode++)
	{
		if (mPlatformData.keycodes[scancode] > 0)
			mPlatformData.scancodes[mPlatformData.keycodes[scancode]] = scancode;
	}
}

void KeyMapWin32::DestroyTable()
{
	::memset(mPlatformData.keycodes, -1, sizeof(mPlatformData.keycodes));
	::memset(mPlatformData.scancodes, -1, sizeof(mPlatformData.scancodes));
}

int32_t KeyMapWin32::GetKeyScancode(int32_t key)
{
	return mPlatformData.scancodes[key];
}

const KeyMapWin32::PlatformData& KeyMapWin32::GetPlatformData() const
{
	return mPlatformData;
}

/////////////////////////

WindowWin32::WindowWin32()
	: Window()
{
	mpKeyMap = GE_ALLOC(KeyMapWin32);
	assert(mpKeyMap != nullptr);
}

WindowWin32::WindowWin32(const std::string& title, uint32_t width, uint32_t height, uint32_t flags)
	: WindowWin32()
{
	ResetWindowData();

	if (false == CreateWindowNative(title, width, height, flags))
	{
		DestroyWindowNative();
		LOG_ERROR("Failed to create native window!");
	}

	UpdateWindowFlags(flags);
}

WindowWin32::~WindowWin32()
{
	DestroyWindowNative();

	GE_FREE(mpKeyMap);
}

bool_t WindowWin32::CreateWindowNative(const std::string& title, uint32_t width, uint32_t height, uint32_t flags)
{
	assert(title.empty() == false);
	assert(width > 0);
	assert(height > 0);

	// window class register step
	mPlatformData.instance = ::GetModuleHandle(nullptr);

	if (false == RegisterWindowClass())
	{
		LOG_ERROR("Failed to register native window class, error: %d", ::GetLastError());
		return false;
	}

	// window creation step
	mState.title = title;
	mState.flags = flags;

	int32_t screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
	int32_t screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

	if (mState.flags & GE_WindowFlags::GE_WF_FULLSCREEN)
	{
		DEVMODE dmScreenSettings;
		::memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = screenWidth;
		dmScreenSettings.dmPelsHeight = screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//if ((mWindowWidth != (uint32_t)screenWidth) && (mWindowHeight != (uint32_t)screenHeight))
		{
			if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			{
				if (MessageBox(nullptr, (LPCWSTR)"Fullscreen Mode not supported!\n Switch to window mode?", (LPCWSTR)"Error", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
				{
					//mSettings.fullscreen = false;
				}
				else
				{
					LOG_WARNING("Switched to windows mode!");
					return false;
				}
			}
		}
	}

	RECT windowRect;
	windowRect.left = 0L;
	windowRect.top = 0L;
	windowRect.right = ((mState.flags & GE_WindowFlags::GE_WF_FULLSCREEN) ? static_cast<LONG>(screenWidth) : static_cast<LONG>(width));
	windowRect.bottom = ((mState.flags & GE_WindowFlags::GE_WF_FULLSCREEN) ? static_cast<LONG>(screenHeight) : static_cast<LONG>(height));

	DWORD style = GetWindowStyle();
	DWORD exStyle = GetWindowExStyle();

	::AdjustWindowRectEx(&windowRect, style, FALSE, exStyle);

	mState.width = windowRect.right - windowRect.left;
	mState.height = windowRect.bottom - windowRect.top;

	WCHAR* pWideTitle = Platform::CreateWideStringFromUTF8(title.c_str());

	mPlatformData.handle = ::CreateWindowExW(exStyle,
		GE_WNDCLASSNAME,
		pWideTitle,
		style,
		0,
		0,
		mState.width,
		mState.height,
		nullptr,
		nullptr,
		mPlatformData.instance,
		nullptr);

	GE_FREE_ARRAY(pWideTitle);

	if (NULL == mPlatformData.handle)
	{
		LOG_ERROR("Could not create window, error: %d", ::GetLastError());
		return false;
	}

	// Center on screen if not fullscreen
	if (false == (mState.flags & GE_WindowFlags::GE_WF_FULLSCREEN))
	{
		int32_t x = (screenWidth - windowRect.right) / 2;
		int32_t y = (screenHeight - windowRect.bottom) / 2;
		::SetWindowPos(mPlatformData.handle, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}

	::SetWindowLongPtrW(mPlatformData.handle, GWLP_USERDATA, (LONG_PTR)this);

	return true;
}

void WindowWin32::DestroyWindowNative()
{
	ResetWindowData();

	if (mPlatformData.handle)
	{
		::DestroyWindow(mPlatformData.handle);

		if (false == UnRegisterWindowClass())
		{
			LOG_ERROR("Failed to unregister the native window, error: %d", ::GetLastError());
		}
	}

	ResetWindowDataNative();
}

void WindowWin32::ResetWindowDataNative()
{
	mPlatformData.handle = nullptr;
	mPlatformData.instance = nullptr;
}


LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

bool_t WindowWin32::RegisterWindowClass()
{
	WNDCLASSEX wndClass;

	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndClass.lpfnWndProc = WindowProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = mPlatformData.instance;
	wndClass.hIcon = ::LoadIconW(nullptr, IDI_APPLICATION);
	wndClass.hCursor = ::LoadCursorW(nullptr, IDC_ARROW);
	wndClass.hbrBackground = static_cast<HBRUSH>(::GetStockObject(BLACK_BRUSH));
	wndClass.lpszMenuName = nullptr;
	wndClass.lpszClassName = GE_WNDCLASSNAME;
	wndClass.hIconSm = ::LoadIconW(nullptr, IDI_WINLOGO);

	if (false == ::RegisterClassExW(&wndClass))
	{
		LOG_ERROR("Could not register window class, error: %d\n", GetLastError());
		return false;
	}

	return true;
}

bool_t WindowWin32::UnRegisterWindowClass()
{
	return ::UnregisterClassW(GE_WNDCLASSNAME, mPlatformData.instance);
}

DWORD WindowWin32::GetWindowStyle()
{
	DWORD style = GE_WINDOW_STYLE_BASIC;

	if (mState.flags & Window::GE_WindowFlags::GE_WF_FULLSCREEN)
	{
		style |= GE_WINDOW_STYLE_FULLSCREEN;
	}
	else
	{
		if (mState.flags & Window::GE_WindowFlags::GE_WF_BORDERLESS)
		{
			style |= GE_WINDOW_STYLE_BORDERLESS;
		}
		else
		{
			style |= GE_WINDOW_STYLE_NORMAL;
		}

		if (mState.flags & GE_WindowFlags::GE_WF_RESIZABLE)
		{
			/* You can have a borderless resizable window, but Windows doesn't always draw it correctly,
				see https://bugzilla.libsdl.org/show_bug.cgi?id=4466
				*/
			if (!(mState.flags & GE_WindowFlags::GE_WF_BORDERLESS))
			{
				style |= GE_WINDOW_STYLE_RESIZABLE;
			}
		}

		/* Need to set initialize minimize style, or when we call ShowWindow with WS_MINIMIZE it will activate a random window */
		if (mState.flags & GE_WindowFlags::GE_WF_MINIMIZED)
		{
			style |= WS_MINIMIZE;
		}
	}

	return style;
}

// Returns the extended window style for the specified window
//
DWORD WindowWin32::GetWindowExStyle()
{
	DWORD style = WS_EX_APPWINDOW;

	if (!(mState.flags & GE_WF_FULLSCREEN) && !(mState.flags & GE_WF_BORDERLESS))
	{
		style |= WS_EX_WINDOWEDGE;
	}

	return style;
}

//////////////// WINDOW API //////////////////////

void WindowWin32::GetWindowSize(uint32_t* pWidth, uint32_t* pHeight)
{
	assert(pWidth != nullptr);
	assert(pHeight != nullptr);

	RECT area{};
	::GetClientRect(mPlatformData.handle, &area);

	mState.width = area.right;
	mState.height = area.bottom;

	*pWidth = mState.width;
	*pHeight = mState.height;
}

void WindowWin32::SetWindowSize(uint32_t width, uint32_t height)
{
	mState.width = width;
	mState.height = height;

	DWORD style = GetWindowStyle();
	DWORD exStyle = GetWindowExStyle();

	RECT rect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };

	::AdjustWindowRectEx(&rect, style, FALSE, exStyle);

	::SetWindowPos(mPlatformData.handle, HWND_TOP,
		0, 0, rect.right - rect.left, rect.bottom - rect.top,
		SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOZORDER);
}

void WindowWin32::GetWindowPos(int32_t* pXPos, int32_t* pYPos)
{
	assert(pXPos != nullptr);
	assert(pYPos != nullptr);

	POINT pos = { 0, 0 };
	::ClientToScreen(mPlatformData.handle, &pos);

	*pXPos = pos.x;
	*pYPos = pos.y;
}

void WindowWin32::SetWindowPos(int32_t xPos, int32_t yPos)
{
	DWORD style = GetWindowStyle();
	DWORD exStyle = GetWindowExStyle();

	RECT rect = { xPos, xPos, xPos, xPos };
	::AdjustWindowRectEx(&rect, style, FALSE, exStyle);

	::SetWindowPos(mPlatformData.handle, NULL, rect.left, rect.top, 0, 0,
		SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
}

void WindowWin32::SetWindowTitle(const std::string& title)
{
	WCHAR* pWideTitle = Platform::CreateWideStringFromUTF8(title.c_str());
	assert(pWideTitle != nullptr);

	::SetWindowTextW(mPlatformData.handle, static_cast<LPCWSTR>(pWideTitle));

	GE_FREE_ARRAY(pWideTitle);

	mState.title = title;
}

void WindowWin32::MinimizeWindow()
{
	::ShowWindow(mPlatformData.handle, SW_MINIMIZE);
}

void WindowWin32::MaximizeWindow()
{
	::ShowWindow(mPlatformData.handle, SW_MAXIMIZE);
}

void WindowWin32::RestoreWindow()
{
	::ShowWindow(mPlatformData.handle, SW_RESTORE);
}

void WindowWin32::ShowWindow()
{
	::ShowWindow(mPlatformData.handle, SW_SHOWNA); //not activated window
	//::ShowWindow(mPlatformData.handle, SW_SHOW); //activated window
}

void WindowWin32::HideWindow()
{
	::ShowWindow(mPlatformData.handle, SW_HIDE); //show our window
}

void WindowWin32::FocusWindow()
{
	::BringWindowToTop(mPlatformData.handle);
	::SetForegroundWindow(mPlatformData.handle);
	::SetFocus(mPlatformData.handle);
}

bool_t WindowWin32::IsWindowVisible()
{
	return ::IsWindowVisible(mPlatformData.handle);
}

bool_t WindowWin32::IsWindowMinimized()
{
	return ::IsIconic(mPlatformData.handle);
}

bool_t WindowWin32::IsWindowMaximized()
{
	return ::IsZoomed(mPlatformData.handle);
}

bool_t WindowWin32::IsWindowFocused()
{
	return mPlatformData.handle == ::GetActiveWindow();
}

//////////////// MOUSE CURSOR API //////////////////////

void WindowWin32::GetCursorPos(int32_t* pXPos, int32_t* pYPos)
{
	assert(pXPos != nullptr);
	assert(pYPos != nullptr);

	POINT pos{};

	if (::GetCursorPos(&pos))
	{
		::ScreenToClient(mPlatformData.handle, &pos);

		*pXPos = pos.x;
		*pYPos = pos.y;
	}
}

void WindowWin32::SetCursorPos(int32_t xPos, int32_t yPos)
{
	POINT pos = { xPos, yPos };

	::ClientToScreen(mPlatformData.handle, &pos);
	::SetCursorPos(pos.x, pos.y);
}

void WindowWin32::ShowCursor()
{
	::ShowCursor(TRUE);
}

void WindowWin32::HideCursor()
{
	::ShowCursor(FALSE);
}


///////////// EVENTS API ////////////////

void WindowWin32::PollEvents()
{
	MSG msg{};

	while (::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			InputWindowCloseRequest();
		}
		else
		{
			::TranslateMessage(&msg);
			::DispatchMessageW(&msg);
		}
	}
}

void WindowWin32::WaitEvents()
{
	::WaitMessage();

	PollEvents();
}

void WindowWin32::WaitEventsTimeout(float64_t timeout)
{
	::MsgWaitForMultipleObjects(0, NULL, FALSE, static_cast<DWORD>(timeout * 1e3), QS_ALLEVENTS);

	PollEvents();
}

int32_t WindowWin32::GetKeyScancodeNative(int32_t key)
{
	assert(mpKeyMap != nullptr);

	return mpKeyMap->GetKeyScancode(key);
}

const WindowWin32::PlatformData& WindowWin32::GetPlatformData() const
{
	return mPlatformData;
}

KeyMapWin32* WindowWin32::GetKeyMap() const
{
	return mpKeyMap;
}

//////////////////////////////////////////////

// Window callback function (handles window messages)
//
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WindowWin32* pWindow = reinterpret_cast<WindowWin32*>(::GetWindowLongPtrW(hWnd, GWLP_USERDATA));

	if (pWindow == nullptr)
		return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);

	switch (uMsg)
	{
		case WM_CLOSE:
			pWindow->InputWindowCloseRequest();
			break;
		case WM_PAINT:
		{
			RECT rect;
			if (::GetUpdateRect(pWindow->GetPlatformData().handle, &rect, FALSE))
			{
				::ValidateRect(pWindow->GetPlatformData().handle, NULL);
			}
			pWindow->InputWindowRefresh();
		}
			break;
		case WM_SETFOCUS:
			pWindow->InputWindowFocus(true);
			break;
		case WM_KILLFOCUS:
			pWindow->InputWindowFocus(false);
			break;
		case WM_INPUTLANGCHANGE:
			break;
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			int32_t key = -1, scancode = -1;
			const int32_t action = (HIWORD(lParam) & KF_UP) ? GE_KEY_RELEASE : GE_KEY_PRESS;

			scancode = (HIWORD(lParam) & (KF_EXTENDED | 0xff));
			if (!scancode)
			{
				// NOTE: Some synthetic key messages have a scancode of zero
				// Map the virtual key back to a usable scancode
				scancode = MapVirtualKey(static_cast<UINT>(wParam), MAPVK_VK_TO_VSC);
			}

			KeyMapWin32* pKeyMap = pWindow->GetKeyMap();
			assert(pKeyMap != nullptr);

			key = pKeyMap->GetPlatformData().keycodes[scancode];

			pWindow->InputKey(key, scancode, action);
		}
			break;
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_XBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_XBUTTONUP:
		{
			int32_t i = -1, button = -1, action = -1;

			if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP)
				button = GE_MOUSE_BUTTON_LEFT;
			else if (uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP)
				button = GE_MOUSE_BUTTON_RIGHT;
			else if (uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONUP)
				button = GE_MOUSE_BUTTON_MIDDLE;
			else if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
				button = GE_MOUSE_BUTTON_4;
			else
				button = GE_MOUSE_BUTTON_5;

			if (uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN ||
				uMsg == WM_MBUTTONDOWN || uMsg == WM_XBUTTONDOWN)
			{
				action = GE_MOUSE_BUTTON_PRESS;
			}
			else
			{
				action = GE_MOUSE_BUTTON_RELEASE;
			}
			for (i = 0; i <= GE_MOUSE_BUTTON_LAST; i++)
			{
				if (pWindow->GetState().mouseButtons[i] == GE_MOUSE_BUTTON_PRESS)
					break;
			}

			if (i > GE_MOUSE_BUTTON_LAST)
				::SetCapture(hWnd);

			pWindow->InputMouseButton(button, action);

			for (i = 0; i <= GE_MOUSE_BUTTON_LAST; i++)
			{
				if (pWindow->GetState().mouseButtons[i] == GE_MOUSE_BUTTON_PRESS)
					break;
			}

			if (i > GE_MOUSE_BUTTON_LAST)
				::ReleaseCapture();

			if (uMsg == WM_XBUTTONDOWN || uMsg == WM_XBUTTONUP)
				return TRUE;
		}
			break;
		case WM_MOUSEMOVE:
		{
			const int32_t x = LOWORD(lParam);
			const int32_t y = HIWORD(lParam);

			if (false == pWindow->GetState().isMouseInsideWindow)
			{
				pWindow->SetIsMouseInsideWindow(true);
				pWindow->InputMouseEnterWindow(true);
			}
			pWindow->InputMouseMove(x, y);
		}
			break;
		case WM_MOUSELEAVE:
		{
			pWindow->SetIsMouseInsideWindow(false);
			pWindow->InputMouseEnterWindow(false);
		}
			break;
		case WM_MOUSEWHEEL:
			pWindow->InputMouseScroll(0.0, static_cast<SHORT>(HIWORD(wParam)) / static_cast<float64_t>(WHEEL_DELTA));
			break;
		case WM_MOUSEHWHEEL:
			// NOTE! This message is only sent on Windows Vista and later
			// NOTE: The X-axis is inverted for consistency with macOS and X11
			pWindow->InputMouseScroll(-(static_cast<SHORT>(HIWORD(wParam)) / static_cast<float64_t>(WHEEL_DELTA)), 0.0);
			break;
		case WM_ENTERSIZEMOVE:
		case WM_ENTERMENULOOP:
			break;
		case WM_EXITSIZEMOVE:
		case WM_EXITMENULOOP:
			break;
		case WM_SIZE:
		{
			const bool_t isMinimized = wParam == SIZE_MINIMIZED;
			const bool_t isMaximized = wParam == SIZE_MAXIMIZED ||
				(pWindow->GetState().isMaximized && wParam != SIZE_RESTORED);

			if (pWindow->GetState().isMinimized != isMinimized)
				pWindow->InputWindowMinimize(isMinimized);

			if (pWindow->GetState().isMaximized != isMaximized)
				pWindow->InputWindowMaximize(isMaximized);

			pWindow->InputWindowSize(LOWORD(lParam), HIWORD(lParam));

			pWindow->SetIsMinimized(isMinimized);
			pWindow->SetIsMaximized(isMaximized);
		}
			break;
	}

	return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

///////////////////////////////////


namespace GraphicsEngine
{
	namespace Platform
	{
		// String API
		WCHAR* CreateWideStringFromUTF8(const char_t* pSource)
		{
			WCHAR* pTarget = nullptr;
			int32_t count = 0;

			count = ::MultiByteToWideChar(CP_UTF8, 0, pSource, -1, nullptr, 0);
			if (count == 0)
			{
				LOG_ERROR("Win32: Failed to convert string from UTF-8");
				return nullptr;
			}

			pTarget = GE_ALLOC_ARRAY(WCHAR, count);
			assert(pTarget != nullptr);

			if (0 == ::MultiByteToWideChar(CP_UTF8, 0, pSource, -1, pTarget, count))
			{
				GE_FREE_ARRAY(pTarget);
				LOG_ERROR("Win32: Failed to convert string from UTF-8");

				return nullptr;
			}

			return pTarget;
		}

		char_t* CreateUTF8FromWideString(const WCHAR* pSource)
		{
			char_t* pTarget = nullptr;
			int32_t count = 0;

			count = ::WideCharToMultiByte(CP_UTF8, 0, pSource, -1, nullptr, 0, nullptr, nullptr);
			if (count == 0)
			{
				LOG_ERROR("Win32: Failed to convert string to UTF-8");
				return nullptr;
			}

			pTarget = GE_ALLOC_ARRAY(char_t, count);
			assert(pTarget != nullptr);

			if (0 == ::WideCharToMultiByte(CP_UTF8, 0, pSource, -1, pTarget, count, nullptr, nullptr))
			{
				GE_FREE_ARRAY(pTarget);
				LOG_ERROR("Win32: Failed to convert string to UTF-8");

				return nullptr;
			}

			return pTarget;
		}
	}
}