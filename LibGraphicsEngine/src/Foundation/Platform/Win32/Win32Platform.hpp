#ifndef FOUNDATION_PLATFORM_WIN32_PLATFORM_HPP
#define FOUNDATION_PLATFORM_WIN32_PLATFORM_HPP

#include "Foundation/Platform/PlatformTypes.hpp"
#include <windows.h>

namespace GraphicsEngine
{
	namespace Platform
	{
		void CreateKeyTables_WIN32();
		void UpdateKeyNames_WIN32();

		bool_t RegisterWindowClass_WIN32(GE_Window* pWindow);
		bool_t UnRegisterWindowClass_WIN32(GE_Window* pWindow);

		WCHAR* CreateWideStringFromUTF8_WIN32(const char_t* pSource);
		char_t* CreateUTF8FromWideString_WIN32(const WCHAR* pSource);
	}
}

#endif // FOUNDATION_PLATFORM_WIN32_PLATFORM_HPP