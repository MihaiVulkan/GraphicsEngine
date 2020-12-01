#ifndef FOUNDATION_SINGLETON_HPP
#define FOUNDATION_SINGLETON_HPP

#include "MemoryManagement/MemoryOperations.hpp"

// The Scott Meyers - thread-safe singleton 
// using static variables with block scope

namespace GraphicsEngine
{
	/* Singleton life time exquals the application lifetime
	   At application shutdown the resources will be freed by the OS 
	*/
	template<class T>
	class Singleton
	{
	public:
		static T* GetInstance()
		{
			// the static instance is created only once
			static T* mInstance = nullptr;

			if (mInstance == nullptr)
			{
				mInstance = GE_ALLOC(T);
			}

			return mInstance;
		}

		/*static void Terminate()
		{
			GE_FREE(mInstance);
		}*/

	protected:
		Singleton();
		virtual ~Singleton();

	};

	/* As we do use templates we have to provie the definition in the header */
	//////////////// IMPLEMENTATION ////////////

	template<class T>
	Singleton<T>::Singleton() = default;

	template<class T>
	Singleton<T>::~Singleton() = default;

}

#endif /* FOUNDATION_SINGLETON_HPP */