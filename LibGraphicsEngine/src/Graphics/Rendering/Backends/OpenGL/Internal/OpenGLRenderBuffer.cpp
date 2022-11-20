#include "Graphics/Rendering/Backends/OpenGL/Internal/OpenGLRenderBuffer.hpp"
#include "Foundation/Logger.hpp"
#include <string>
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

OpenGLRenderBuffer::OpenGLRenderBuffer()
	: mHandle(NULL)
{}

OpenGLRenderBuffer::OpenGLRenderBuffer(GLenum internalFormat, uint32_t width, uint32_t height)
	: mHandle(NULL)
{
	Create(internalFormat, width, height);
}

OpenGLRenderBuffer::~OpenGLRenderBuffer()
{
	Destroy();
}

void OpenGLRenderBuffer::Create(GLenum internalFormat, uint32_t width, uint32_t height)
{
	glCreateRenderbuffers(1, &mHandle);
	glNamedRenderbufferStorage(mHandle, internalFormat, width, height);
}

void OpenGLRenderBuffer::Destroy()
{
	if (mHandle)
	{
		glDeleteRenderbuffers(1, &mHandle);
		mHandle = NULL;
	}
}

void OpenGLRenderBuffer::Bind()
{
	glBindRenderbuffer(GL_RENDERBUFFER, mHandle);
}

void OpenGLRenderBuffer::UnBind()
{
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

const GLuint& OpenGLRenderBuffer::GetHandle() const
{
	return mHandle;
}