#ifndef GRAPHICS_RENDERING_BACKENDS_OPENGL_RESOURCES_OPENGL_INDEX_BUFFER_HPP
#define GRAPHICS_RENDERING_BACKENDS_OPENGL_RESOURCES_OPENGL_INDEX_BUFFER_HPP

#if defined(OPENGL_RENDERER)
#include "Graphics/Rendering/Backends/OpenGL/Resources/OpenGLResource.hpp"
#include "Graphics/Rendering/Resources/IndexBuffer.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Renderer;
		class OpenGLBuffer;

		// OpenGL implementation of the Graphics API Dependent Resource
		class GADRIndexBuffer : public GADRResource
		{
			GE_RTTI(GraphicsEngine::Graphics::GADRIndexBuffer)

		public:
			GADRIndexBuffer();
			explicit GADRIndexBuffer(Renderer* pRenderer, IndexBuffer* pIndexBuffer);
			virtual ~GADRIndexBuffer();

			virtual void OnBind(uint32_t currentBufferIdx = 0) override;
			virtual void OnUnBind(uint32_t currentBufferIdx = 0) override;

			const Buffer::BufferUsage& GetBufferUsage() const;
			const IndexBuffer::IndexType& GetIndexType() const;

		private:
			void Create();
			void Destroy();

			OpenGLBuffer* mpOpenGLBuffer;
			IndexBuffer* mpIndexBuffer;
		};
	}
}
#endif // OPENGL_RENDERER

#endif // GRAPHICS_RENDERING_BACKENDS_OPENGL_RESOURCES_OPENGL_INDEX_BUFFER_HPP