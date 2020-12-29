#include "MemoryOperations.hpp"

#ifdef ENABLE_ALLOCATOR
LiniarAllocator gAllocator;

using namespace GraphicsEngine;

// we override the global new/delete operators

void* operator new (std::size_t count)
{
	return gAllocator.Allocate(count);
}

void* operator new (std::size_t count, const std::nothrow_t& tag)
{
	return gAllocator.Allocate(count);
}

void* operator new[](std::size_t count)
{
	return gAllocator.Allocate(count);
}

void* operator new[](std::size_t count, const std::nothrow_t& tag)
{
	return gAllocator.Allocate(count);
}


void operator delete (void* ptr)
{
	return gAllocator.Free(ptr);
}

void operator delete (void* ptr, const std::nothrow_t& tag)
{
	return gAllocator.Free(ptr);
}

void operator delete[](void* ptr)
{
	return gAllocator.Free(ptr);
}

void operator delete[](void* ptr, const std::nothrow_t& tag)
{
	return gAllocator.Free(ptr);
}

#endif // ENABLE_ALLOCATOR