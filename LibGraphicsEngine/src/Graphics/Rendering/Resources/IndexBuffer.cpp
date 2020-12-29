#include "IndexBuffer.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <cstring> // memcpy()
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;


IndexBuffer::IndexBuffer()
	: Buffer()
	, mIndexType(IndexType::GE_IT_COUNT)
	, mIndexCount(0)
	, mIsRestartPrimitive(false)
{}

IndexBuffer::IndexBuffer(const Buffer::BufferUsage& usage, const IndexBuffer::IndexType& indexType, void* pData, uint32_t size)
	: Buffer(usage ,pData, size)
	, mIndexType(indexType)
	, mIndexCount(0)
	, mIsRestartPrimitive(false)
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
	case IndexType::GE_IT_UINT32:
		mIndexCount = mSize / sizeof(uint32_t);
		break;
	case IndexType::GE_IT_UINT16:
		mIndexCount = mSize / sizeof(uint16_t);
		break;
	case IndexType::GE_IT_UINT8:
		mIndexCount = mSize / sizeof(uint8_t);
		break;
	}
}

void IndexBuffer::Destroy()
{
	mIsRestartPrimitive = false;
	mIndexCount = 0;
	mIndexType = IndexType::GE_IT_COUNT;
	mUsage = BufferUsage::GE_BU_COUNT;

	Buffer::Destroy();
}

const IndexBuffer::IndexType& IndexBuffer::GetIndexType() const
{
	return mIndexType;
}

const uint32_t& IndexBuffer::GetIndexCount() const
{
	return mIndexCount;
}

const bool_t& IndexBuffer::GetIsRestartPrimitive() const
{
	return mIsRestartPrimitive;
}

void IndexBuffer::SetIsRestartPrimitive(bool_t value)
{
	mIsRestartPrimitive = value;
}