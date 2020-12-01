#ifndef GRAPHICS_RENDERING_RENDERPASSES_DEAFULT_RENDER_PASS_HPP
#define GRAPHICS_RENDERING_RENDERPASSES_DEAFULT_RENDER_PASS_HPP

#include "Foundation/TypeDefs.hpp"
#include "RenderPass.hpp"

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
			virtual void Update(Renderer* pRenderer, RenderQueue* pRenderQueue, Camera* pCamera, bfloat32_t deltaTime) override;

		private:
			void RenderOccluders(Renderer* pRenderer, RenderQueue* pRenderQueue, uint32_t currentBufferIdx);
			void RenderOpaqueObjects(Renderer* pRenderer, RenderQueue* pRenderQueue, uint32_t currentBufferIdx);
			void RenderTranslucentObjects(Renderer* pRenderer, RenderQueue* pRenderQueue, uint32_t currentBufferIdx);

			void UpdateOpaqueObjects(Renderer* pRenderer, RenderQueue* pRenderQueue, Camera* pCamera, bfloat32_t deltaTime);
		};
	}
}

#endif // GRAPHICS_RENDERING_RENDERPASSES_DEAFULT_RENDER_PASS_HPP