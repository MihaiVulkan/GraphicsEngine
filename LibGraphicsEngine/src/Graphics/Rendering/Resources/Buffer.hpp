#ifndef GRAPHICS_RENDERING_RESOURCES_BUFFER_HPP
#define GRAPHICS_RENDERING_RESOURCES_BUFFER_HPP

#include "Graphics/Rendering/Resources/Resource.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		// Buffer - used to store generic data to pass to a specific Graphics API
		class Buffer : public Resource
		{
			GE_RTTI(GraphicsEngine::Graphics::Buffer)

		public:
			enum class BufferUsage : uint8_t
			{
				GE_BU_STATIC = 0,
				GE_BU_DYNAMIC,
				GE_BU_COUNT
			};

			Buffer();
			explicit Buffer(BufferUsage usage, void* pData, uint32_t size);
			virtual ~Buffer();

			const Buffer::BufferUsage& GetBufferUsage() const;

			void* GetData() const;
			const uint32_t& GetSize() const;

		protected:
			void Create(void* pData);
			void Destroy();

			void SetData(void* pData, uint32_t size);

			BufferUsage mUsage;
			uint8_t* mpData; //better to have a buffer of known type, char_t size is always 1 byte!
			uint32_t mSize;
		};
	}
}

#endif // GRAPHICS_RENDERING_RESOURCES_BUFFER_HPP