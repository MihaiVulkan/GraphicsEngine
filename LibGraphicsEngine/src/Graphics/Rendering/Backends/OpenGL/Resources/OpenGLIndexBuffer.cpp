#if defined(OPENGL_RENDERER)
#include "Graphics/Rendering/Backends/OpenGL/Resources/OpenGLIndexBuffer.hpp"
#include "Graphics/Rendering/Backends/OpenGL/Internal/OpenGLBuffer.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

GADRIndexBuffer::GADRIndexBuffer()
	: mpOpenGLBuffer(nullptr)
	, mpIndexBuffer(nullptr)
{}

GADRIndexBuffer::GADRIndexBuffer(Renderer* pRenderer, IndexBuffer* pIndexBuffer)
	: mpOpenGLBuffer(nullptr)
	, mpIndexBuffer(pIndexBuffer)
{
	Create();
}

GADRIndexBuffer::~GADRIndexBuffer()
{
	Destroy();
}

void GADRIndexBuffer::Create()
{
	assert(mpIndexBuffer != nullptr);
	assert(mpIndexBuffer->GetData() != nullptr);

	Buffer::BufferUsage usage = mpIndexBuffer->GetBufferUsage();
	assert(usage == Buffer::BufferUsage::GE_BU_STATIC);
	
	mpOpenGLBuffer = GE_ALLOC(OpenGLBuffer)(GL_ELEMENT_ARRAY_BUFFER, mpIndexBuffer->GetSize(), mpIndexBuffer->GetData());
	assert(mpOpenGLBuffer != nullptr);
}

void GADRIndexBuffer::Destroy()
{
	if (mpIndexBuffer)
	{
		mpIndexBuffer = nullptr;
	}

	GE_FREE(mpOpenGLBuffer);
}

void GADRIndexBuffer::OnBind(uint32_t currentBufferIdx)
{
	assert(mpOpenGLBuffer != nullptr);

	mpOpenGLBuffer->Bind();
}

void GADRIndexBuffer::OnUnBind(uint32_t currentBufferIdx)
{

}

const Buffer::BufferUsage& GADRIndexBuffer::GetBufferUsage() const
{
	assert(mpIndexBuffer != nullptr);

	return mpIndexBuffer->GetBufferUsage();
}

const IndexBuffer::IndexType& GADRIndexBuffer::GetIndexType() const
{
	assert(mpIndexBuffer != nullptr);

	return mpIndexBuffer->GetIndexType();
}
#endif // OPENGL_RENDERER