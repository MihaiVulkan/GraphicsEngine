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

		virtual void Init(const std::string& name, uint32_t width, uint32_t height) override;
		virtual void Terminate() override;

		virtual void Run() override;

	private:
	};
}
#endif // APPLICATIONS_WINDOW_APPLICATION_HPP