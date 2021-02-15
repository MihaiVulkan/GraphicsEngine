#ifndef FOUNDATION_OBJECT_HPP
#define FOUNDATION_OBJECT_HPP

#include "Foundation/TypeDefines.hpp"
#include "Foundation/RTTI.hpp"
#include "Foundation/DefaultClass.hpp"
#include "Foundation/NoCopyNoMoveClass.hpp"

namespace GraphicsEngine
{
	class Object : public RTTI
	{
		GE_RTTI(GraphicsEngine::Object)

	public:
		DEFAULT_CLASS(Object)
	};
}

#endif // FOUNDATION_OBJECT_HPP