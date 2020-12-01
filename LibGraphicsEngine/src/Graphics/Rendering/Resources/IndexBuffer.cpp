#include "IndexBuffer.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <cstring> // memcpy()
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;


IndexBuffer::IndexBuffer()
	: Buffer()
	, mUsage(Usage::U_COUNT)
	, mIndexType(IndexType::IT_COUNT)
	, mIndexCount(0)
{}

IndexBuffer::IndexBuffer(const IndexBuffer::Usage& usage, const IndexBuffer::IndexType& indexType, void* pData, uint32_t size)
	: Buffer(pData, size)
	, mUsage(usage)
	, mIndexType(indexType)
	, mIndexCount(0)
{
	Create(pData);
}

IndexBuffer::~IndexBuffer()
{
	Destroy();
}

void IndexBuffer::Create(void* pData)
{
	Buffer::SetData(pData, mSize);

	switch (mIndexType)
	{
	case IndexType::IT_UINT32:
		mIndexCount = mSize / sizeof(uint32_t);
		break;
	case IndexType::IT_UINT16:
		mIndexCount = mSize / sizeof(uint16_t);
		break;
	case IndexType::IT_UINT8:
		mIndexCount = mSize / sizeof(uint8_t);
		break;
	}
}

void IndexBuffer::Destroy()
{
	mIndexCount = 0;
	mIndexType = IndexType::IT_COUNT;
	mUsage = Usage::U_COUNT;

	Buffer::Destroy();
}

const IndexBuffer::Usage& IndexBuffer::GetUsage() const
{
	return mUsage;
}

const IndexBuffer::IndexType& IndexBuffer::GetIndexType() const
{
	return mIndexType;
}

const uint32_t& IndexBuffer::GetIndexCount() const
{
	return mIndexCount;
}