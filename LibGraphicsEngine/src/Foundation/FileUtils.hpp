#ifndef FOUNDATION_FILE_UTILS_HPP
#define FOUNDATION_FILE_UTILS_HPP

#include "Foundation/TypeDefines.hpp"
#include <string>
#include <vector>

namespace GraphicsEngine
{
	namespace FileUtils
	{
		void ReadTextFile(const std::string& filePath, std::string& fileContentOut);
		void ReadBinaryFile(const std::string& filePath, std::vector<char_t>& dataOut);
	}
}


#endif // FOUNDATION_FILE_UTILS_HPP