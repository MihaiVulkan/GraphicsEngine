#include "Foundation/FileUtils.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"
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

			std::ifstream stream(filePath.c_str(), std::ios::binary | std::ios::in | std::ios::ate);

			if (stream.is_open() && stream.good())
			{
				int32_t size = stream.tellg();
				assert(size > 0);
				stream.seekg(0, std::ios::beg);

				std::ostringstream ss;
				ss << stream.rdbuf(); // reading data directly from the file stream to the outputstream
				fileContentOut = ss.str();

				stream.close();
			}
			else
			{
				LOG_ERROR("Could not open shader file \"%s\"\n", filePath.c_str());
			}
		}
	}
}