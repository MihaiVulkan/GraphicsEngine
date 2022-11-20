#include "Graphics/Rendering/Backends/OpenGL/Internal/OpenGLVertexArrayObject.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

OpenGLVertexArrayObject::OpenGLVertexArrayObject()
	: mHandle(NULL)
{
	Create();
}

OpenGLVertexArrayObject::~OpenGLVertexArrayObject()
{
	Destroy();
}

void OpenGLVertexArrayObject::Create()
{
	glCreateVertexArrays(1, &mHandle);
}

void OpenGLVertexArrayObject::Destroy()
{
	if (mHandle)
	{
		glDeleteVertexArrays(1, &mHandle);
		mHandle = NULL;
	}
}

void OpenGLVertexArrayObject::Bind()
{
	glBindVertexArray(mHandle);
}

void OpenGLVertexArrayObject::UnBind()
{
	glBindVertexArray(0);
}

const GLuint& OpenGLVertexArrayObject::GetHandle() const
{
	return mHandle;
}