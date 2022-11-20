#ifndef GRAPHICS_RENDERING_BACKENDS_OPENGL_RESOURCES_OPENGL_RESOURCE_HPP
#define GRAPHICS_RENDERING_BACKENDS_OPENGL_RESOURCES_OPENGL_RESOURCE_HPP

#if defined(OPENGL_RENDERER)
#include "Graphics/Rendering/Backends/OpenGL/Common/OpenGLObject.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		// OpenGL implementation of the GADR Resource base class
		class GADRResource : public OpenGLObject
		{
			GE_RTTI(GraphicsEngine::Graphics::GADRResource)

		public:
			GADRResource();
			virtual ~GADRResource();

			virtual void OnBind(uint32_t currentBufferIdx = 0) {};
			virtual void OnUnBind(uint32_t currentBufferIdx = 0) {};
		};
	}

}
#endif // OPENGL_RENDERER

#endif // GRAPHICS_RENDERING_BACKENDS_OPENGL_RESOURCES_OPENGL_RESOURCE_HPP