#ifndef GRAPHICS_GRAPHICS_SYSTEM_HPP
#define GRAPHICS_GRAPHICS_SYSTEM_HPP

#include "Core/System.hpp"
#include <string>

namespace GraphicsEngine
{
	namespace Platform
	{
		class Window;
	}

	namespace Graphics
	{
		class Renderer;
		class RenderQueue;
		class RenderPass;
		class Node;
		class Camera;
	}

	class GraphicsSystem : public System
	{
		GE_RTTI(GraphicsEngine::GraphicsSystem)

	public:
		GraphicsSystem();
		explicit GraphicsSystem(Platform::Window* pWindow);
		virtual ~GraphicsSystem();

		void Run(float32_t crrTime);


		void RenderFrame();
		void UpdateFrame(float32_t crrTime);
		void SubmitFrame();

		Graphics::Renderer* GetRenderer();

		Graphics::Node* GetScene();
		void SetScene(Graphics::Node* pScene);

		Graphics::Camera* GetMainCamera();


	private:
		NO_COPY_NO_MOVE_CLASS(GraphicsSystem)

		void Init(Platform::Window* pWindow);
		void Terminate();

		void ComputeRenderQueue();
		void ComputeGraphicsResources();

		// Window ref
		Platform::Window* mpWindow;

		// Renderer
		Graphics::Renderer* mpRenderer;

		// RenderQueue - TODO - for now we support only one
		Graphics::RenderQueue* mpRenderQueue;

		// Root node of the scene graph
		Graphics::Node* mpScene;

		// Main Camera - TODO - for now we have only one camera
		Graphics::Camera* mpMainCamera;

		// Default Render Pass - TODO
		Graphics::RenderPass* mpRenderPass;

	};
}

#endif // GRAPHICS_GRAPHICS_SYSTEM_HPP