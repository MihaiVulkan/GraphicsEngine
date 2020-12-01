#ifndef FOUNDATION_MEMORYMANAGEMENT_ALLOCATOR_HPP
#define FOUNDATION_MEMORYMANAGEMENT_ALLOCATOR_HPP

/* 
   info: 
   https://github.com/mtrebi/memory-allocators

   http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2271.html
*/

#include "Foundation/TypeDefs.hpp"
#include "Foundation/NoCopyNoMove.hpp"
#include "Foundation/RTTI.hpp"

namespace GraphicsEngine
{
	/* Base class for all Allocators */
	class Allocator : public RTTI
	{
		GE_RTTI(GraphicsEngine::Allocator)

	public:
		Allocator()
			: mTotalSize(0), mUsed(0), mPeak(0)
		{}

		Allocator(uint64_t totalSize)
			: mTotalSize(totalSize), mUsed(0), mPeak(0)
		{}

		virtual ~Allocator()
		{
			mTotalSize = 0;
			mUsed = 0;
			mPeak = 0;
		}

		uint64_t TotalSize() { return mTotalSize; }
		uint64_t Used() { return mUsed; }
		uint64_t Peak() { return mPeak; }

		virtual void Init() = 0;
		virtual void Reset() = 0;

		virtual void* Allocate(uint64_t size, uint64_t alignment = 0) = 0;
		virtual void Free(void* ptr) = 0;

	protected:
		uint64_t mTotalSize;
		uint64_t mUsed;
		uint64_t mPeak;

	private:
		NO_COPY_NO_MOVE(Allocator)
	};
}
#endif /* FOUNDATION_MEMORYMANAGEMENT_ALLOCATOR_HPP */