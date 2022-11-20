#if defined(OPENGL_RENDERER)
#include "Graphics/Rendering/Backends/OpenGL/Resources/OpenGLUniformBuffer.hpp"
#include "Graphics/Rendering/Backends/OpenGL/Internal/OpenGLBuffer.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Graphics/Rendering/Resources/UniformBuffer.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

GADRUniformBuffer::GADRUniformBuffer()
	: mpOpenGLBuffer(nullptr)
	, mpUniformBuffer(nullptr)
{}

GADRUniformBuffer::GADRUniformBuffer(Renderer* pRenderer, UniformBuffer* pUniformBuffer)
	: mpOpenGLBuffer(nullptr)
	, mpUniformBuffer(pUniformBuffer)
{
	Create();
}

GADRUniformBuffer::~GADRUniformBuffer()
{
	Destroy();
}

void GADRUniformBuffer::Create()
{
	assert(mpUniformBuffer != nullptr);
	assert(mpUniformBuffer->GetData() != nullptr);

	Buffer::BufferUsage usage = mpUniformBuffer->GetBufferUsage();
	assert(usage == Buffer::BufferUsage::GE_BU_DYNAMIC);

	//GL_MAP_WRITE_BIT bit maps to the GL_DYNAMIC_DRAW usage hint as this buffer needs to be updated per frame!
	mpOpenGLBuffer = GE_ALLOC(OpenGLBuffer)(GL_UNIFORM_BUFFER, mpUniformBuffer->GetSize(), mpUniformBuffer->GetData(), GL_MAP_WRITE_BIT);
	assert(mpOpenGLBuffer != nullptr);

	UpdateData();
}

void GADRUniformBuffer::Destroy()
{
	if (mpUniformBuffer)
	{
		mpUniformBuffer = nullptr;
	}

	GE_FREE(mpOpenGLBuffer);
}

void GADRUniformBuffer::UpdateData()
{
	assert(mpOpenGLBuffer != nullptr);
	assert(mpUniformBuffer != nullptr);
	assert(mpUniformBuffer->GetSize() <= mpOpenGLBuffer->GetSize());

	mpOpenGLBuffer->Map(mpUniformBuffer->GetSize());
	mpOpenGLBuffer->SetData(mpUniformBuffer->GetData(), mpUniformBuffer->GetSize());
	mpOpenGLBuffer->UnMap();
}

void GADRUniformBuffer::OnBind(uint32_t currentBufferIdx)
{
	UpdateData();
}

void GADRUniformBuffer::OnUnBind(uint32_t currentBufferIdx)
{
	//
}

OpenGLBuffer* GADRUniformBuffer::GetGLBuffer() const
{
	return mpOpenGLBuffer;
}
#endif // OPENGL_RENDERER