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
			enum class InputRate : uint8_t
			{
				GE_IR_VERTEX = 0,
				GE_IR_INSTANCE,
				GE_IR_COUNT
			};

			VertexBuffer();
			explicit VertexBuffer(VertexFormat* vertexFormat, Buffer::BufferUsage usage, VertexBuffer::InputRate inputRate, void* pData, uint32_t size);
			virtual ~VertexBuffer();

			VertexFormat* GetFormat() const;
			const VertexBuffer::InputRate& GetInputRate() const;
			const uint32_t& GetVertexCount() const;


		private:
			void Create(void* pData);
			void Destroy();

			VertexFormat* mpFormat;
			VertexBuffer::InputRate mInputRate;
			uint32_t mVertexCount;
		};
	}
}

#endif // GRAPHICS_RENDERING_RESOURCES_VERTEX_BUFFER_HPP