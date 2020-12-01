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
			enum class Usage : uint8_t
			{
				U_STATIC = 0,
				U_DYNAMIC,
				U_COUNT
			};

			enum class InputRate : uint8_t
			{
				IR_VERTEX = 0,
				IR_INSTANCE,
				IR_COUNT
			};

			VertexBuffer();
			explicit VertexBuffer(VertexFormat* vertexFormat, VertexBuffer::Usage usage, VertexBuffer::InputRate inputRate, void* pData, uint32_t size);
			virtual ~VertexBuffer();

			VertexFormat* GetFormat() const;
			const VertexBuffer::Usage& GetUsage() const;
			const VertexBuffer::InputRate& GetInputRate() const;
			const uint32_t& GetVertexCount() const;


		private:
			void Create(void* pData);// override;
			void Destroy();// override;

			VertexFormat* mpFormat;
			VertexBuffer::Usage mUsage;
			VertexBuffer::InputRate mInputRate;
			uint32_t mVertexCount;
		};
	}
}

#endif // GRAPHICS_RENDERING_RESOURCES_VERTEX_BUFFER_HPP