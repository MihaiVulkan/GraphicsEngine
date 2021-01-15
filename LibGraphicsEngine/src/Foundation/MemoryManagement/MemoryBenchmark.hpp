#ifndef FOUNDATION_MEMORYMANAGEMENT_MEMORY_BENCHMARK_HPP
#define FOUNDATION_MEMORYMANAGEMENT_MEMORY_BENCHMARK_HPP

#include "Foundation/TypeDefines.hpp"
#include "Foundation/NoCopyNoMove.hpp"
#include "Foundation/Timer.hpp"

namespace GraphicsEngine
{
	class Allocator;

	class MemoryBenchmark
	{
	public:
		MemoryBenchmark();
		explicit MemoryBenchmark(uint64_t operationCount);
		virtual ~MemoryBenchmark();

		void SingleAllocation(Allocator* allocatorPtr, uint64_t size, uint64_t alignment);
		void SingleFree(Allocator* allocatorPtr, uint64_t size, uint64_t alignment);

		void CollectResults(int64_t elapsedTime, uint64_t memoryPeak);

	private:
		NO_COPY_NO_MOVE(MemoryBenchmark)

		uint64_t mOperationCount;
		Timer mTimer;
	};
}
#endif /* FOUNDATION_MEMORYMANAGEMENT_ */