#ifndef CORE_SYSTEM_HPP
#define CORE_SYSTEM_HPP

#include "Core/AppConfig.hpp"
#include "Foundation/Object.hpp"

namespace GraphicsEngine
{
	class System : public Object
	{
		GE_RTTI(GraphicsEngine::System)

	public:
		System();
		virtual ~System();

	private:
		NO_COPY_NO_MOVE_CLASS(System)
	};
}
#endif // CORE_SYSTEM_HPP