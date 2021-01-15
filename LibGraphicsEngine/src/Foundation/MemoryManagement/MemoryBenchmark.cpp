#include "Foundation/MemoryManagement/MemoryBenchmark.hpp"
#include "Foundation/MemoryManagement/Allocator.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <iostream>
#include <cassert>


using namespace GraphicsEngine;

MemoryBenchmark::MemoryBenchmark()
	: mOperationCount(0)
	, mTimer()
{}

MemoryBenchmark::MemoryBenchmark(uint64_t operationCount)
	: mOperationCount(operationCount)
	, mTimer(1000)
{}

MemoryBenchmark::~MemoryBenchmark()
{}

void MemoryBenchmark::SingleAllocation(Allocator* allocatorPtr, uint64_t size, uint64_t alignment)
{
	assert(nullptr != allocatorPtr);

	mTimer.Start();

	allocatorPtr->Init(1e5);

	for (uint32_t operation = 0; operation < mOperationCount; ++ operation)
	{
		allocatorPtr->Allocate(size, alignment);
	}


	mTimer.Stop();

	CollectResults(mTimer.ElapsedTimeInMiliseconds(), allocatorPtr->Peak());
}

void MemoryBenchmark::SingleFree(Allocator* allocatorPtr, uint64_t size, uint64_t alignment)
{
	assert(nullptr != allocatorPtr);

	void** allocatedMemory = GE_ALLOC_ARRAY(void*, mOperationCount);
	assert(allocatedMemory != nullptr);

	allocatorPtr->Init(1e5);

	for (uint32_t operation = 0; operation < mOperationCount; ++operation)
	{
		allocatedMemory[operation] = allocatorPtr->Allocate(size, alignment);
	}

	for (uint32_t operation = 0; operation < mOperationCount; ++operation)
	{
		allocatorPtr->Free(allocatedMemory[operation]);
	}

	GE_FREE_ARRAY(allocatedMemory);
}

void MemoryBenchmark::CollectResults(int64_t elapsedTime, uint64_t memoryPeak)
{
	float32_t operationsPerSecond = static_cast<float32_t>(mOperationCount) / elapsedTime;
	float32_t timePerOperation = static_cast<float32_t>(elapsedTime) / mOperationCount;

	// Print results
	std::cout << "---------- BENCHMARK --------- " << std::endl;
	std::cout << "Operations per sec: " << operationsPerSecond << std::endl;
	std::cout << "Time per Operation: " << timePerOperation << std::endl;
	std::cout << "Memory peak: " << memoryPeak << std::endl;
	std::cout << "---------- BENCHMARK --------- " << std::endl;
}