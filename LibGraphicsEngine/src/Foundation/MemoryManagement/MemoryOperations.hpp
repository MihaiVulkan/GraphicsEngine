//#ifndef FOUNDATION_MEMORYMANAGEMENT_MEMORY_OPERATIONS_HPP
//#define FOUNDATION_MEMORYMANAGEMENT_MEMORY_OPERATIONS_HPP

#include "AppConfig.hpp"
#include "LiniarAllocator.hpp"
#include <new>

#define GE_ALLOC(T) ::new (T)
#define GE_ALLOC_ARRAY(T, size) ::new T[size] 
#define GE_FREE(T) if (T) { ::delete (T); }
#define GE_FREE_ARRAY(T) if (T) { ::delete[] (T); }


#ifdef ENABLE_ALLOCATOR

// we use the LiniarAllocator to allocate all the needed memory for our engine/app
extern GraphicsEngine::LiniarAllocator gGlobalAllocator;

// we override the global new/delete operators

void* operator new (std::size_t count)
{
	return gGlobalAllocator.Allocate(count);
}

void* operator new (std::size_t count, const std::nothrow_t& tag)
{
	return gGlobalAllocator.Allocate(count);
}

void* operator new[] (std::size_t count)
{
	return gGlobalAllocator.Allocate(count);
}

void* operator new[] (std::size_t count, const std::nothrow_t& tag)
{
	return gGlobalAllocator.Allocate(count);
}


void operator delete (void* ptr)
{
	//
}

void operator delete (void* ptr, const std::nothrow_t& tag)
{
	//
}

void operator delete[] (void* ptr)
{
	//
}

void operator delete[] (void* ptr, const std::nothrow_t& tag)
{
	//
}

#endif // ENABLE_ALLOCATOR

//#endif // FOUNDATION_MEMORYMANAGEMENT_MEMORY_OPERATIONS_HPP