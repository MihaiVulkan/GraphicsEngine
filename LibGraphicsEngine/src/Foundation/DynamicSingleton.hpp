#ifndef FOUNDATION_DYNAMIC_SINGLETON_HPP
#define FOUNDATION_DYNAMIC_SINGLETON_HPP

#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <cassert>

// The Scott Meyers - thread-safe singleton 
// using static variables with block scope

namespace GraphicsEngine
{
	/* Singleton life time exquals the application lifetime
	   At application shutdown the resources will be freed by the OS 

	   Heap memory used to store the owned object.
	*/
	template<class T>
	class DynamicSingleton
	{
	public:
		static T* GetInstance()
		{
			// the static instance is created only once
			static T* mpInstance = nullptr;

			if (mpInstance == nullptr)
			{
				mpInstance = GE_ALLOC(T);
				assert(mpInstance != nullptr);
			}

			return mpInstance;
		}

		/*static void Terminate()
		{
			GE_FREE(mInstance);
		}*/

	protected:
		DynamicSingleton();
		virtual ~DynamicSingleton();

	};

	/* As we do use templates we have to provie the definition in the header */
	//////////////// IMPLEMENTATION ////////////

	template<class T>
	DynamicSingleton<T>::DynamicSingleton() = default;

	template<class T>
	DynamicSingleton<T>::~DynamicSingleton() = default;

}

#endif /* FOUNDATION_DYNAMIC_SINGLETON_HPP */