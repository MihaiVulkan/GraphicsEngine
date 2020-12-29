#ifndef GRAPHICS_RENDERING_RESOURCES_INDEX_BUFFER_HPP
#define GRAPHICS_RENDERING_RESOURCES_INDEX_BUFFER_HPP

#include "Buffer.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		// IndexBuffer - used to store index data to pass to a specific Graphics API
		class IndexBuffer : public Buffer
		{
			GE_RTTI(GraphicsEngine::Graphics::IndexBuffer)

		public:
			enum class IndexType : uint8_t
			{
				GE_IT_UINT32 = 0,
				GE_IT_UINT16,
				GE_IT_UINT8,
				GE_IT_COUNT
			};

			IndexBuffer();
			explicit IndexBuffer(const Buffer::BufferUsage& usage, const IndexBuffer::IndexType& indexType, void* pData, uint32_t size);
			virtual ~IndexBuffer();

			const IndexBuffer::IndexType& GetIndexType() const;
			const uint32_t& GetIndexCount() const;

			const bool_t& GetIsRestartPrimitive() const;
			void SetIsRestartPrimitive(bool_t value);

		private:
			void Create(void* pData);
			void Destroy();

			IndexBuffer::IndexType mIndexType;
			uint32_t mIndexCount;
			bool_t mIsRestartPrimitive; // index drawing - false by default
		};
	}
}

#endif // GRAPHICS_RENDERING_RESOURCES_INDEX_BUFFER_HPP