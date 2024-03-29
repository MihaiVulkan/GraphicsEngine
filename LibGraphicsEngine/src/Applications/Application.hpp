#ifndef APPLICATIONS_APPLICATION_HPP
#define APPLICATIONS_APPLICATION_HPP

#include "Foundation/TypeDefines.hpp"
#include "Foundation/NoCopyNoMoveClass.hpp"
#include "Foundation/RTTI.hpp"
#include "Core/Engine.hpp"
#include <string>

namespace GraphicsEngine
{
	class GraphicsSystem;
	class InputSystem;

	class Application : public RTTI
	{
		GE_RTTI(GraphicsEngine::Application)

	public:
		Application();
		explicit Application(const std::string& name, uint32_t width = 0, uint32_t height = 0);
		virtual ~Application();

		GraphicsSystem* GetGraphicsSystem() const;
		InputSystem* GetInputSystem() const;

	protected:
		virtual void Init(const std::string& name, uint32_t width = 0, uint32_t height = 0);
		virtual void Terminate();

		virtual void Run();

	private:
		NO_COPY_NO_MOVE_CLASS(Application)
	};
}

#endif // APPLICATIONS_APPLICATION_HPP