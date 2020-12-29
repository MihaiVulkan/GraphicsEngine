#include "Win32Platform.hpp"
#include "Foundation/Platform/PlatformInternal.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"
#include <cstring>
#include <cassert>

namespace GraphicsEngine
{
	namespace Platform
	{
		//////////////////
		extern GE_KeyData g_KeyData;

		//////////// HELPER FUNCTIONS //////////////

		LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		//////////////////////////

		bool_t RegisterWindowClass_WIN32(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			WNDCLASSEX wndClass;

			wndClass.cbSize = sizeof(WNDCLASSEX);
			wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
			wndClass.lpfnWndProc = WindowProc;
			wndClass.cbClsExtra = 0;
			wndClass.cbWndExtra = 0;
			wndClass.hInstance = pWindow->win32.instance;
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

		bool_t UnRegisterWindowClass_WIN32(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			return ::UnregisterClassW(GE_WNDCLASSNAME, pWindow->win32.instance);
		}

		// String API
		WCHAR* CreateWideStringFromUTF8_WIN32(const char_t* pSource)
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

		char_t* CreateUTF8FromWideString_WIN32(const WCHAR* pSource)
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

		DWORD GetWindowStyle(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);


			DWORD style = GE_WINDOW_STYLE_BASIC;

			if (pWindow->flags & GE_WindowFlags::GE_WINDOW_FULLSCREEN)
			{
				style |= GE_WINDOW_STYLE_FULLSCREEN;
			}
			else
			{
				if (pWindow->flags & GE_WindowFlags::GE_WINDOW_BORDERLESS)
				{
					style |= GE_WINDOW_STYLE_BORDERLESS;
				}
				else
				{
					style |= GE_WINDOW_STYLE_NORMAL;
				}

				if (pWindow->flags & GE_WindowFlags::GE_WINDOW_RESIZABLE)
				{
					/* You can have a borderless resizable window, but Windows doesn't always draw it correctly,
						see https://bugzilla.libsdl.org/show_bug.cgi?id=4466
						*/
					if (!(pWindow->flags & GE_WindowFlags::GE_WINDOW_BORDERLESS))
					{
						style |= GE_WINDOW_STYLE_RESIZABLE;
					}
				}

				/* Need to set initialize minimize style, or when we call ShowWindow with WS_MINIMIZE it will activate a random window */
				if (pWindow->flags & GE_WindowFlags::GE_WINDOW_MINIMIZED)
				{
					style |= WS_MINIMIZE;
				}
			}

			return style;
		}

		// Returns the extended window style for the specified window
		//
		DWORD GetWindowExStyle(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			DWORD style = WS_EX_APPWINDOW;

			if (!(pWindow->flags & GE_WINDOW_FULLSCREEN) && !(pWindow->flags & GE_WINDOW_BORDERLESS))
			{
				style |= WS_EX_WINDOWEDGE;
			}

			return style;
		}

		/////////////////////////

		const char_t* GetScancodeName_Platform(int32_t scancode)
		{
			if (scancode < 0 || scancode >(KF_EXTENDED | 0xff) ||
				g_KeyData.win32.keycodes[scancode] == GE_KEY_UNKNOWN)
			{
				LOG_ERROR("Invalid scancode");
				return nullptr;
			}

			return g_KeyData.win32.keynames[g_KeyData.win32.keycodes[scancode]];
		}

		int32_t GetKeyScancode_Platform(int32_t key)
		{
			return g_KeyData.win32.scancodes[key];
		}

		char_t* DuplicateStr(const char_t* pStr)
		{
			size_t size = ::strlen(pStr);
			char_t* pDupStr = GE_ALLOC_ARRAY(char_t, size + 1);
			assert(pDupStr != nullptr);

			::strcpy_s(pDupStr, size + 1, pStr);
			pDupStr[size] = '\0';

			return pDupStr;
		}

		///////////// INIT API /////////////

		void Init_Platform()
		{
			CreateKeyTables_WIN32();
			UpdateKeyNames_WIN32();
		}

