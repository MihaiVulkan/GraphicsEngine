#include "Engine.hpp"
#include "Foundation/Platform/Platform.hpp"
#include "Graphics/GraphicsSystem.hpp"
#include "Graphics/Rendering/Renderer.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Input/InputSystem.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"
#include <chrono>
#include <sstream> // fps
#include <cassert>
#include <functional>

#define NEW_GRAPHICS

using namespace GraphicsEngine;

Engine::Engine()
	: mpGraphicsSystem(nullptr)
	, mpInputSystem(nullptr)
{}

void Engine::Init(const std::string& name, uint32_t width, uint32_t height)
{
	Platform::Init();

	mpGraphicsSystem = GE_ALLOC(GraphicsSystem)(name, width, height);
	assert(mpGraphicsSystem != nullptr);

	mpInputSystem = GE_ALLOC(InputSystem);
	assert(mpInputSystem != nullptr);

	mpInputSystem->Init(mpGraphicsSystem->GetWindow(), mpGraphicsSystem->GetMainCamera(), InputSystem::GE_InputMode::GE_FPS);
}

Engine::~Engine()
{
	Terminate();
}

void Engine::Terminate()
{
	GE_FREE(mpGraphicsSystem);

	GE_FREE(mpInputSystem);

	Platform::Terminate();
}

void Engine::Run()
{
	while (Platform::IsWindowVisible(mpGraphicsSystem->GetWindow()) && (false == Platform::ShouldWindowClose(mpGraphicsSystem->GetWindow())))
	{
		static bfloat32_t deltaTime = 0.0f;
		auto startFrameTime = std::chrono::system_clock::now();

		if (false == Platform::IsWindowMinimized(mpGraphicsSystem->GetWindow()))
		{
#ifdef NEW_GRAPHICS
			mpGraphicsSystem->Run(deltaTime);
#else
			mpGraphicsSystem->GetRenderer()->RenderFrame();
#endif

			mpInputSystem->UpdateContinuousInput(deltaTime);
		}

		auto endFrameTime = std::chrono::system_clock::now();
		std::chrono::duration<bfloat32_t> diffFrameTime = endFrameTime - startFrameTime;
		deltaTime = diffFrameTime.count();

		deltaTime += 1; // offset TO BE REMOVED WHEN RENDERING SOMETHING

		//LOG_DEBUG("deltaTime: %.10lf %.10f", deltaTime);

		//////////////////// FPS Count //////////
		FPSCount(deltaTime);

		Platform::PollEvents(mpGraphicsSystem->GetWindow());
	}
}

void Engine::FPSCount(bfloat32_t deltaTime)
{
	// Static values which only get initialised the first time the function runs
	static auto start = std::chrono::system_clock::now(); // Set the initial time to now
	static bfloat32_t fps = 0.0f; //inital FPS = 0

	// Set the initial frame count to -1.0 (it gets set to 0.0 on the next line). Because
	// we don't have a start time we simply cannot get an accurate FPS value on our very
	// first read if the time interval is zero, so we'll settle for an FPS value of zero instead.
	static bfloat32_t frameCount = -1.0f;

	float timeInterval = 1.0f;


	// Increment the frame count
	frameCount++;

	// Ensure the time interval between FPS checks is sane (low cap = 0.0 i.e. every frame, high cap = 10.0s)
	if (timeInterval < 0.0f)
	{
		timeInterval = 0.0f;
	}
	else if (timeInterval > 10.0f)
	{
		timeInterval = 10.0f;
	}

	// Get the duration in seconds since the last FPS reporting interval elapsed
	// as the current time minus the interval start time
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<bfloat32_t> diff = end - start;

	bfloat32_t duration = diff.count();

	if (duration > timeInterval)
	{
		// Calculate the FPS as the number of frames divided by the duration in seconds
		fps = frameCount / duration;

		// update window title with FPS count
		std::stringstream ss;
		static std::string appName = mpGraphicsSystem->GetWindow()->pTitle;
		ss << appName << " | FPS: " << fps << " | DeltaTime: " << deltaTime;

		Platform::SetWindowTitle(mpGraphicsSystem->GetWindow(), ss.str().c_str());

		// Reset the frame count to zero and set the initial time to be now
		frameCount = 0.0f;
		start = std::chrono::system_clock::now();
	}

}