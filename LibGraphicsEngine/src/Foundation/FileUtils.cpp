#include "FileUtils.hpp"
#include "MemoryManagement/MemoryOperations.hpp"
#include "Logger.hpp"
#include <fstream>
#include <sstream>
#include <cassert>

namespace GraphicsEngine
{
	namespace FileUtils
	{
		void ReadFile(const std::string& filePath, std::string& fileContentOut)
		{
			assert(filePath.empty() == false);

			std::ifstream is(filePath.c_str(), std::ios::binary | std::ios::in | std::ios::ate);

			if (is.is_open())
			{
				int32_t size = is.tellg();
				assert(size > 0);
				is.seekg(0, std::ios::beg);

				std::ostringstream ss;
				ss << is.rdbuf(); // reading data directly form the file stream to the outputstream
				fileContentOut = ss.str();

				is.close();
			}
			else
			{
				LOG_ERROR("Could not open shader file \"%s\"\n", filePath.c_str());
			}
		}
	}
}