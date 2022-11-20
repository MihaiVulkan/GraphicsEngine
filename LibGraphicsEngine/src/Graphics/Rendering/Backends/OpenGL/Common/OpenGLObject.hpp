#ifndef GRAPHICS_RENDERING_BACKENDS_OPENGL_COMMON_OPENGL_OBJECT_HPP
#define GRAPHICS_RENDERING_BACKENDS_OPENGL_COMMON_OPENGL_OBJECT_HPP

#include "Foundation/Object.hpp"
#include "Foundation/Platform/GLLoader/GLLoader.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class OpenGLObject : public Object
		{
		public:
			DEFAULT_CLASS(OpenGLObject)
		};

	}
}

#endif // GRAPHICS_RENDERING_BACKENDS_OPENGL_COMMON_OPENGL_OBJECT_HPP