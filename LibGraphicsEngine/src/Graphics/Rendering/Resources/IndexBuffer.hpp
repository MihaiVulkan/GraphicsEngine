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
			enum class Usage : uint8_t
			{
				U_STATIC = 0,
				U_DYNAMIC,
				U_COUNT
			};

			enum class IndexType : uint8_t
			{
				IT_UINT32 = 0,
				IT_UINT16,
				IT_UINT8,
				IT_COUNT
			};

			IndexBuffer();
			explicit IndexBuffer(const IndexBuffer::Usage& usage, const IndexBuffer::IndexType& indexType, void* pData, uint32_t size);
			virtual ~IndexBuffer();

			const IndexBuffer::Usage& GetUsage() const;
			const IndexBuffer::IndexType& GetIndexType() const;
			const uint32_t& GetIndexCount() const;

		private:
			void Create(void* pData); //override;
			void Destroy(); //override;

			IndexBuffer::Usage mUsage;
			IndexBuffer::IndexType mIndexType;
			uint32_t mIndexCount;
		};
	}
}

#endif // GRAPHICS_RENDERING_RESOURCES_INDEX_BUFFER_HPP