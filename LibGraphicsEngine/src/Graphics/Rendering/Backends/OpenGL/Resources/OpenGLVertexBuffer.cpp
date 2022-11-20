#if defined(OPENGL_RENDERER)
#include "Graphics/Rendering/Backends/OpenGL/Resources/OpenGLVertexBuffer.hpp"
#include "Graphics/Rendering/Backends/OpenGL/Internal/OpenGLBuffer.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

GADRVertexBuffer::GADRVertexBuffer()
	: mpOpenGLBuffer(nullptr)
	, mpVertexBuffer(nullptr)
{}

GADRVertexBuffer::GADRVertexBuffer(Renderer* pRenderer, VertexBuffer* pVertexBuffer)
	: mpOpenGLBuffer(nullptr)
	, mpVertexBuffer(pVertexBuffer)
{
	Create();
}

GADRVertexBuffer::~GADRVertexBuffer()
{
	Destroy();
}

void GADRVertexBuffer::Create()
{
	assert(mpVertexBuffer != nullptr);
	assert(mpVertexBuffer->GetData() != nullptr);

	Buffer::BufferUsage usage = mpVertexBuffer->GetBufferUsage();
	assert(usage == Buffer::BufferUsage::GE_BU_STATIC);

	mpOpenGLBuffer = GE_ALLOC(OpenGLBuffer)(GL_ARRAY_BUFFER, mpVertexBuffer->GetSize(), mpVertexBuffer->GetData());
	assert(mpOpenGLBuffer != nullptr);
}

void GADRVertexBuffer::Destroy()
{
	if (mpVertexBuffer)
	{
		mpVertexBuffer = nullptr;
	}

	GE_FREE(mpOpenGLBuffer);
}

void GADRVertexBuffer::OnBind(uint32_t currentBufferIdx)
{
	assert(mpOpenGLBuffer != nullptr);

	mpOpenGLBuffer->Bind();
}

void GADRVertexBuffer::OnUnBind(uint32_t currentBufferIdx)
{
	assert(mpOpenGLBuffer != nullptr);

	mpOpenGLBuffer->UnBind();
}

const Buffer::BufferUsage& GADRVertexBuffer::GetBufferUsage() const
{
	assert(mpVertexBuffer != nullptr);

	return mpVertexBuffer->GetBufferUsage();
}
#endif // OPENGL_RENDERER