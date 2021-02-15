#ifndef GRAPHICS_RENDERING_RENDER_PASSES_DEFAULT_RENDER_PASS_HPP
#define GRAPHICS_RENDERING_RENDER_PASSES_DEFAULT_RENDER_PASS_HPP

#include "Graphics/Rendering/RenderPasses/RenderPass.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class DefaultRenderPass : public RenderPass
		{
			GE_RTTI(GraphicsEngine::Graphics::DefaultRenderPass)

		public:
			DefaultRenderPass();
			virtual ~DefaultRenderPass();

			virtual void Render(Renderer* pRenderer, RenderQueue* pRenderQueue, uint32_t currentBufferIdx) override;
			virtual void Update(Renderer* pRenderer, RenderQueue* pRenderQueue, Camera* pCamera, float32_t crrTime) override;

		private:
			NO_COPY_NO_MOVE_CLASS(DefaultRenderPass)

			void RenderOccluders(Renderer* pRenderer, RenderQueue* pRenderQueue, uint32_t currentBufferIdx);
			void RenderOpaqueObjects(Renderer* pRenderer, RenderQueue* pRenderQueue, uint32_t currentBufferIdx);
			void RenderTranslucentObjects(Renderer* pRenderer, RenderQueue* pRenderQueue, uint32_t currentBufferIdx);

			void UpdateOpaqueObjects(Renderer* pRenderer, RenderQueue* pRenderQueue, Camera* pCamera, float32_t deltaTime);
		};
	}
}

#endif // GRAPHICS_RENDERING_RENDER_PASSES_DEFAULT_RENDER_PASS_HPP