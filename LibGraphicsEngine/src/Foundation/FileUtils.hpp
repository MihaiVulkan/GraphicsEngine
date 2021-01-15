#ifndef FOUNDATION_FILE_UTILS_HPP
#define FOUNDATION_FILE_UTILS_HPP

#include "Foundation/TypeDefines.hpp"
#include <string>

namespace GraphicsEngine
{
	namespace FileUtils
	{
		void ReadFile(const std::string& filePath, std::string& fileContentOut);
	}
}


#endif // FOUNDATION_FILE_UTILS_HPP