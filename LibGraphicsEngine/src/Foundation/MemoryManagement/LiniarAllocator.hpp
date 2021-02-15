#ifndef FOUNDATION_MEMORYMANAGEMENT_LINIAR_ALLOCATOR_HPP
#define FOUNDATION_MEMORYMANAGEMENT_LINIAR_ALLOCATOR_HPP

#include "Foundation/MemoryManagement/Allocator.hpp"

namespace GraphicsEngine
{
	class LiniarAllocator : public Allocator
	{
		GE_RTTI(GraphicsEngine::LiniarAllocator)

	public:
		LiniarAllocator();
		explicit LiniarAllocator(uint64_t totalSize);
		virtual ~LiniarAllocator();

		virtual void Init(uint64_t totalSize) override;
		virtual void Reset() override;
		virtual void Terminate() override;

		virtual void* Allocate(uint64_t size, uint64_t alignment = 0) override;
		virtual void Free(void* ptr) override;

	private:
		NO_COPY_NO_MOVE_CLASS(LiniarAllocator)

		void* mpMemoryBuffer;
		uint64_t mOffset;
	};
}

#endif /* FOUNDATION_MEMORYMANAGEMENT_LINIAR_ALLOCATOR_HPP */