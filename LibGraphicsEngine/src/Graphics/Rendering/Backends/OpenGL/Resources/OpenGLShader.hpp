#ifndef GRAPHICS_RENDERING_BACKENDS_OPENGL_RESOURCES_OPENGL_SHADER_HPP
#define GRAPHICS_RENDERING_BACKENDS_OPENGL_RESOURCES_OPENGL_SHADER_HPP

#if defined(OPENGL_RENDERER)
#include "Graphics/Rendering/Backends/OpenGL/Resources/OpenGLResource.hpp"
#include "Graphics/Rendering/Resources/Shader.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Renderer;
		class OpenGLShaderObject;

		// OpenGL implementation of the Graphics API Dependent Resource
		class GADRShader : public GADRResource
		{
			GE_RTTI(GraphicsEngine::Graphics::GADRShader)

		public:
			GADRShader();
			explicit GADRShader(Renderer* pRenderer, Shader* pShader);
			virtual ~GADRShader();

			Shader* GetShader() const;

			//
			OpenGLShaderObject* GetGLShaderObject() const;

		private:
			void Create(Renderer* pRenderer);
			void Destroy();

			OpenGLShaderObject* mpOpenGLShaderObject;

			Shader* mpShader;
		};
	}
}
#endif // OPENGL_RENDERER

#endif // GRAPHICS_RENDERING_BACKENDS_OPENGL_RESOURCES_OPENGL_SHADER_HPP