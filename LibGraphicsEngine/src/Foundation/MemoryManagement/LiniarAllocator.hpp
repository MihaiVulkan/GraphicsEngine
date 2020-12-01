#ifndef FOUNDATION_MEMORYMANAGEMENT_LINIAR_ALLOCATOR_HPP
#define FOUNDATION_MEMORYMANAGEMENT_LINIAR_ALLOCATOR_HPP

#include "Allocator.hpp"
#include "Foundation/Singleton.hpp"

namespace GraphicsEngine
{
	class LiniarAllocator : public Allocator//, public Singleton<LiniarAllocator>
	{
		GE_RTTI(GraphicsEngine::LiniarAllocator)

		// to keep ctor() and dtor() private
		//friend Singleton<LiniarAllocator>;

	public:
		LiniarAllocator();
		explicit LiniarAllocator(uint64_t totalSize);
		virtual ~LiniarAllocator();

		void Init();
		void Reset();

		void* Allocate(uint64_t size, uint64_t alignment = 0);
		void Free(void* ptr);

	private:
		NO_COPY_NO_MOVE(LiniarAllocator)

		void* mpMemoryBuffer;
		uint64_t mOffset;
	};
}

#endif /* FOUNDATION_MEMORYMANAGEMENT_LINIAR_ALLOCATOR_HPP */