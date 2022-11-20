#ifndef GRAPHICS_RENDERING_BACKENDS_OPENGL_INTERNAL_OPENGL_DEBUG_HPP
#define GRAPHICS_RENDERING_BACKENDS_OPENGL_INTERNAL_OPENGL_DEBUG_HPP

#include "Foundation/TypeDefines.hpp"
#include "Foundation/Platform/GLLoader/GLLoader.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		/*
			Debug wrapper when working with OpenGL errors
		*/
		namespace OpenGLDebug
		{

			void CALLBACK DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

		}
	}
}

#endif // GRAPHICS_RENDERING_BACKENDS_OPENGL_INTERNAL_OPENGL_DEBUG_HPP