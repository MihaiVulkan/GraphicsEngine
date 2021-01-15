#ifndef APPLICATIONS_APPLICATION_HPP
#define APPLICATIONS_APPLICATION_HPP

#include "Foundation/TypeDefines.hpp"
#include "Foundation/NoCopyNoMove.hpp"
#include "Foundation/RTTI.hpp"
#include "Core/Engine.hpp"
#include <functional>
#include <string>

namespace GraphicsEngine
{
	class GraphicsSystem;
	class InputSystem;

	class Application : public RTTI
	{
		GE_RTTI(GraphicsEngine::Application)

	public:
	//	typedef std::function<void(const std::string& name, uint32_t width, uint32_t height)> InitializeFN;
	//	typedef std::function<void()> TerminateFN;
	//	typedef std::function<void()> RunFN;

		Application();
		virtual ~Application();

		GraphicsSystem* GetGraphicsSystem() const;
		InputSystem* GetInputSystem() const;

	protected:
	//	InitializeFN mOnInitialize;
	//	TerminateFN mOnTerminate;
	//	RunFN mOnRun;

		virtual void Init(const std::string& name, uint32_t width = 0, uint32_t height = 0);
		virtual void Terminate();

		virtual void Run();

	private:
		NO_COPY_NO_MOVE(Application)
	};
}

#endif // APPLICATIONS_APPLICATION_HPP