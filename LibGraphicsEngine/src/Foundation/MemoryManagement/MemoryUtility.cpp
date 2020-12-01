#include "MemoryUtility.hpp"

namespace GraphicsEngine
{
	namespace MemoryUtility
	{
		uint64_t CalculateMemoryPadding(uint64_t baseAddress, uint64_t alignment)
		{
			const uint64_t multiplier = (baseAddress / alignment) + 1;
			const uint64_t alignedAddress = multiplier * alignment;
			const uint64_t padding = alignedAddress - baseAddress;

			return padding;
		}
	}
}