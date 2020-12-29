#ifndef FOUNDATION_FIXED_SINGLETON_HPP
#define FOUNDATION_FIXED_SINGLETON_HPP

#include "MemoryManagement/MemoryOperations.hpp"

// The Scott Meyers - thread-safe singleton 
// using static variables with block scope

namespace GraphicsEngine
{
	/* Singleton life time equals the application lifetime
	   At application shutdown the resources will be freed by the OS 

	   Stack memory used to store the owned object.
	*/
	template<class T>
	class FixedSingleton
	{
	public:
		static T GetInstance()
		{
			// the static instance is created only once
			static T mInstance;

			return mInstance;
		}

	protected:
		FixedSingleton();
		virtual ~FixedSingleton();

	};

	/* As we do use templates we have to provie the definition in the header */
	//////////////// IMPLEMENTATION ////////////

	template<class T>
	FixedSingleton<T>::FixedSingleton() = default;

	template<class T>
	FixedSingleton<T>::~FixedSingleton() = default;

}

#endif /* FOUNDATION_SINGLETON_HPP */