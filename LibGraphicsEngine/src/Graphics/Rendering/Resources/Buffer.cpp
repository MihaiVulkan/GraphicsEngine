#include "Graphics/Rendering/Resources/Buffer.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <cstring> // ::memcpy()
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;


Buffer::Buffer()
	: mUsage(BufferUsage::GE_BU_COUNT)
	, mpData(nullptr)
	, mSize(0)
{}

Buffer::Buffer(BufferUsage usage, void* pData, uint32_t size)
	: mUsage(usage)
	, mpData(nullptr)
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
	}
	mSize = 0;
	mUsage = BufferUsage::GE_BU_COUNT;
}

const Buffer::BufferUsage& Buffer::GetBufferUsage() const
{
	return mUsage;
}

void* Buffer::GetData() const
{
	return mpData;
}

//TODO - maybe replace void* array with std::vector<uint8_t> ?
void Buffer::SetData(void* pData, uint32_t size)
{
	if (pData)
	{
		assert(size > 0);
		mSize = size;

		mpData = GE_ALLOC_ARRAY(uint8_t, mSize);
		assert(mpData != nullptr);

		::memcpy(mpData, pData, mSize);
	}
}

const uint32_t& Buffer::GetSize() const
{
	return mSize;
}