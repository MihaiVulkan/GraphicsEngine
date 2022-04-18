#ifndef GRAPHICS_RENDERING_VISUAL_EFFECTS_LIT_EFFECTS_LIT_SHADOW_COLOR_ATTRIBUTE_VISUAL_EFFECT_HPP
#define GRAPHICS_RENDERING_VISUAL_EFFECTS_LIT_EFFECTS_LIT_SHADOW_COLOR_ATTRIBUTE_VISUAL_EFFECT_HPP

#include "Graphics/Rendering/VisualEffects/VisualEffect.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		/* Effect which applies a Lit + shadow vertex attribute color on geometry */
		class LitShadowColorAttributeVisualEffect : public VisualEffect
		{
			GE_RTTI(GraphicsEngine::Graphics::LitShadowColorAttributeVisualEffect)

		public:
			LitShadowColorAttributeVisualEffect();
			~LitShadowColorAttributeVisualEffect();

		private:
			NO_COPY_NO_MOVE_CLASS(LitShadowColorAttributeVisualEffect)

			virtual void InitCustomEffect() override;

			static RenderTarget* mpDepthRT;
		};
	}
}

#endif // GRAPHICS_RENDERING_VISUAL_EFFECTS_LIT_EFFECTS_LIT_SHADOW_COLOR_ATTRIBUTE_VISUAL_EFFECT_HPP