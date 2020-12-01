#include "FileUtils.hpp"
#include "MemoryManagement/MemoryOperations.hpp"
#include "Logger.hpp"
#include "fstream"
#include <cassert>

namespace GraphicsEngine
{
	namespace FileUtils
	{
		char_t* ReadFile(const char_t* pFilePath, uint32_t& sizeOut)
		{
			assert(pFilePath != nullptr);

			char_t* pFileContent = nullptr;
			std::ifstream is(pFilePath, std::ios::binary | std::ios::in | std::ios::ate);

			if (is.is_open())
			{
				int32_t size = is.tellg();
				assert(size > 0);
				is.seekg(0, std::ios::beg);

				pFileContent = GE_ALLOC_ARRAY(char_t, (size + 1)); //1 more for terminator operator
				is.read(pFileContent, size);
				is.close();

				pFileContent[size] = '\0';

				sizeOut = size + 1;
			}
			else
			{
				LOG_ERROR("Could not open shader file \"%s\"\n", pFilePath);
				return nullptr;
			}
			return pFileContent;
		}
	}
}