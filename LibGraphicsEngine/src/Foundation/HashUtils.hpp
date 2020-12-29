#ifndef FOUNDATION_HASH_UTILS_HPP
#define FOUNDATION_HASH_UTILS_HPP

#include "TypeDefs.hpp"
#include <typeinfo> //std::type_info, std::typeid()

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

		template <typename T>
		static std::size_t GetTypeHash()
		{
			//const std::type_info& ti = typeid(T);
			//return ti.hash_code();

			return typeid(T).hash_code();
		}

	}
}

#endif // FOUNDATION_HASH_UTILS_HPP