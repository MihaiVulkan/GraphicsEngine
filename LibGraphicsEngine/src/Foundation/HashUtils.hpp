#ifndef FOUNDATION_HASH_UTILS_HPP
#define FOUNDATION_HASH_UTILS_HPP

#include "TypeDefs.hpp"

namespace GraphicsEngine
{
	namespace HashUtils
	{
		// we hash the memory address of the pointer itself
		template<typename T>
		struct PointerHash
		{
			inline size_t operator()(const T* pointer) const
			{
				auto addr = reinterpret_cast<uintptr_t>(pointer);
#if SIZE_MAX < UINTPTR_MAX
				/* size_t is not large enough to hold the pointer’s memory address */
				addr %= SIZE_MAX; /* truncate the address so it is small enough to fit in a size_t */
#endif
				return addr;
			}
		};
	}
}

#endif // FOUNDATION_HASH_UTILS_HPP