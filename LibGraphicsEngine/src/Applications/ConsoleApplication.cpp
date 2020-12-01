#include "ConsoleApplication.hpp"

using namespace GraphicsEngine;

ConsoleApplication::ConsoleApplication()
	: Application()
{}

ConsoleApplication::ConsoleApplication(const std::string& name)
	: Application()
{}

ConsoleApplication::~ConsoleApplication()
{}

void ConsoleApplication::Init(const std::string& name, uint32_t width, uint32_t height)
{
	Application::Init(name, width, height);
}

void ConsoleApplication::Terminate()
{
	Application::Terminate();
}

void ConsoleApplication::Run()
{
	Application::Run();
}