#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include "AppConfig.hpp"
#include "Foundation/TypeDefs.hpp"
#include "Foundation/NoCopyNoMove.hpp"
#include "Foundation/RTTI.hpp"

namespace GraphicsEngine
{
	class System : public RTTI
	{
		GE_RTTI(GraphicsEngine::System)

	public:
		System();
		virtual ~System();

	private:
		NO_COPY_NO_MOVE(System)
	};
}
#endif // SYSTEM_HPP