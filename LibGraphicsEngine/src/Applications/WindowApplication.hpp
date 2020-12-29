#ifndef APPLICATIONS_WINDOW_APPLICATION_HPP
#define APPLICATIONS_WINDOW_APPLICATION_HPP

#include "Application.hpp"

namespace GraphicsEngine
{
	class WindowApplication : public Application
	{
		GE_RTTI(GraphicsEngine::WindowApplication)

	public:
		WindowApplication();
		virtual ~WindowApplication();

		virtual void Init(const std::string& name, uint32_t width = 0, uint32_t height = 0) override;
		virtual void Terminate() override;

		virtual void Run() override;

	private:
	};
}
#endif // APPLICATIONS_WINDOW_APPLICATION_HPP