		void Terminate_Platform()
		{
		}

		void CreateKeyTables_WIN32()
		{
			int32_t scancode;

			memset(g_KeyData.win32.keycodes, -1, sizeof(g_KeyData.win32.keycodes));
			memset(g_KeyData.win32.scancodes, -1, sizeof(g_KeyData.win32.scancodes));

			g_KeyData.win32.keycodes[0x00B] = GE_KEY_0;
			g_KeyData.win32.keycodes[0x002] = GE_KEY_1;
			g_KeyData.win32.keycodes[0x003] = GE_KEY_2;
			g_KeyData.win32.keycodes[0x004] = GE_KEY_3;
			g_KeyData.win32.keycodes[0x005] = GE_KEY_4;
			g_KeyData.win32.keycodes[0x006] = GE_KEY_5;
			g_KeyData.win32.keycodes[0x007] = GE_KEY_6;
			g_KeyData.win32.keycodes[0x008] = GE_KEY_7;
			g_KeyData.win32.keycodes[0x009] = GE_KEY_8;
			g_KeyData.win32.keycodes[0x00A] = GE_KEY_9;
			g_KeyData.win32.keycodes[0x01E] = GE_KEY_A;
			g_KeyData.win32.keycodes[0x030] = GE_KEY_B;
			g_KeyData.win32.keycodes[0x02E] = GE_KEY_C;
			g_KeyData.win32.keycodes[0x020] = GE_KEY_D;
			g_KeyData.win32.keycodes[0x012] = GE_KEY_E;
			g_KeyData.win32.keycodes[0x021] = GE_KEY_F;
			g_KeyData.win32.keycodes[0x022] = GE_KEY_G;
			g_KeyData.win32.keycodes[0x023] = GE_KEY_H;
			g_KeyData.win32.keycodes[0x017] = GE_KEY_I;
			g_KeyData.win32.keycodes[0x024] = GE_KEY_J;
			g_KeyData.win32.keycodes[0x025] = GE_KEY_K;
			g_KeyData.win32.keycodes[0x026] = GE_KEY_L;
			g_KeyData.win32.keycodes[0x032] = GE_KEY_M;
			g_KeyData.win32.keycodes[0x031] = GE_KEY_N;
			g_KeyData.win32.keycodes[0x018] = GE_KEY_O;
			g_KeyData.win32.keycodes[0x019] = GE_KEY_P;
			g_KeyData.win32.keycodes[0x010] = GE_KEY_Q;
			g_KeyData.win32.keycodes[0x013] = GE_KEY_R;
			g_KeyData.win32.keycodes[0x01F] = GE_KEY_S;
			g_KeyData.win32.keycodes[0x014] = GE_KEY_T;
			g_KeyData.win32.keycodes[0x016] = GE_KEY_U;
			g_KeyData.win32.keycodes[0x02F] = GE_KEY_V;
			g_KeyData.win32.keycodes[0x011] = GE_KEY_W;
			g_KeyData.win32.keycodes[0x02D] = GE_KEY_X;
			g_KeyData.win32.keycodes[0x015] = GE_KEY_Y;
			g_KeyData.win32.keycodes[0x02C] = GE_KEY_Z;

			g_KeyData.win32.keycodes[0x028] = GE_KEY_APOSTROPHE;
			g_KeyData.win32.keycodes[0x02B] = GE_KEY_BACKSLASH;
			g_KeyData.win32.keycodes[0x033] = GE_KEY_COMMA;
			g_KeyData.win32.keycodes[0x00D] = GE_KEY_EQUAL;
			g_KeyData.win32.keycodes[0x029] = GE_KEY_GRAVE_ACCENT;
			g_KeyData.win32.keycodes[0x01A] = GE_KEY_LEFT_BRACKET;
			g_KeyData.win32.keycodes[0x00C] = GE_KEY_MINUS;
			g_KeyData.win32.keycodes[0x034] = GE_KEY_PERIOD;
			g_KeyData.win32.keycodes[0x01B] = GE_KEY_RIGHT_BRACKET;
			g_KeyData.win32.keycodes[0x027] = GE_KEY_SEMICOLON;
			g_KeyData.win32.keycodes[0x035] = GE_KEY_SLASH;
			g_KeyData.win32.keycodes[0x056] = GE_KEY_WORLD_2;

			g_KeyData.win32.keycodes[0x00E] = GE_KEY_BACKSPACE;
			g_KeyData.win32.keycodes[0x153] = GE_KEY_DELETE;
			g_KeyData.win32.keycodes[0x14F] = GE_KEY_END;
			g_KeyData.win32.keycodes[0x01C] = GE_KEY_ENTER;
			g_KeyData.win32.keycodes[0x001] = GE_KEY_ESCAPE;
			g_KeyData.win32.keycodes[0x147] = GE_KEY_HOME;
			g_KeyData.win32.keycodes[0x152] = GE_KEY_INSERT;
			g_KeyData.win32.keycodes[0x15D] = GE_KEY_MENU;
			g_KeyData.win32.keycodes[0x151] = GE_KEY_PAGE_DOWN;
			g_KeyData.win32.keycodes[0x149] = GE_KEY_PAGE_UP;
			g_KeyData.win32.keycodes[0x045] = GE_KEY_PAUSE;
			g_KeyData.win32.keycodes[0x146] = GE_KEY_PAUSE;
			g_KeyData.win32.keycodes[0x039] = GE_KEY_SPACE;
			g_KeyData.win32.keycodes[0x00F] = GE_KEY_TAB;
			g_KeyData.win32.keycodes[0x03A] = GE_KEY_CAPS_LOCK;
			g_KeyData.win32.keycodes[0x145] = GE_KEY_NUM_LOCK;
			g_KeyData.win32.keycodes[0x046] = GE_KEY_SCROLL_LOCK;
			g_KeyData.win32.keycodes[0x03B] = GE_KEY_F1;
			g_KeyData.win32.keycodes[0x03C] = GE_KEY_F2;
			g_KeyData.win32.keycodes[0x03D] = GE_KEY_F3;
			g_KeyData.win32.keycodes[0x03E] = GE_KEY_F4;
			g_KeyData.win32.keycodes[0x03F] = GE_KEY_F5;
			g_KeyData.win32.keycodes[0x040] = GE_KEY_F6;
			g_KeyData.win32.keycodes[0x041] = GE_KEY_F7;
			g_KeyData.win32.keycodes[0x042] = GE_KEY_F8;
			g_KeyData.win32.keycodes[0x043] = GE_KEY_F9;
			g_KeyData.win32.keycodes[0x044] = GE_KEY_F10;
			g_KeyData.win32.keycodes[0x057] = GE_KEY_F11;
			g_KeyData.win32.keycodes[0x058] = GE_KEY_F12;
			g_KeyData.win32.keycodes[0x064] = GE_KEY_F13;
			g_KeyData.win32.keycodes[0x065] = GE_KEY_F14;
			g_KeyData.win32.keycodes[0x066] = GE_KEY_F15;
			g_KeyData.win32.keycodes[0x067] = GE_KEY_F16;
			g_KeyData.win32.keycodes[0x068] = GE_KEY_F17;
			g_KeyData.win32.keycodes[0x069] = GE_KEY_F18;
			g_KeyData.win32.keycodes[0x06A] = GE_KEY_F19;
			g_KeyData.win32.keycodes[0x06B] = GE_KEY_F20;
			g_KeyData.win32.keycodes[0x06C] = GE_KEY_F21;
			g_KeyData.win32.keycodes[0x06D] = GE_KEY_F22;
			g_KeyData.win32.keycodes[0x06E] = GE_KEY_F23;
			g_KeyData.win32.keycodes[0x076] = GE_KEY_F24;
			g_KeyData.win32.keycodes[0x038] = GE_KEY_LEFT_ALT;
			g_KeyData.win32.keycodes[0x01D] = GE_KEY_LEFT_CONTROL;
			g_KeyData.win32.keycodes[0x02A] = GE_KEY_LEFT_SHIFT;
			g_KeyData.win32.keycodes[0x15B] = GE_KEY_LEFT_SUPER;
			g_KeyData.win32.keycodes[0x137] = GE_KEY_PRINT_SCREEN;
			g_KeyData.win32.keycodes[0x138] = GE_KEY_RIGHT_ALT;
			g_KeyData.win32.keycodes[0x11D] = GE_KEY_RIGHT_CONTROL;
			g_KeyData.win32.keycodes[0x036] = GE_KEY_RIGHT_SHIFT;
			g_KeyData.win32.keycodes[0x15C] = GE_KEY_RIGHT_SUPER;
			g_KeyData.win32.keycodes[0x150] = GE_KEY_DOWN;
			g_KeyData.win32.keycodes[0x14B] = GE_KEY_LEFT;
			g_KeyData.win32.keycodes[0x14D] = GE_KEY_RIGHT;
			g_KeyData.win32.keycodes[0x148] = GE_KEY_UP;

			g_KeyData.win32.keycodes[0x052] = GE_KEY_KP_0;
			g_KeyData.win32.keycodes[0x04F] = GE_KEY_KP_1;
			g_KeyData.win32.keycodes[0x050] = GE_KEY_KP_2;
			g_KeyData.win32.keycodes[0x051] = GE_KEY_KP_3;
			g_KeyData.win32.keycodes[0x04B] = GE_KEY_KP_4;
			g_KeyData.win32.keycodes[0x04C] = GE_KEY_KP_5;
			g_KeyData.win32.keycodes[0x04D] = GE_KEY_KP_6;
			g_KeyData.win32.keycodes[0x047] = GE_KEY_KP_7;
			g_KeyData.win32.keycodes[0x048] = GE_KEY_KP_8;
			g_KeyData.win32.keycodes[0x049] = GE_KEY_KP_9;
			g_KeyData.win32.keycodes[0x04E] = GE_KEY_KP_ADD;
			g_KeyData.win32.keycodes[0x053] = GE_KEY_KP_DECIMAL;
			g_KeyData.win32.keycodes[0x135] = GE_KEY_KP_DIVIDE;
			g_KeyData.win32.keycodes[0x11C] = GE_KEY_KP_ENTER;
			g_KeyData.win32.keycodes[0x059] = GE_KEY_KP_EQUAL;
			g_KeyData.win32.keycodes[0x037] = GE_KEY_KP_MULTIPLY;
			g_KeyData.win32.keycodes[0x04A] = GE_KEY_KP_SUBTRACT;

			for (scancode = 0; scancode < 512; scancode++)
			{
				if (g_KeyData.win32.keycodes[scancode] > 0)
					g_KeyData.win32.scancodes[g_KeyData.win32.keycodes[scancode]] = scancode;
			}
		}

