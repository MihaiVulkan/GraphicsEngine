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

		/* Base class for Render passes */
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
			explicit RenderPass(RenderPass::PassType type);
			virtual ~RenderPass();

			virtual void Render(Renderer* pRenderer, RenderQueue* pRenderQueue, uint32_t currentBufferIdx) {}
			virtual void Update(Renderer* pRenderer, RenderQueue* pRenderQueue, Camera* pCamera, float32_t crrTime) {}

			const RenderPass::PassType& GetPassType() const;

		protected:
			NO_COPY_NO_MOVE_CLASS(RenderPass)

			PassType mPassType;
		};
	}
}

#endif // GRAPHICS_RENDERING_RENDER_PASSES_RENDER_PASS_HPP