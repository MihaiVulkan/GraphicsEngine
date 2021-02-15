#include "Applications/WindowApplication.hpp"

using namespace GraphicsEngine;

WindowApplication::WindowApplication()
	: Application()
{}

WindowApplication::WindowApplication(const std::string& name, uint32_t width, uint32_t height)
{
	Init(name, width, height);
}

WindowApplication::~WindowApplication()
{
	Terminate();
}

void WindowApplication::Init(const std::string& name, uint32_t width, uint32_t height)
{
	Application::Init(name, width, height);
}

void WindowApplication::Terminate()
{
	Application::Terminate();
}

void WindowApplication::Run()
{
	Application::Run();
}