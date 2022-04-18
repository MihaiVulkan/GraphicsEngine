#include "Graphics/Rendering/Resources/VertexBuffer.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;


VertexBuffer::VertexBuffer()
	: Buffer()
	, mpFormat(nullptr)
	, mVertexCount(0)
{}

VertexBuffer::VertexBuffer(VertexFormat* pFormat, Buffer::BufferUsage usage, void* pData, uint32_t size)
	: Buffer(usage, pData, size)
	, mpFormat(pFormat)
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
	mpFormat = nullptr;
	
	Buffer::Destroy();
}

VertexFormat* VertexBuffer::GetFormat() const
{
	return mpFormat;
}

const uint32_t& VertexBuffer::GetVertexCount() const
{
	return mVertexCount;
}