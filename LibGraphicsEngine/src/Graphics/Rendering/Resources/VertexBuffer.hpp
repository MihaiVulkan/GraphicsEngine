#ifndef GRAPHICS_RENDERING_RESOURCES_VERTEX_BUFFER_HPP
#define GRAPHICS_RENDERING_RESOURCES_VERTEX_BUFFER_HPP

#include "Graphics/Rendering/Resources/Buffer.hpp"
#include "Graphics/Rendering/Resources/VertexFormat.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		// VertexBuffer - used to store vertex data to pass to a specific Graphics API
		class VertexBuffer: public Buffer
		{
			GE_RTTI(GraphicsEngine::Graphics::VertexBuffer)

		public:
			VertexBuffer();
			explicit VertexBuffer(VertexFormat* vertexFormat, Buffer::BufferUsage usage, void* pData, uint32_t size);
			virtual ~VertexBuffer();

			VertexFormat* GetFormat() const;
			const uint32_t& GetVertexCount() const;


		private:
			void Create(void* pData);
			void Destroy();

			VertexFormat* mpFormat;
			uint32_t mVertexCount;
		};
	}
}

#endif // GRAPHICS_RENDERING_RESOURCES_VERTEX_BUFFER_HPP