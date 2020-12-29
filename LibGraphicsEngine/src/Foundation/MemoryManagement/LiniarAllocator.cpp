#include "LiniarAllocator.hpp"
#include "MemoryUtility.hpp"
#include <cstdlib> // malloc(), free()
#include <algorithm> // std::max
#include <cassert>

using namespace GraphicsEngine;

LiniarAllocator::LiniarAllocator()
	: Allocator()
	, mpMemoryBuffer(nullptr)
	, mOffset(0)
{}

LiniarAllocator::LiniarAllocator(uint64_t totalSize)
	: Allocator(totalSize)
	, mpMemoryBuffer(nullptr)
	, mOffset(0)
{}

LiniarAllocator::~LiniarAllocator()
{
	Terminate();
}

void LiniarAllocator::Init(uint64_t totalSize)
{
	mTotalSize = totalSize;

	mpMemoryBuffer = ::malloc(mTotalSize);
}

void LiniarAllocator::Reset()
{
	mOffset = 0;
	mUsed = 0;
	mPeak = 0;
}

void LiniarAllocator::Terminate()
{
	if (mpMemoryBuffer)
	{
		::free(mpMemoryBuffer);
		mpMemoryBuffer = nullptr;

		Reset();
	}
}

void* LiniarAllocator::Allocate(uint64_t size, uint64_t alignment)
{
	assert(size > 0);

	const uint64_t currentAddress = reinterpret_cast<uint64_t>(mpMemoryBuffer) + mOffset;

	// in case alignment is needed we find the correct padding
	uint64_t padding = 0;
	if ((alignment > 0) && (mOffset % alignment != 0))
	{
		padding = MemoryUtility::CalculateMemoryPadding(currentAddress, alignment);
	}

	// check if there is space left to allocate
	if (mOffset + padding + size > mTotalSize)
	{
		return nullptr;
	}

	const uint64_t nextAvailableAddress = currentAddress + padding;

	mOffset += (size + padding);
	mUsed = mOffset;
	mPeak = std::max(mPeak, mUsed);

	return reinterpret_cast<void*>(nextAvailableAddress);
}

void LiniarAllocator::Free(void* ptr)
{
	// This allocator does not support this operation!
	// Do nothing
	//assert(false && "Use Reset() method");
}
