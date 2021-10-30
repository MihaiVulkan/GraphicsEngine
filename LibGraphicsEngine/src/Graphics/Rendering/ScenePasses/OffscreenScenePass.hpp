#ifndef GRAPHICS_RENDERING_SCENE_PASSES_OFFSCREEN_SCENE_PASS_HPP
#define GRAPHICS_RENDERING_SCENE_PASSES_OFFSCREEN_SCENE_PASS_HPP

#include "Graphics/Rendering/ScenePasses/ScenePass.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		/* Offscreen Scene Pass - to render scene in an offscreen framebuffer */
		class OffscreenScenePass : public ScenePass
		{
			GE_RTTI(GraphicsEngine::Graphics::OffscreenScenePass)

		public:
			OffscreenScenePass();
			virtual ~OffscreenScenePass();

			virtual void Render(Renderer* pRenderer, RenderQueue* pRenderQueue, uint32_t currentBufferIdx,
								ScenePass::OnBeginFN OnBegin, ScenePass::OnEndFN OnEnd) override;
			virtual void Update(Renderer* pRenderer, RenderQueue* pRenderQueue, Camera* pCamera, float32_t crrTime) override;

		private:
			NO_COPY_NO_MOVE_CLASS(OffscreenScenePass)

			void RenderOpaqueObjects(Renderer* pRenderer, RenderQueue* pRenderQueue, uint32_t currentBufferIdx);
			void UpdateOpaqueObjects(Renderer* pRenderer, RenderQueue* pRenderQueue, Camera* pCamera, float32_t deltaTime);
		};
	}
}

#endif // GRAPHICS_RENDERING_SCENE_PASSES_OFFSCREEN_SCENE_PASS_HPP