#ifndef APPLICATIONS_CONSOLE_APPLICATION_HPP
#define APPLICATIONS_CONSOLE_APPLICATION_HPP

#include "Application.hpp"

namespace GraphicsEngine
{
	class ConsoleApplication : public Application
	{
		GE_RTTI(GraphicsEngine::ConsoleApplication)

	public:
		ConsoleApplication();
		explicit ConsoleApplication(const std::string& name);
		virtual ~ConsoleApplication();

		virtual void Init(const std::string& name, uint32_t width = 0, uint32_t height = 0) override;
		virtual void Terminate() override;

		virtual void Run() override;

		private:
	};
}
#endif // APPLICATIONS_CONSOLE_APPLICATION_HPP