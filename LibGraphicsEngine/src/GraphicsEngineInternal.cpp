#include "GraphicsEngineInternal.hpp"
#include "AppConfig.hpp"
#include "Foundation/MemoryManagement/LiniarAllocator.hpp"

using namespace GraphicsEngine;

void GraphicsEngine::Init()
{
#ifdef ENABLE_ALLOCATOR
	LOG_INFO("Global Allocator uses 1e9 bytes as memory buffer!");

	gAllocator.Init(1e9); //TODO - make configurable
#endif // ENABLE_ALLOCATOR
}

void GraphicsEngine::Terminate()
{
#ifdef ENABLE_ALLOCATOR
	gAllocator.Terminate();
#endif // ENABLE_ALLOCATOR
}