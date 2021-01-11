#ifndef GRAPHICS_RENDERING_RESOURCES_VERTEX_BUFFER_HPP
#define GRAPHICS_RENDERING_RESOURCES_VERTEX_BUFFER_HPP

#include "Buffer.hpp"
#include "VertexFormat.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		// VertexBuffer - used to store vertex data to pass to a specific Graphics API
		class VertexBuffer: public Buffer
		{
			GE_RTTI(GraphicsEngine::Graphics::VertexBuffer)

		public:
			enum class VertexInputRate : uint8_t
			{
				GE_VIR_VERTEX = 0,
				GE_VIR_INSTANCE,
				GE_VIR_COUNT
			};

			VertexBuffer();
			explicit VertexBuffer(VertexFormat* vertexFormat, Buffer::BufferUsage usage, VertexBuffer::VertexInputRate vertexInputRate, void* pData, uint32_t size);
			virtual ~VertexBuffer();

			VertexFormat* GetFormat() const;
			const VertexBuffer::VertexInputRate& GetVertexInputRate() const;
			const uint32_t& GetVertexCount() const;


		private:
			void Create(void* pData);
			void Destroy();

			VertexFormat* mpFormat;
			VertexBuffer::VertexInputRate mVertexInputRate;
			uint32_t mVertexCount;
		};
	}
}

#endif // GRAPHICS_RENDERING_RESOURCES_VERTEX_BUFFER_HPP