#include "Graphics/Rendering/Backends/OpenGL/Internal/OpenGLFrameBuffer.hpp"
#include "Foundation/Logger.hpp"
#include <string>
#include <vector>
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

OpenGLFrameBuffer::OpenGLFrameBuffer()
	: mHandle(NULL)
{
	Create();
}

OpenGLFrameBuffer::~OpenGLFrameBuffer()
{
	Destroy();
}

void OpenGLFrameBuffer::Create()
{
	glCreateFramebuffers(1, &mHandle);
}

void OpenGLFrameBuffer::Destroy()
{
	if (mHandle)
	{
		glDeleteFramebuffers(1, &mHandle);
		mHandle = NULL;
	}
}

void OpenGLFrameBuffer::Add2DTextureAttachment(GLenum attachment, GLuint textureHandle)
{
	// This API takes care af all texture types: 2D, 3D, cubemap, arrays
	glNamedFramebufferTexture(mHandle, attachment, textureHandle, 0);
}

void OpenGLFrameBuffer::AddRenderBufferAttachment(GLenum attachment, GLuint renderBufferHandle)
{
	glNamedFramebufferRenderbuffer(mHandle, attachment, GL_RENDERBUFFER, renderBufferHandle);
}

void OpenGLFrameBuffer::CheckCompleteness()
{
	GLenum ret = glCheckNamedFramebufferStatus(mHandle, GL_FRAMEBUFFER);

	std::string retStr;
	switch (ret)
	{
	case GL_FRAMEBUFFER_UNDEFINED:
		LOG_ERROR("GL_FRAMEBUFFER_UNDEFINED - specified framebuffer is the default read or draw framebuffer, but the default framebuffer does not exist!");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		LOG_ERROR("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT - at least one of the framebuffer attachment points is framebuffer incomplete!");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		LOG_ERROR("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT - the framebuffer does not have at least one image attached to it!");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		LOG_ERROR("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER - the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for any color attachment point(s) named by GL_DRAW_BUFFERi!");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		LOG_ERROR("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER - GL_READ_BUFFER is not GL_NONE and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for the color attachment point named by GL_READ_BUFFER!");
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED:
		LOG_ERROR("GL_FRAMEBUFFER_UNSUPPORTED - the combination of internal formats of the attached images violates an implementation-dependent set of restrictions!");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		LOG_ERROR("GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE - the value of GL_RENDERBUFFER_SAMPLES is not the same for all attached renderbuffers; if the value of GL_TEXTURE_SAMPLES is the not same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_RENDERBUFFER_SAMPLES does not match the value of GL_TEXTURE_SAMPLES!");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
		LOG_ERROR("GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS - any framebuffer attachment is layered, and any populated attachment is not layered, or if all populated color attachments are not from textures of the same target!");
		break;
	case GL_FRAMEBUFFER_COMPLETE:
		LOG_INFO("FBO %d is complete!", mHandle);
		break;
	default:
		LOG_ERROR("Unknown error!");
		break;
	}
}

void OpenGLFrameBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, mHandle);
}

void OpenGLFrameBuffer::UnBind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

const GLuint& OpenGLFrameBuffer::GetHandle() const
{
	return mHandle;
}