#ifndef FOUNDATION_FILE_UTILS_HPP
#define FOUNDATION_FILE_UTILS_HPP

#include "TypeDefs.hpp"

namespace GraphicsEngine
{
	namespace FileUtils
	{
		char_t* ReadFile(const char_t* pFilePath, uint32_t& sizeOut);
	}
}


#endif // FOUNDATION_FILE_UTILS_HPP