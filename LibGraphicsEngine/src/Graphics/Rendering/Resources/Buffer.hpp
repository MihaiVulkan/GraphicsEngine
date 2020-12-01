#ifndef GRAPHICS_RENDERING_RESOURCES_BUFFER_HPP
#define GRAPHICS_RENDERING_RESOURCES_BUFFER_HPP

#include "Resource.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		// Buffer - used to store generic data to pass to a specific Graphics API
		class Buffer : public Resource
		{
			GE_RTTI(GraphicsEngine::Graphics::Buffer)

		public:
			Buffer();
			explicit Buffer(void* pData, uint32_t size);
			virtual ~Buffer();

			void* GetData() const;
			const uint32_t& GetSize() const;

		protected:
			void Create(void* pData);
			void Destroy();

			void SetData(void* pData, uint32_t size);

			void* mpData;
			uint32_t mSize;
		};
	}
}

#endif // GRAPHICS_RENDERING_RESOURCES_BUFFER_HPP