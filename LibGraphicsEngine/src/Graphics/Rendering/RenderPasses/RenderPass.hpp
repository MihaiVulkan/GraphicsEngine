#ifndef GRAPHICS_RENDERING_RENDERPASSES_RENDER_PASS_HPP
#define GRAPHICS_RENDERING_RENDERPASSES_RENDER_PASS_HPP

#include "Foundation/TypeDefs.hpp"
#include "Foundation/RTTI.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Renderer;
		class RenderQueue;
		class Camera;

		class RenderPass : public RTTI
		{
			GE_RTTI(GraphicsEngine::Graphics::RenderPass)

		public:
			RenderPass();
			virtual ~RenderPass();

			virtual void Render(Renderer* pRenderer, RenderQueue* pRenderQueue, uint32_t currentBufferIdx) {}
			virtual void Update(Renderer* pRenderer, RenderQueue* pRenderQueue, Camera* pCamera, bfloat32_t deltaTime) {}

		private:
		};
	}
}

#endif // GRAPHICS_RENDERING_RENDERPASSES_DEAFULT_RENDER_PASS_HPP