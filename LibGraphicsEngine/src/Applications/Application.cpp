#include "Applications/Application.hpp"
#include "Core/GraphicsEngineInternal.hpp"
#include "Graphics/GraphicsSystem.hpp"
#include "Input/InputSystem.hpp"

using namespace GraphicsEngine;

Application::Application()
{}

Application::Application(const std::string& name, uint32_t width, uint32_t height)
{
	Init(name, width, height);
}

Application::~Application()
{
	Terminate();
}

void Application::Init(const std::string& name, uint32_t width, uint32_t height)
{
	GraphicsEngine::Init();

	Engine::GetInstance()->Init(name, width, height);
}

void Application::Terminate()
{
	Engine::GetInstance()->Terminate();

	GraphicsEngine::Terminate();
}

void Application::Run()
{
	Engine::GetInstance()->Run();
}

GraphicsSystem* Application::GetGraphicsSystem() const
{
	return Engine::GetInstance()->GetGraphicsSystem();
}

InputSystem* Application::GetInputSystem() const
{
	return Engine::GetInstance()->GetInputSystem();
}