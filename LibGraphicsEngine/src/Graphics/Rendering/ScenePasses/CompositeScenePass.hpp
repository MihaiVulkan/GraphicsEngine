#ifndef GRAPHICS_RENDERING_SCENE_PASSES_COMPOSITE_SCENE_PASS_HPP
#define GRAPHICS_RENDERING_SCENE_PASSES_COMPOSITE_SCENE_PASS_HPP

#include "Graphics/Rendering/ScenePasses/ScenePass.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		/* Composite Scene Pass - to render multiple passes */
		class CompositeScenePass : public ScenePass
		{
			GE_RTTI(GraphicsEngine::Graphics::CompositeScenePass)

		public:
			CompositeScenePass();
			virtual ~CompositeScenePass();

			virtual void AddPass(ScenePass* pPass) override;

			virtual void Render(Renderer* pRenderer, RenderQueue* pRenderQueue, uint32_t currentBufferIdx,
								ScenePass::OnBeginFN OnBegin, ScenePass::OnEndFN OnEnd) override;
			virtual void Update(Renderer* pRenderer, RenderQueue* pRenderQueue, Camera* pCamera, float32_t crrTime) override;

		private:
			NO_COPY_NO_MOVE_CLASS(CompositeScenePass)
		};
	}
}

#endif // GRAPHICS_RENDERING_SCENE_PASSES_COMPOSITE_SCENE_PASS_HPP