//#ifndef FOUNDATION_MEMORYMANAGEMENT_MEMORY_OPERATIONS_HPP
//#define FOUNDATION_MEMORYMANAGEMENT_MEMORY_OPERATIONS_HPP

#include "Core/AppConfig.hpp"
#include "Foundation/MemoryManagement/LiniarAllocator.hpp"
#ifndef ENABLE_ALLOCATOR
#include <new>
#endif // ENABLE_ALLOCATOR

#define GE_ALLOC(T) ::new (T)
#define GE_ALLOC_ARRAY(T, size) ::new T[size] 
#define GE_FREE(T) if (T) { ::delete (T); T = nullptr; }
#define GE_FREE_ARRAY(T) if (T) { ::delete[] (T); T = nullptr; }


#ifdef ENABLE_ALLOCATOR
using namespace GraphicsEngine;

extern LiniarAllocator gAllocator;

// we override the global new/delete operators
//
//void* operator new (std::size_t count);
//void* operator new (std::size_t count, const std::nothrow_t & tag);
//void* operator new[](std::size_t count);
//void* operator new[](std::size_t count, const std::nothrow_t& tag);
//
//void operator delete (void* ptr);
//void operator delete (void* ptr, const std::nothrow_t & tag);
//void operator delete[](void* ptr);
//void operator delete[](void* ptr, const std::nothrow_t& tag);

#endif // ENABLE_ALLOCATOR

//#endif // FOUNDATION_MEMORYMANAGEMENT_MEMORY_OPERATIONS_HPP