		void UpdateKeyNames_WIN32()
		{
			int32_t key = 0;
			BYTE state[256] = { 0 };

			memset(g_KeyData.win32.keynames, 0, sizeof(g_KeyData.win32.keynames));

			for (key = GE_KEY_SPACE; key <= GE_KEY_LAST; key++)
			{
				UINT vk = 0;
				int32_t scancode, length;
				WCHAR chars[16] = { 0 };

				scancode = g_KeyData.win32.scancodes[key];
				if (scancode == -1)
					continue;

				if (key >= GE_KEY_KP_0 && key <= GE_KEY_KP_ADD)
				{
					const UINT vks[] = {
						VK_NUMPAD0,  VK_NUMPAD1,  VK_NUMPAD2, VK_NUMPAD3,
						VK_NUMPAD4,  VK_NUMPAD5,  VK_NUMPAD6, VK_NUMPAD7,
						VK_NUMPAD8,  VK_NUMPAD9,  VK_DECIMAL, VK_DIVIDE,
						VK_MULTIPLY, VK_SUBTRACT, VK_ADD
					};

					vk = vks[key - GE_KEY_KP_0];
				}
				else
				{
				 	vk = ::MapVirtualKeyW(scancode, MAPVK_VSC_TO_VK);
				}


				length = ::ToUnicode(vk, scancode, state,
					chars, sizeof(chars) / sizeof(WCHAR), 0);

				if (length == -1)
				{
					length = ::ToUnicode(vk, scancode, state,
						chars, sizeof(chars) / sizeof(WCHAR), 0);
				}

				if (length < 1)
				{
					continue;
				}

				::WideCharToMultiByte(CP_UTF8, 0, chars, 1, g_KeyData.win32.keynames[key],
					sizeof(g_KeyData.win32.keynames[key]), nullptr, nullptr);
			}
		}

