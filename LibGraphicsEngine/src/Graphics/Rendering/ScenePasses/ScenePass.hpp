#ifndef GRAPHICS_RENDERING_SCENE_PASSES_SCENE_PASS_HPP
#define GRAPHICS_RENDERING_SCENE_PASSES_SCENE_PASS_HPP

#include "Foundation/Object.hpp"
#include "Graphics/Rendering/Resources/RenderTarget.hpp"
#include "Graphics/Rendering/PipelineStates/CullFaceState.hpp"
#include <map>
#include <unordered_map>
#include <functional>
#include "glm/mat4x4.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Renderer;
		class RenderQueue;
		class Camera;
		class Node;

		/* Base class for Scene passes */
		class ScenePass : public Object
		{
			GE_RTTI(GraphicsEngine::Graphics::ScenePass)

		public:
			enum class PassType : uint8_t
			{
				// ordered by the render order
				GE_PT_OFFSCREEN = 1, // render to an offscreen frambuffer
				GE_PT_SHADOWS = 2, // shadows
				GE_PT_STANDARD = 3, // render to screen - last pass
				GE_PT_COMPOSITE = 4, // multiple passes
				GE_PT_COUNT
			};

			typedef std::map<PassType, ScenePass*> ScenePassMap;
			typedef std::unordered_map<RenderTarget::TargetType, RenderTarget*> RenderTargetMap;

			struct PassData
			{
				PassData()
					: width(0), height(0)
					, clearColor(0.0f)
					, depth(0.0f), stencil(0)
				{}

				uint32_t width, height;
				Color4f clearColor;
				float32_t depth;
				uint32_t stencil;
			};

			typedef std::function<void(ScenePass*, uint32_t)> OnBeginFN;
			typedef std::function<void(ScenePass*, uint32_t)> OnEndFN;

			ScenePass();
			explicit ScenePass(ScenePass::PassType type);
			virtual ~ScenePass();

			virtual void AddPass(ScenePass* pPass) {};

			virtual void Render(Renderer* pRenderer, RenderQueue* pRenderQueue, uint32_t currentBufferIdx,
								ScenePass::OnBeginFN OnBegin, ScenePass::OnEndFN OnEnd) {}
			virtual void Update(Renderer* pRenderer, RenderQueue* pRenderQueue, Camera* pCamera, float32_t crrTime) {}

			const ScenePass::PassType& GetPassType() const;
			const ScenePass::ScenePassMap& GetPasses() const;
			ScenePass::ScenePassMap& GetPasses();

			const ScenePass::PassData& Data() const;
			ScenePass::PassData& Data();

			void AddRenderTarget(RenderTarget* pRenderTarget);
			RenderTarget* GetRenderTarget(RenderTarget::TargetType targetType);
			const ScenePass::RenderTargetMap& GetRenderTargets() const;

			static std::string PassTypeToStr(ScenePass::PassType type);

			struct VisualEffect
			{
				VisualEffect()
					: isEnabled(false)
					, transform(glm::mat4(1.0f))
				{
					cullFaceState.SetCullMode(CullFaceState::CullMode::GE_CM_BACK);
				}

				bool_t isEnabled;

				// different states/data used per effect
				CullFaceState cullFaceState;
				
				glm::mat4 transform;
			};

			const ScenePass::VisualEffect& GetVisualEffect() const;
			void SetVisualEffect(const ScenePass::VisualEffect& effect);

		protected:
			NO_COPY_NO_MOVE_CLASS(ScenePass)

			PassType mPassType;
			ScenePassMap mPassMap;

			PassData mData;
			//TOOD - add support for subpasses!

			RenderTargetMap mRenderTargets;

			VisualEffect mVisualEffect;
		};
	}
}

#endif // GRAPHICS_RENDERING_SCENE_PASSES_SCENE_PASS_HPP