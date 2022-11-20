#ifndef GRAPHICS_RENDERING_BACKENDS_OPENGL_RESOURCES_OPENGL_TEXTURE_HPP
#define GRAPHICS_RENDERING_BACKENDS_OPENGL_RESOURCES_OPENGL_TEXTURE_HPP

#if defined(OPENGL_RENDERER)
#include "Graphics/Rendering/Backends/OpenGL/Resources/OpenGLResource.hpp"
#include "Graphics/Rendering/Resources/Texture.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Renderer;
		class OpenGLTextureObject;
		class OpenGLSampler;
		class Texture;

		// OpenGL implementation of the Graphics API Dependent Resource
		class GADRTexture : public GADRResource
		{
			GE_RTTI(GraphicsEngine::Graphics::GADRTexture)

		public:
			GADRTexture();
			explicit GADRTexture(Renderer* pRenderer, Texture* pTexture);
			virtual ~GADRTexture();

			void Bind(GLuint unit = 0);

			Texture* GetTexture();
			OpenGLTextureObject* GetGLTextureObject();
			OpenGLSampler* GetGLSampler();

		private:
			void Create();
			void Destroy();

			OpenGLTextureObject* mpOpenGLTextureObject;
			OpenGLSampler* mpOpenGLSampler;
			Texture* mpTexture;
		};
	}
}
#endif // OPENGL_RENDERER

#endif // GRAPHICS_RENDERING_BACKENDS_OPENGL_RESOURCES_OPENGL_TEXTURE_HPP