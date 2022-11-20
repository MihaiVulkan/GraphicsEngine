#ifndef GRAPHICS_RENDERING_BACKENDS_OPENGL_INTERNAL_OPENGL_RENDERBUFFER_HPP
#define GRAPHICS_RENDERING_BACKENDS_OPENGL_INTERNAL_OPENGL_RENDERBUFFER_HPP

#include "Graphics/Rendering/Backends/OpenGL/Common/OpenGLObject.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		/*
			*Wrapper for GL RenderBuffer object*

			Renderbuffer objects contain a single image in a format which can be rendered to.
			Renderbuffer objects are attached to framebuffer objects (see below) when
			performing off-screen rendering.
		*/
		class OpenGLRenderBuffer : OpenGLObject
		{
			GE_RTTI(GraphicsEngine::Graphics::OpenGLRenderBuffer)

		public:
			OpenGLRenderBuffer();
			explicit OpenGLRenderBuffer(GLenum internalFormat, uint32_t width, uint32_t height);
			virtual ~OpenGLRenderBuffer();

			void Bind();
			void UnBind();

			const GLuint& GetHandle() const;

		private:
			void Create(GLenum internalFormat, uint32_t width, uint32_t height);
			void Destroy();

			GLuint mHandle;
		};
	}
}

#endif // GRAPHICS_RENDERING_BACKENDS_OPENGL_INTERNAL_OPENGL_RENDERBUFFER_HPP