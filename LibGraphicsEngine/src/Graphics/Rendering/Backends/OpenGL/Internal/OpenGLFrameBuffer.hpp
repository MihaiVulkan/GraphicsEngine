#ifndef GRAPHICS_RENDERING_BACKENDS_OPENGL_INTERNAL_OPENGL_FRAMEBUFFER_HPP
#define GRAPHICS_RENDERING_BACKENDS_OPENGL_INTERNAL_OPENGL_FRAMEBUFFER_HPP

#include "Graphics/Rendering/Backends/OpenGL/Common/OpenGLObject.hpp"
#include <vector>

namespace GraphicsEngine
{
	namespace Graphics
	{
		/*
			*Wrapper for GL Framebuffer object*

			Framebuffer objects encapsulate the state of a framebuffer, including a collection of
			color, depth, and stencil buffers. Each such buffer is represented by a renderbuffer
			object or texture object attached to the framebuffer object.

			NOTE! The backbuffer attached to the window is implicit in OpenGL, but must be defined and used explicitly in Vulkan.
		*/
		class OpenGLFrameBuffer : OpenGLObject
		{
			GE_RTTI(GraphicsEngine::Graphics::OpenGLFrameBuffer)

		public:
			OpenGLFrameBuffer();
			virtual ~OpenGLFrameBuffer();

			void Add2DTextureAttachment(GLenum attachment, GLuint textureHandle);
			void AddRenderBufferAttachment(GLenum attachment, GLuint renderBufferHandle);

			void CheckCompleteness();

			void Bind();
			void UnBind();

			const GLuint& GetHandle() const;

		private:
			void Create();
			void Destroy();

			GLuint mHandle;
		};
	}
}

#endif // GRAPHICS_RENDERING_BACKENDS_OPENGL_INTERNAL_OPENGL_FRAMEBUFFER_HPP