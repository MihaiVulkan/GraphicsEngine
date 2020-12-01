#ifndef FOUNDATION_MEMORYMANAGEMENT_MEMORY_UTILITY_HPP
#define FOUNDATION_MEMORYMANAGEMENT_MEMORY_UTILITY_HPP

#include "Foundation/TypeDefs.hpp"

namespace GraphicsEngine
{
	namespace MemoryUtility
	{
		uint64_t CalculateMemoryPadding(uint64_t baseAddress, uint64_t alignment);
	}
}

#endif /* FOUNDATION_MEMORYMANAGEMENT_MEMORY_UTILITY_HPP */