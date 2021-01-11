#include "GraphicsSystem.hpp"
#include "Foundation/Platform/Platform.hpp"
#if defined(VULKAN_RENDERER)
#include "Graphics/Rendering/Vulkan/VulkanRenderer.hpp"
#endif // VULKAN_RENDERER
#include "Graphics/Rendering/RenderQueue.hpp"
#include "Graphics/Rendering/RenderPasses/DefaultRenderPass.hpp"
#include "Graphics/SceneGraph/Node.hpp"
#include "Graphics/SceneGraph/Visitors/ComputeRenderQueueVisitor.hpp"
#include "Camera/FPSCamera.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

GraphicsSystem::GraphicsSystem()
: mpRenderer(nullptr)
, mpRenderQueue(nullptr)
, mpScene(nullptr)
, mpMainCamera(nullptr)
, mpRenderPass(nullptr)
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

	//TODO - for now the vulkan renderer supports only one view - viewport
	// use Vulkan renderer
#if defined(VULKAN_RENDERER)
	mpRenderer = GE_ALLOC(VulkanRenderer);
#else
	// other
#endif // VULKAN_RENDERER
	assert(mpRenderer != nullptr);

	mpMainCamera = GE_ALLOC(FPSCamera)("MainCamera"); //TODO - use FPSCamera by default
	assert(mpMainCamera != nullptr);

	uint32_t windowWidth = 0, windowHeight = 0;
	pWindow->GetWindowSize(&windowWidth, &windowHeight);

	mpMainCamera->SetAspectRatio(windowWidth / static_cast<float32_t>(windowHeight));

	// Renderer init
	mpRenderer->Init(pWindow);

	// RenderQueue
	mpRenderQueue = GE_ALLOC(RenderQueue);
	assert(mpRenderQueue != nullptr);

	// Default Render Pass - by default TODO
	mpRenderPass = GE_ALLOC(DefaultRenderPass);
	assert(mpRenderPass != nullptr);

	// SceneGraph
	// constructed by the app, traversed by the engine - TODO

}

void GraphicsSystem::Terminate()
{
	if (mpMainCamera)
	{
		GE_FREE(mpMainCamera);
	}

	if (mpScene)
	{
		//TODO - cleaup scene tree
	}

	if (mpRenderPass)
	{
		GE_FREE(mpRenderPass);
	}

	if (mpRenderQueue)
	{
		GE_FREE(mpRenderQueue);
	}

	if (mpRenderer)
	{
		GE_FREE(mpRenderer);
	}
}

void GraphicsSystem::Run(float32_t deltaTime)
{
	UpdateFrame(deltaTime);
	RenderFrame();
	SubmitFrame();
}


void GraphicsSystem::RenderFrame()
{
#if defined(VULKAN_RENDERER)
	// The scene has already been rendered !!!
	// Nothing to do here for the Vulkan API
#endif // VULKAN_RENDERER
}

void GraphicsSystem::UpdateFrame(float32_t deltaTime)
{
	assert(mpRenderer != nullptr);

#if defined(VULKAN_RENDERER)
	mpRenderer->UpdateFrame(mpMainCamera, deltaTime);
#endif // VULKAN_RENDERER
}

void GraphicsSystem::SubmitFrame()
{
	assert(mpRenderer != nullptr);

#if defined(VULKAN_RENDERER)
	// here we just submit & present the frame to GPU
	mpRenderer->SubmitFrame();
#endif // VULKAN_RENDERER
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

	mpRenderer->ComputeGraphicsResources(mpRenderQueue, mpRenderPass);

#if defined(VULKAN_RENDERER)
	// NOTE! With Vulkan we record all rendering upfront!
	mpRenderer->RenderFrame(mpRenderQueue, mpRenderPass);
#endif // VULKAN_RENDERER
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
