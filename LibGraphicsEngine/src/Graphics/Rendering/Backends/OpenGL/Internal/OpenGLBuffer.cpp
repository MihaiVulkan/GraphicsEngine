#include "Graphics/Rendering/Backends/OpenGL/Internal/OpenGLBuffer.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

OpenGLBuffer::OpenGLBuffer()
	: mHandle(NULL)
	, mSize(0)
	, mpMappedData(nullptr)
	, mType(0)
	, mAcessFlags(0)
{}

OpenGLBuffer::OpenGLBuffer(GLenum type, GLsizeiptr size, void* pData, GLbitfield flags)
	: mHandle(NULL)
	, mSize(size)
	, mpMappedData(nullptr)
	, mType(type)
	, mAcessFlags(flags)
{
	Create(pData);
}

OpenGLBuffer::~OpenGLBuffer()
{
	Destroy();
}

void OpenGLBuffer::Create(void* pData)
{
	// create the buffer
	glCreateBuffers(1, &mHandle);

	glNamedBufferStorage(mHandle, mSize, pData, mAcessFlags);
}

void OpenGLBuffer::Destroy()
{
	if (mpMappedData)
	{
		mpMappedData = nullptr;
	}

	if (mSize > 0)
	{
		mSize = 0;
	}

	mAcessFlags = 0;

	if (mHandle)
	{
		glDeleteBuffers(1, &mHandle);
		mHandle = NULL;
	}
}

void OpenGLBuffer::Map(GLsizeiptr size)
{
	// NOTE! Mapping a buffer is more efficient as we avoid extra copying!

	mpMappedData = (uint8_t*)glMapNamedBufferRange(mHandle, 0, size, mAcessFlags);
	assert(mpMappedData != nullptr);
}

void OpenGLBuffer::UnMap()
{
	glUnmapNamedBuffer(mHandle);
}

void OpenGLBuffer::SetData(void* pData, GLsizeiptr size)
{
	assert(pData != nullptr);
	assert(size > 0);
	assert(mpMappedData != nullptr);

	::memcpy(mpMappedData, pData, size);
}

void OpenGLBuffer::Bind()
{
	glBindBuffer(mType, mHandle);
}

void OpenGLBuffer::UnBind()
{
	glBindBuffer(mType, 0);
}

void OpenGLBuffer::CopyTo(OpenGLBuffer* pDestBuffer)
{
	assert(pDestBuffer != nullptr);

	glCopyNamedBufferSubData(mHandle, pDestBuffer->GetHandle(), 0, 0, mSize);
}

GLenum OpenGLBuffer::GetType() const
{
	return mType;
}

const GLuint& OpenGLBuffer::GetHandle() const
{
	return mHandle;
}

GLsizeiptr OpenGLBuffer::GetSize() const
{
	return mSize;
}

void* OpenGLBuffer::GetData() const
{
	return mpMappedData;
}