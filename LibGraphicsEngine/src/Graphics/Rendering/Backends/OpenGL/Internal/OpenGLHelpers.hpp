#ifndef GRAPHICS_RENDERING_BACKENDS_OPENGL_INTERNAL_OPENGL_HELPERS_HPP
#define GRAPHICS_RENDERING_BACKENDS_OPENGL_INTERNAL_OPENGL_HELPERS_HPP

#include "Foundation/TypeDefines.hpp"
#include "Foundation/Logger.hpp"
#include "Foundation/Platform/GLLoader/GLLoader.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <string>
#include <cassert>

namespace GraphicsEngine
{
	namespace Graphics
	{
		/*
			Wrapper for several OpenGL helper functionalities
		*/
		namespace OpenGLHelpers
		{

#define OFFSETOF(s,m) ((::size_t)&reinterpret_cast<char const volatile&>((((s*)0)->m)))

			std::string ErrorString(GLenum errorCode);

#define GL_CHECK_RESULT(funCall)																				\
			{																									\
				funCall;																						\
				GLenum res = glGetError();																		\
				if (res != GL_NO_ERROR)																			\
				{																								\
					LOG_DEBUG("Fatal : GLResult is \"%s\"", OpenGLHelpers::ErrorString(res).c_str());			\
				}																								\
			}

			void ListRendererInfo();
			void ListSupportedExtensions();
			void ListCapabilities();
		}
	}
}

#endif // GRAPHICS_RENDERING_BACKENDS_OPENGL_INTERNAL_OPENGL_HELPERS_HPP