#ifndef GRAPHICS_RENDERING_BACKENDS_OPENGL_RESOURCES_OPENGL_VERTEX_BUFFER_HPP
#define GRAPHICS_RENDERING_BACKENDS_OPENGL_RESOURCES_OPENGL_VERTEX_BUFFER_HPP

#if defined(OPENGL_RENDERER)
#include "Graphics/Rendering/Backends/OpenGL/Resources/OpenGLResource.hpp"
#include "Graphics/Rendering/Resources/VertexBuffer.hpp"


namespace GraphicsEngine
{
	namespace Graphics
	{
		class Renderer;
		class OpenGLBuffer;

		// OpenGL implementation of the Graphics API Dependent Resource
		class GADRVertexBuffer : public GADRResource
		{
			GE_RTTI(GraphicsEngine::Graphics::GADRVertexBuffer)

		public:
			GADRVertexBuffer();
			explicit GADRVertexBuffer(Renderer* pRenderer, VertexBuffer* pVertexBuffer);
			virtual ~GADRVertexBuffer();

			virtual void OnBind(uint32_t currentBufferIdx = 0) override;
			virtual void OnUnBind(uint32_t currentBufferIdx = 0) override;

			const Buffer::BufferUsage& GetBufferUsage() const;

		private:
			void Create();
			void Destroy();

			OpenGLBuffer* mpOpenGLBuffer;
			VertexBuffer* mpVertexBuffer;
		};
	}
}
#endif // OPENGL_RENDERER

#endif // GRAPHICS_RENDERING_BACKENDS_OPENGL_RESOURCES_OPENGL_VERTEX_BUFFER_HPP