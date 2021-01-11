#ifndef FOUNDATION_OBJECT_HPP
#define FOUNDATION_OBJECT_HPP

#include "TypeDefs.hpp"
#include "RTTI.hpp"
#include "Foundation/NoCopyNoMove.hpp"

namespace GraphicsEngine
{
	class Object : public RTTI
	{
		GE_RTTI(GraphicsEngine::Object)

	public:
		Object();
		virtual ~Object();
	};
}

#endif // FOUNDATION_OBJECT_HPP