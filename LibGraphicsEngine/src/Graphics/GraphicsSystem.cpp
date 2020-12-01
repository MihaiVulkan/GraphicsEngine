#include "GraphicsSystem.hpp"
#include "Foundation/Platform/Platform.hpp"
#if defined(VULKAN_RENDERER)
#include "Graphics/Rendering/Vulkan/VulkanRenderer.hpp"
#endif // VULKAN_RENDERER
#include "Graphics/Rendering/RenderQueue.hpp"
#include "Graphics/Rendering/RenderPasses/DefaultRenderPass.hpp"
#include "Graphics/SceneGraph/Node.hpp"
#include "Camera/FPSCamera.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <cassert>

#define NEW_GRAPHICS

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

GraphicsSystem::GraphicsSystem()
	: mpWindow(nullptr)
	, mpRenderer(nullptr)
	, mpRenderQueue(nullptr)
	, mpScene(nullptr)
	, mpMainCamera(nullptr)
	, mpRenderPass(nullptr)
{}

GraphicsSystem::GraphicsSystem(const std::string& name, uint32_t width, uint32_t height)
	: GraphicsSystem()
{
	Init(name, width, height);
}

GraphicsSystem::~GraphicsSystem()
{
	Terminate();
}

void GraphicsSystem::Init(const std::string& name, uint32_t width, uint32_t height)
{
	mpWindow = Platform::CreateWindow(name.c_str(), 0, 0, width, height);

	// Window Callbacks
	Platform::RegisterWindowSizeCallback(mpWindow,
		[this](Platform::GE_Window* pWindow, uint32_t width, uint32_t height)
		{
			if (mpRenderer == nullptr || mpMainCamera == nullptr)
				return;

			if ((false == Platform::IsWindowMinimized(pWindow)) && mpRenderer->IsPrepared())
			{
				// update projection matrix as aspect ratio is different on window resize
				mpMainCamera->UpdatePerspectiveProjectionMatrix(60, width / static_cast<bfloat32_t>(height), 0.1f, 256.0f);

				mpRenderer->WindowResize(width, height);

			}
		});


	// use Vulkan renderer
#if defined(VULKAN_RENDERER)
	mpRenderer = GE_ALLOC(VulkanRenderer);
#endif // VULKAN_RENDERER
	assert(mpRenderer != nullptr);

	mpMainCamera = GE_ALLOC(FPSCamera)("MainCamera"); //TODO - use FPSCamera by default
	assert(mpMainCamera != nullptr);

	//TODO - remove hardocodes, use a config
	mpMainCamera->SetPosition(glm::vec3(0.0f, 0.0f, 2.5f));
	mpMainCamera->UpdateViewMatrix();
	mpMainCamera->UpdatePerspectiveProjectionMatrix(60, mpWindow->width / static_cast<bfloat32_t>(mpWindow->height), 0.1f, 256.0f);

#ifndef NEW_GRAPHICS
	// TODO remove Camera from Renderer 
	mpRenderer->SetCamera(mpMainCamera);
#endif

	mpRenderer->Init(mpWindow);


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
		//TODO
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

	if (mpWindow)
	{
		Platform::DestroyWindow(mpWindow);
	}
}

void GraphicsSystem::Run(bfloat32_t deltaTime)
{
	RenderFrame();
	UpdateFrame(deltaTime);
	SubmitFrame();
}


void GraphicsSystem::RenderFrame()
{
#if defined(VULKAN_RENDERER)
	// The scene has already been rendered !!!
	// Nothing to do here for Vulkan API
#endif // VULKAN_RENDERER
}

void GraphicsSystem::UpdateFrame(bfloat32_t deltaTime)
{
#if defined(VULKAN_RENDERER)
	mpRenderer->Update(mpMainCamera, deltaTime);
#endif // VULKAN_RENDERER
}

void GraphicsSystem::SubmitFrame()
{
#if defined(VULKAN_RENDERER)
	// here we just submit & present the frame to GPU
	mpRenderer->SubmitFrame();
#endif // VULKAN_RENDERER
}

void GraphicsSystem::ComputeRenderQueue()
{
	assert(mpScene != nullptr);
	assert(mpRenderQueue != nullptr);

	mpScene->Visit([this](Node* pNode)
		{
			mpRenderQueue->Push(pNode);
		});
}

Platform::GE_Window* GraphicsSystem::GetWindow()
{
	return mpWindow;
}

Renderer* GraphicsSystem::GetRenderer()
{
	return mpRenderer;
}

Node* GraphicsSystem::GetScene()
{
	return mpScene;
}

void GraphicsSystem::SetScene(Node* pScene)
{
	assert(pScene != nullptr);

	mpScene = pScene;

	//////// Rendering setup
	ComputeRenderQueue();

	// NOTE! With Vulkan we record all rendering upfront!
#if defined(VULKAN_RENDERER)
	mpRenderer->Render(mpRenderQueue, mpRenderPass);
#endif // VULKAN_RENDERER
}

Camera* GraphicsSystem::GetMainCamera()
{
	return mpMainCamera;
}
