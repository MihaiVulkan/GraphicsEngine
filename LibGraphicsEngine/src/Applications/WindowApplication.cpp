#include "WindowApplication.hpp"

using namespace GraphicsEngine;

WindowApplication::WindowApplication()
	: Application()
{}

WindowApplication::~WindowApplication()
{}

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