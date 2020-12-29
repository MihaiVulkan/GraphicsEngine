#include "VertexBuffer.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <cstring> // memcpy()
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;


VertexBuffer::VertexBuffer()
	: Buffer()
	, mpFormat(nullptr)
	, mInputRate(VertexBuffer::InputRate::GE_IR_COUNT)
	, mVertexCount(0)
{}

VertexBuffer::VertexBuffer(VertexFormat* pFormat, Buffer::BufferUsage usage, VertexBuffer::InputRate inputRate, void* pData, uint32_t size)
	: Buffer(usage, pData, size)
	, mpFormat(pFormat)
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

	mVertexCount = mSize / mpFormat->GetVertexTotalStride();
}

void VertexBuffer::Destroy()
{
	mVertexCount = 0;
	mInputRate = InputRate::GE_IR_COUNT;
	mpFormat = nullptr;
	
	Buffer::Destroy();
}

VertexFormat* VertexBuffer::GetFormat() const
{
	return mpFormat;
}

const VertexBuffer::InputRate& VertexBuffer::GetInputRate() const
{
	return mInputRate;
}

const uint32_t& VertexBuffer::GetVertexCount() const
{
	return mVertexCount;
}