#ifndef GRAPHICS_GRAPHICS_SYSTEM_HPP
#define GRAPHICS_GRAPHICS_SYSTEM_HPP

#include "System.hpp"
#include <string>

namespace GraphicsEngine
{
	namespace Platform
	{
		class GE_Window;
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
		explicit GraphicsSystem(const std::string& name, uint32_t width, uint32_t height);
		virtual ~GraphicsSystem();

		void Run(bfloat32_t deltaTime);

		void ComputeRenderQueue();

		void RenderFrame();
		void UpdateFrame(bfloat32_t deltaTime);
		void SubmitFrame();

		Platform::GE_Window* GetWindow();

		Graphics::Renderer* GetRenderer();

		Graphics::Node* GetScene();
		void SetScene(Graphics::Node* pScene);

		Graphics::Camera* GetMainCamera();


	private:
		NO_COPY_NO_MOVE(GraphicsSystem)

		void Init(const std::string& name, uint32_t width, uint32_t height);
		void Terminate();

		// Window - TODO - only one window for now
		Platform::GE_Window* mpWindow;

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