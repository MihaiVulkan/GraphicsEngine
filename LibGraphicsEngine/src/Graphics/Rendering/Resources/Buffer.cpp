#include "Buffer.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <cstring> // ::memcpy()
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;


Buffer::Buffer()
	: mpData(nullptr)
	, mSize(0)
{}

Buffer::Buffer(void* pData, uint32_t size)
	: mpData(nullptr)
	, mSize(size)
{
	Create(pData);
}

Buffer::~Buffer()
{
	Destroy();
}

void Buffer::Create(void* pData)
{
	SetData(pData, mSize);
}

void Buffer::Destroy()
{	
	if (mpData)
	{
		GE_FREE_ARRAY(mpData);
		mpData = nullptr;
	}
	mSize = 0;
}

void* Buffer::GetData() const
{
	return mpData;
}

void Buffer::SetData(void* pData, uint32_t size)
{
	if (pData)
	{
		assert(size > 0);
		mSize = size;

		mpData = GE_ALLOC_ARRAY(char_t, mSize);

		::memcpy(mpData, pData, mSize);
	}
}

const uint32_t& Buffer::GetSize() const
{
	return mSize;
}