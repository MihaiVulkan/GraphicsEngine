#ifndef GRAPHICS_RENDERING_RENDER_PASSES_RENDER_PASS_HPP
#define GRAPHICS_RENDERING_RENDER_PASSES_RENDER_PASS_HPP

#include "Foundation/Object.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Renderer;
		class RenderQueue;
		class Camera;

		class RenderPass : public Object
		{
			GE_RTTI(GraphicsEngine::Graphics::RenderPass)

		public:
			enum class PassType : uint8_t
			{
				GE_PT_STANDARD = 0,
				GE_PT_OFFSCREEN,
				GE_PT_SHADOWS,
				GE_PT_COUNT
			};

			RenderPass();
			virtual ~RenderPass();

			virtual void Render(Renderer* pRenderer, RenderQueue* pRenderQueue, uint32_t currentBufferIdx) {}
			virtual void Update(Renderer* pRenderer, RenderQueue* pRenderQueue, Camera* pCamera, float32_t crrTime) {}

			const RenderPass::PassType& GetPassType() const;

		protected:
			PassType mPassType;
		};
	}
}

#endif // GRAPHICS_RENDERING_RENDER_PASSES_RENDER_PASS_HPP