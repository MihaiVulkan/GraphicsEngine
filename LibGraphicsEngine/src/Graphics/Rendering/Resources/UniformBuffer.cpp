#include "UniformBuffer.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <cstring> // memcpy()
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;


UniformBuffer::UniformBuffer()
	: mUniformUsage(UniformUsage::UU_COUNT)
	//, mpData(nullptr)
	//, mSize(0)
{}

UniformBuffer::UniformBuffer(UniformUsage uniformUsage)
	: mUniformUsage(uniformUsage)
	//, mpData(nullptr)
	//, mSize(0)
{
	Create();
}

UniformBuffer::~UniformBuffer()
{
	Destroy();
}

void UniformBuffer::Create()
{
	SetupUniforms();
}

void UniformBuffer::Destroy()
{
	mUniformUsage = UniformUsage::UU_COUNT;
	
	/*if (mpData)
	{
		GE_FREE_ARRAY(mpData);
		mpData = nullptr;
	}
	mSize = 0;*/
}

void UniformBuffer::SetupUniforms()
{
	::memset(&Uniforms, 0, sizeof(Uniforms));

	switch(mUniformUsage)
	{
	case UniformUsage::UU_PVM:
		//SetData(&mPVMVert, sizeof(mPVMVert));
		break;
	
		//others

	default:
		;
	}
}

const UniformBuffer::UniformUsage& UniformBuffer::GetUniformUsage() const
{
	return mUniformUsage;
}

void* UniformBuffer::GetData()
{
	return &Uniforms;
}

//void* UniformBuffer::GetData() const
//{
//	eturn mpData;
//}

//void UniformBuffer::SetData(void* pData, uint32_t size)
//{
//	if (pData)
//	{
//		assert(size > 0);
//
//		mSize = size;
//
//		mpData = GE_ALLOC_ARRAY(char_t, mSize);
//
//		::memcpy(mpData, pData, mSize);
//	}
//}

//const uint32_t& UniformBuffer::GetSize() const
//{
//	return mSize;
//}

uint32_t UniformBuffer::GetSize() const
{
	return sizeof(Uniforms);
}