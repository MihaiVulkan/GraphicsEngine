#include "VertexBuffer.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <cstring> // memcpy()
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;


VertexBuffer::VertexBuffer()
	: Buffer()
	, mpFormat(nullptr)
	, mUsage(VertexBuffer::Usage::U_COUNT)
	, mInputRate(VertexBuffer::InputRate::IR_COUNT)
	, mVertexCount(0)
{}

VertexBuffer::VertexBuffer(VertexFormat* pFormat, VertexBuffer::Usage usage, VertexBuffer::InputRate inputRate, void* pData, uint32_t size)
	: Buffer(pData, size)
	, mpFormat(pFormat)
	, mUsage(usage)
	, mInputRate(inputRate)
	, mVertexCount(0)
{
	Create(pData);
}

VertexBuffer::~VertexBuffer()
{
	Destroy();
}

void VertexBuffer::Create(void* pData)
{
	assert(mpFormat != nullptr);

	Buffer::SetData(pData, mSize);

	mVertexCount = mSize / mpFormat->GetTotalStride();
}

void VertexBuffer::Destroy()
{
	mVertexCount = 0;
	mInputRate = InputRate::IR_COUNT;
	mUsage = Usage::U_COUNT;
	mpFormat = nullptr;
	
	Buffer::Destroy();
}

VertexFormat* VertexBuffer::GetFormat() const
{
	return mpFormat;
}

const VertexBuffer::Usage& VertexBuffer::GetUsage() const
{
	return mUsage;
}

const VertexBuffer::InputRate& VertexBuffer::GetInputRate() const
{
	return mInputRate;
}

const uint32_t& VertexBuffer::GetVertexCount() const
{
	return mVertexCount;
}