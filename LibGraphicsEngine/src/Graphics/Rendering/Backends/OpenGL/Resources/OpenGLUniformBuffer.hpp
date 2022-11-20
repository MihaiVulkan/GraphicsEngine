#ifndef GRAPHICS_RENDERING_BACKENDS_OPENGL_RESOURCES_OPENGL_UNIFORM_BUFFER_HPP
#define GRAPHICS_RENDERING_BACKENDS_OPENGL_RESOURCES_OPENGL_UNIFORM_BUFFER_HPP

#if defined(OPENGL_RENDERER)
#include "Graphics/Rendering/Backends/OpenGL/Resources/OpenGLResource.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Renderer;
		class OpenGLBuffer;
		class UniformBuffer;

		// OpenGL implementation of the Graphics API Dependent Resource
		class GADRUniformBuffer : public GADRResource
		{
			GE_RTTI(GraphicsEngine::Graphics::GADRUniformBuffer)

		public:
			GADRUniformBuffer();
			explicit GADRUniformBuffer(Renderer* pRenderer, UniformBuffer* pUniformBuffer);
			virtual ~GADRUniformBuffer();

			void UpdateData();

			virtual void OnBind(uint32_t currentBufferIdx = 0) override;
			virtual void OnUnBind(uint32_t currentBufferIdx = 0) override;

			OpenGLBuffer* GetGLBuffer() const;

		private:
			void Create();
			void Destroy();

			OpenGLBuffer* mpOpenGLBuffer;
			UniformBuffer* mpUniformBuffer;
		};
	}
}
#endif // OPENGL_RENDERER

#endif // GRAPHICS_RENDERING_BACKENDS_OPENGL_RESOURCES_OPENGL_UNIFORM_BUFFER_HPP