#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "AppConfig.hpp"
#include "Foundation/Object.hpp"
#include "Foundation/NoCopyNoMove.hpp"
#include "Foundation/DynamicSingleton.hpp"
#include <string>

/* SETUP

	Vulkan layers info -> Instance -> Physical devices (GPUs) -> Window -> Surface ->
	Logical device -> Graphics/Present queue -> Swap chain -> Color format + Image View ->
	Depth buffer (Image + Image view, content memory is mapped onto them) ->
	Uniform buffer (MVP matrices, content memory is mapped onto them) ->
	Descriptor set layout -> Pipeline layout -> Render pass (FBO attachments + subpass) ->
	Shader modules (for each shader stage) -> Frame buffers (one for each swap chain image) ->
	Vertex buffers (content memory is mapped onto them) -> Descriptor set pool -> Descriptor sets (from the pool) ->
	Pipeline cache -> Pipeline (dynamic state, vertex, primitive assembly, rasterization, color blend, depth & sencil,
	viewport & sccisors, multisampling, render passes) ->
	Syncronization primitives during drawing (smaphores) -> Command buffers pool -> Command buffers (from the pool) ->
	Add/Record drawing instructions into the command buffers (render passes)
*/

/* RENDER LOOP

	1) Prepare next image (Acquire an image from the swap chain)
	2) Submit the command buffers in the drawing queue (Execute the command buffer with that image as attachment in the framebuffer)
	3) Preset the drawing queue (Return the image to the swap chain for presentation)

*/

namespace GraphicsEngine
{
	namespace Platform
	{
		class GE_Window;
	}

	class InputSystem;
	class GraphicsSystem;

	class Engine : public Object, public DynamicSingleton<Engine>
	{
		GE_RTTI(GraphicsEngine::Engine)

		// to keep ctor() and dtor() private
		friend DynamicSingleton<Engine>;

	public:
		void Init(const std::string& name, uint32_t width, uint32_t height);
		void Terminate();

		void Run();

		Platform::GE_Window* GetWindow();
		GraphicsSystem* GetGraphicsSystem();
		InputSystem* GetInputSystem();

	private:
		NO_COPY_NO_MOVE(Engine)

		Engine();
		virtual ~Engine();


		void FPSCount(float32_t deltaTime);

		// Window - TODO - only one window for now
		Platform::GE_Window* mpWindow;

		GraphicsSystem* mpGraphicsSystem;
		InputSystem* mpInputSystem;
	};

}

#endif /* ENGINE_HPP */
