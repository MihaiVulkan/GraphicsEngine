#include "Graphics/GraphicsSystem.hpp"
#include "Foundation/Platform/Platform.hpp"
#if defined(VULKAN_RENDERER)
#include "Graphics/Rendering/Backends/Vulkan/VulkanRenderer.hpp"
#elif defined(OPENGL_RENDERER)
#include "Graphics/Rendering/Backends/OpenGL/OpenGLRenderer.hpp"
#endif // 
#include "Graphics/Rendering/RenderQueue.hpp"
#include "Graphics/SceneGraph/Node.hpp"
#include "Graphics/SceneGraph/Visitors/ComputeRenderQueueVisitor.hpp"
#include "Graphics/Cameras/FPSCamera.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

GraphicsSystem::GraphicsSystem()
: mpWindow(nullptr)
, mpRenderer(nullptr)
, mpRenderQueue(nullptr)
, mpScene(nullptr)
, mpMainCamera(nullptr)
{}

GraphicsSystem::GraphicsSystem(Platform::Window* pWindow)
	: GraphicsSystem()
{
	Init(pWindow);
}

GraphicsSystem::~GraphicsSystem()
{
	Terminate();
}

void GraphicsSystem::Init(Platform::Window* pWindow)
{
	assert(pWindow != nullptr);

	mpWindow = pWindow;

	//TODO - for now the vulkan renderer supports only one view - viewport
	// use Vulkan renderer
#if defined(VULKAN_RENDERER)
	mpRenderer = GE_ALLOC(VulkanRenderer)(pWindow);
#elif defined(OPENGL_RENDERER)
	mpRenderer = GE_ALLOC(OpenGLRenderer)(pWindow);
#else
	// other
#endif // 
	assert(mpRenderer != nullptr);

	mpMainCamera = GE_ALLOC(FPSCamera)("MainCamera"); //TODO - use FPSCamera by default
	assert(mpMainCamera != nullptr);

	uint32_t windowWidth = 0, windowHeight = 0;
	pWindow->GetWindowSize(&windowWidth, &windowHeight);

	mpMainCamera->SetAspectRatio(windowWidth / static_cast<float32_t>(windowHeight));

	// RenderQueue
	mpRenderQueue = GE_ALLOC(RenderQueue);
	assert(mpRenderQueue != nullptr);

	// SceneGraph
	// constructed by the app, traversed by the engine - TODO

}

void GraphicsSystem::Terminate()
{
	GE_FREE(mpMainCamera);
	GE_FREE(mpScene);

	GE_FREE(mpRenderQueue);
	GE_FREE(mpRenderer);

	if (mpWindow)
	{
		mpWindow = nullptr;
	}
}

void GraphicsSystem::Run(float32_t crrTime)
{
	UpdateFrame(crrTime);
	RenderFrame();
	SubmitFrame();
}


void GraphicsSystem::RenderFrame()
{
#if defined(VULKAN_RENDERER)
	// The scene has already been rendered !!!
	// Nothing to do here for the Vulkan API
#elif defined(OPENGL_RENDERER)
	mpRenderer->RenderFrame(mpRenderQueue);
#endif // 
}

void GraphicsSystem::UpdateFrame(float32_t crrTime)
{
	assert(mpRenderer != nullptr);

#if defined(VULKAN_RENDERER)
	mpRenderer->UpdateFrame(mpMainCamera, crrTime);
#elif defined(OPENGL_RENDERER)
	mpRenderer->UpdateFrame(mpMainCamera, crrTime);
#endif // 
}

void GraphicsSystem::SubmitFrame()
{
	assert(mpRenderer != nullptr);

#if defined(VULKAN_RENDERER)
	// here we just submit & present the frame to GPU
	mpRenderer->SubmitFrame();
#elif defined(OPENGL_RENDERER)
	mpRenderer->SubmitFrame();
#endif //
}

void GraphicsSystem::ComputeRenderQueue()
{
	assert(mpScene != nullptr);
	assert(mpRenderQueue != nullptr);

	ComputeRenderQueueVisitor queueVisitor(mpRenderQueue);

	mpScene->Traverse(queueVisitor);
}

void GraphicsSystem::ComputeGraphicsResources()
{
	//NOTE! To be called after the renderqueue has been populated

	assert(mpRenderer != nullptr);
	assert(mpRenderQueue != nullptr);
	assert(mpWindow != nullptr);

	mpRenderer->ComputeGraphicsResources(mpRenderQueue);

#if defined(VULKAN_RENDERER)
	// NOTE! With Vulkan we record all rendering upfront!
	mpRenderer->RenderFrame(mpRenderQueue);
#endif // VULKAN_RENDERER

	//TODO - proper handling of uneeded GAIR resources
	// NOTE! If window is resizable then this will not work, as we need the resources on window resize
	if (mpWindow->IsWindowResizable() == false)
	{
//		mpRenderer->CleanUpGAIR();
	}

	// NOTE! We can not free the scene graph here as we still need it to update the objects/components each frame!
}

Graphics::Renderer* GraphicsSystem::GetRenderer()
{
	return mpRenderer;
}

Graphics::Node* GraphicsSystem::GetScene()
{
	return mpScene;
}

void GraphicsSystem::SetScene(Node* pScene)
{
	assert(pScene != nullptr);
	assert(mpMainCamera != nullptr);

	mpScene = pScene;

	// camera update
	mpMainCamera->UpdateViewMatrix();
	mpMainCamera->UpdatePerspectiveProjectionMatrix();

	//////// Rendering setup
	ComputeRenderQueue();

	ComputeGraphicsResources();
}

Graphics::Camera* GraphicsSystem::GetMainCamera()
{
	return mpMainCamera;
}

void GraphicsSystem::SetMainCamera(Graphics::Camera* pCamera)
{
	GE_FREE(mpMainCamera);

	mpMainCamera = pCamera;
}