		//////////////// MOUSE CURSOR API //////////////////////

		void GetCursorPos_Platform(GE_Window* pWindow, int32_t* pXPos, int32_t* pYPos)
		{
			assert(pWindow != nullptr);
			assert(pXPos != nullptr);
			assert(pYPos != nullptr);

			POINT pos;

			if (GetCursorPos(&pos))
			{
				::ScreenToClient(pWindow->win32.handle, &pos);

				*pXPos = pos.x;
				*pYPos = pos.y;
			}
		}

		void SetCursorPos_Platform(GE_Window* pWindow, int32_t xPos, int32_t yPos)
		{
			assert(pWindow != nullptr);

			POINT pos = { xPos, yPos };

			::ClientToScreen(pWindow->win32.handle, &pos);
			::SetCursorPos(pos.x, pos.y);
		}

		void SetCursorMode_Platform(GE_Window* window, int32_t mode)
		{
			//TODO
		}

		void ShowCursor_Platform()
		{
			::ShowCursor(TRUE);
		}

		void HideCursor_Platform()
		{
			::ShowCursor(FALSE);
		}

		//////////////// WINDOW API //////////////////////

		bool_t CreateWindow_Platform(GE_Window* pWindow, const char_t* pTitle, int32_t xPos, int32_t yPos, uint32_t width, uint32_t height, uint32_t flags)
		{
			assert(pWindow != nullptr);
			assert(pTitle != nullptr);
			assert(width > 0);
			assert(height > 0);

			// window class register step
			pWindow->win32.instance = GetModuleHandle(nullptr);

			if (false == RegisterWindowClass_WIN32(pWindow))
			{
				return false;
			}

			// window creation step
			pWindow->pTitle = DuplicateStr(pTitle);
			pWindow->flags = flags;

			int32_t screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
			int32_t screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

			if (pWindow->flags & GE_WindowFlags::GE_WINDOW_FULLSCREEN)
				//if (mSettings.fullscreen)
			{
				DEVMODE dmScreenSettings;
				memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
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
			windowRect.right = ((pWindow->flags & GE_WindowFlags::GE_WINDOW_FULLSCREEN) ? static_cast<LONG>(screenWidth) : static_cast<LONG>(width));
			windowRect.bottom = ((pWindow->flags & GE_WindowFlags::GE_WINDOW_FULLSCREEN) ? static_cast<LONG>(screenHeight) : static_cast<LONG>(height));

			DWORD style = GetWindowStyle(pWindow);
			DWORD exStyle = GetWindowExStyle(pWindow);

			::AdjustWindowRectEx(&windowRect, style, FALSE, exStyle);

			pWindow->width = windowRect.right - windowRect.left;
			pWindow->height = windowRect.bottom - windowRect.top;

			WCHAR* pWideTitle = CreateWideStringFromUTF8_WIN32(pTitle);

			pWindow->win32.handle = ::CreateWindowExW(exStyle,
				GE_WNDCLASSNAME,
				pWideTitle,
				style,
				0,
				0,
				pWindow->width,
				pWindow->height,
				nullptr,
				nullptr,
				pWindow->win32.instance,
				nullptr);

			GE_FREE_ARRAY(pWideTitle);

			if (NULL == pWindow->win32.handle)
			{
				LOG_ERROR("Could not create window, error: %d\n", GetLastError());
				return false;
			}

			if (false == (pWindow->flags & GE_WindowFlags::GE_WINDOW_FULLSCREEN))
			{
				// Center on screen
				int32_t x = (screenWidth - windowRect.right) / 2;
				int32_t y = (screenHeight - windowRect.bottom) / 2;
				::SetWindowPos(pWindow->win32.handle, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			}

			//SetPropW(pWindow->win32.handle, GE_WindowProperty, pWindow);
			::SetWindowLongPtrW(pWindow->win32.handle, GWLP_USERDATA, (LONG_PTR)pWindow);

			return true;
		}

		void DestroyWindow_Platform(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			if (pWindow->pTitle)
			{
				GE_FREE_ARRAY(pWindow->pTitle);
			}

			if (pWindow->win32.handle)
			{
				UnRegisterWindowClass_WIN32(pWindow);

				//::RemovePropW(pWindow->win32.handle, GE_WindowProperty);

				::DestroyWindow(pWindow->win32.handle);
			}

			ResetWindowData(pWindow);

			GE_FREE(pWindow);
		}

		void ResetWindowData_Platform(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			pWindow->win32.handle = nullptr;
			pWindow->win32.instance = nullptr;
		}

		void GetWindowSize_Platform(GE_Window* pWindow, uint32_t* pWidth, uint32_t* pHeight)
		{
			assert(pWindow != nullptr);
			assert(pWidth != nullptr);
			assert(pHeight != nullptr);

			RECT area;
			::GetClientRect(pWindow->win32.handle, &area);

			pWindow->width = area.right;
			pWindow->height = area.bottom;

			*pWidth = pWindow->width;
			*pHeight = pWindow->height;
		}

		void SetWindowSize_Platform(GE_Window* pWindow, uint32_t width, uint32_t height)
		{
			assert(pWindow != nullptr);

			pWindow->width = width;
			pWindow->height = height;

			DWORD style = GetWindowStyle(pWindow);
			DWORD exStyle = GetWindowExStyle(pWindow);

			RECT rect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };

			::AdjustWindowRectEx(&rect, style, FALSE, exStyle);

			::SetWindowPos(pWindow->win32.handle, HWND_TOP,
				0, 0, rect.right - rect.left, rect.bottom - rect.top,
				SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOZORDER);
		}

		void GetWindowPos_Platform(GE_Window* pWindow, int32_t* pXPos, int32_t* pYPos)
		{
			assert(pWindow != nullptr);
			assert(pXPos != nullptr);
			assert(pYPos != nullptr);

			POINT pos = { 0, 0 };
			::ClientToScreen(pWindow->win32.handle, &pos);

			*pXPos = pos.x;
			*pYPos = pos.y;
		}

		void SetWindowPos_Platform(GE_Window* pWindow, int32_t xPos, int32_t yPos)
		{
			assert(pWindow != nullptr);

			DWORD style = GetWindowStyle(pWindow);
			DWORD exStyle = GetWindowExStyle(pWindow);

			RECT rect = { xPos, xPos, xPos, xPos };
			::AdjustWindowRectEx(&rect, style, FALSE, exStyle);

			::SetWindowPos(pWindow->win32.handle, NULL, rect.left, rect.top, 0, 0,
				SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
		}

		void GetWindowTitle_Platform(GE_Window* pWindow, const char_t* pTitle)
		{
			assert(pWindow != nullptr);
			//assert(pTitle != nullptr); //assume pTitle is preallocated!

			//pTitle = pWindow->pTitle;

			pTitle = DuplicateStr(pWindow->pTitle); //we make a deep copy
		}

		void SetWindowTitle_Platform(GE_Window* pWindow, const char_t* pTitle)
		{
			assert(pWindow != nullptr);

			WCHAR* pWideTitle = CreateWideStringFromUTF8_WIN32(pTitle);
			assert(pWideTitle != nullptr);

			::SetWindowTextW(pWindow->win32.handle, static_cast<LPCWSTR>(pWideTitle));

			GE_FREE_ARRAY(pWideTitle);

			if (pWindow->pTitle)
			{
				GE_FREE_ARRAY(pWindow->pTitle);
			}

			pWindow->pTitle = DuplicateStr(pTitle);
		}

		void MinimizeWindow_Platform(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			::ShowWindow(pWindow->win32.handle, SW_MINIMIZE);
		}

		void MaximizeWindow_Platform(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			::ShowWindow(pWindow->win32.handle, SW_MAXIMIZE);
		}

		void RestoreWindow_Platform(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			::ShowWindow(pWindow->win32.handle, SW_RESTORE);
		}

		void ShowWindow_Platform(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			::ShowWindow(pWindow->win32.handle, SW_SHOWNA); //not activated window
			//::ShowWindow(pWindow->win32.handle, SW_SHOW); //activated window
		}

		void HideWindow_Platform(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			::ShowWindow(pWindow->win32.handle, SW_HIDE); //show our window
		}

		void FocusWindow_Platform(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			::BringWindowToTop(pWindow->win32.handle);
			::SetForegroundWindow(pWindow->win32.handle);
			::SetFocus(pWindow->win32.handle);
		}

		bool_t IsWindowVisible_Platform(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			return ::IsWindowVisible(pWindow->win32.handle);
		}

		bool_t IsWindowMinimized_Platform(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			return ::IsIconic(pWindow->win32.handle);
		}

		bool_t IsWindowMaximized_Platform(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			return ::IsZoomed(pWindow->win32.handle);
		}

		bool_t IsWindowFocused_Platform(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			return pWindow->win32.handle == ::GetActiveWindow();
		}

		///////////// EVENTS API ////////////////

		void PollEvents_Platform(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			MSG msg;

			while (::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
				{
					// NOTE: While GLFW does not itself post WM_QUIT, other processes
					//       may post it to this one, for example Task Manager
					// HACK: Treat WM_QUIT as a close on all windows

					InputWindowCloseRequest(pWindow);
				}
				else
				{
					::TranslateMessage(&msg);
					::DispatchMessageW(&msg);
				}
			}
		}

		void WaitEvents_Platform(GE_Window* pWindow)
		{
			assert(pWindow != nullptr);

			::WaitMessage();

			PollEvents_Platform(pWindow);
		}

		void WaitEventsTimeout_Platform(GE_Window* pWindow, float64_t timeout)
		{
			assert(pWindow != nullptr);

			::MsgWaitForMultipleObjects(0, NULL, FALSE, static_cast<DWORD>(timeout * 1e3), QS_ALLEVENTS);

			PollEvents_Platform(pWindow);
		}
		//////////////////////////////////////////////

		// Window callback function (handles window messages)
		//
		LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			//GE_Window* pWindow = (GE_Window*)::GetPropW(hWnd, GE_WindowProperty);
			GE_Window* pWindow = reinterpret_cast<GE_Window*>(::GetWindowLongPtrW(hWnd, GWLP_USERDATA));

			if (pWindow == nullptr)
				return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);

			switch (uMsg)
			{
			case WM_CLOSE:
			{
				InputWindowCloseRequest(pWindow);
				break;
			}

			case WM_PAINT:
			{
				/* We were occluded, refresh our display */
				RECT rect;
				if (GetUpdateRect(pWindow->win32.handle, &rect, FALSE))
				{
					ValidateRect(pWindow->win32.handle, NULL);
				}

				InputWindowRefresh(pWindow);

				break;
			}

			case WM_SETFOCUS:
			{
				InputWindowFocus(pWindow, true);

				break;
			}

			case WM_KILLFOCUS:
			{
				if (pWindow->autoMinimize)
					MinimizeWindow_Platform(pWindow);

				InputWindowFocus(pWindow, false);
				break;
			}

			case WM_INPUTLANGCHANGE:
			{
				UpdateKeyNames_WIN32();
				break;
			}

			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
			case WM_KEYUP:
			case WM_SYSKEYUP:
			{
				int32_t key, scancode;
				const int32_t action = (HIWORD(lParam) & KF_UP) ? GE_KEY_RELEASE : GE_KEY_PRESS;
				const int32_t mods = 0;// getKeyMods();

				scancode = (HIWORD(lParam) & (KF_EXTENDED | 0xff));
				if (!scancode)
				{
					// NOTE: Some synthetic key messages have a scancode of zero
					// HACK: Map the virtual key back to a usable scancode
					scancode = MapVirtualKey(static_cast<UINT>(wParam), MAPVK_VK_TO_VSC);
				}

				key = g_KeyData.win32.keycodes[scancode];

				InputKey(pWindow, key, scancode, action, mods);

				break;
			}

			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_MBUTTONDOWN:
			case WM_XBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
			case WM_MBUTTONUP:
			case WM_XBUTTONUP:
			{
				int32_t i, button, action;

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
					action = GE_MOUSE_BUTTON_RELEASE;

				for (i = 0; i <= GE_MOUSE_BUTTON_LAST; i++)
				{
					if (pWindow->mouseButtons[i] == GE_MOUSE_BUTTON_PRESS)
						break;
				}

				if (i > GE_MOUSE_BUTTON_LAST)
					::SetCapture(hWnd);

				InputMouseButton(pWindow, button, action, 0);// getKeyMods());

				for (i = 0; i <= GE_MOUSE_BUTTON_LAST; i++)
				{
					if (pWindow->mouseButtons[i] == GE_MOUSE_BUTTON_PRESS)
						break;
				}

				if (i > GE_MOUSE_BUTTON_LAST)
					::ReleaseCapture();

				if (uMsg == WM_XBUTTONDOWN || uMsg == WM_XBUTTONUP)
					return TRUE;

				break;
			}

			case WM_MOUSEMOVE:
			{
				const int32_t x = LOWORD(lParam); // GET_X_LPARAM(lParam);
				const int32_t y = HIWORD(lParam); // GET_Y_LPARAM(lParam);

				if (false == pWindow->win32.isMouseInsideWindow)
				{
					pWindow->win32.isMouseInsideWindow = true;
					InputMouseEnterWindow(pWindow, true);
				}
				InputMouseMove(pWindow, x, y);

				break;
			}

			case WM_MOUSELEAVE:
			{
				pWindow->win32.isMouseInsideWindow = false;
				InputMouseEnterWindow(pWindow, false);

				break;
			}

			case WM_MOUSEWHEEL:
			{
				InputMouseScroll(pWindow, 0.0, static_cast<SHORT>(HIWORD(wParam)) / static_cast<float64_t>(WHEEL_DELTA));

				break;
			}

			case WM_MOUSEHWHEEL:
			{
				// This message is only sent on Windows Vista and later
				// NOTE: The X-axis is inverted for consistency with macOS and X11
				InputMouseScroll(pWindow, -(static_cast<SHORT>(HIWORD(wParam)) / static_cast<float64_t>(WHEEL_DELTA)), 0.0);

				break;
			}

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
					(pWindow->isMaximized && wParam != SIZE_RESTORED);

				if (pWindow->isMinimized != isMinimized)
					InputWindowMinimize(pWindow, isMinimized);

				if (pWindow->isMaximized != isMaximized)
					InputWindowMaximize(pWindow, isMaximized);

				//InputFramebufferSize(pWindow, LOWORD(lParam), HIWORD(lParam));
				InputWindowSize(pWindow, LOWORD(lParam), HIWORD(lParam));

				pWindow->isMinimized = isMinimized;
				pWindow->isMaximized = isMaximized;

				break; //return 0;
			}
			}

			return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
		}

	}
}