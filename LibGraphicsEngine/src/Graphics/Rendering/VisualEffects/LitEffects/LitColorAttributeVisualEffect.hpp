#ifndef GRAPHICS_RENDERING_VISUAL_EFFECTS_LIT_EFFECTS_LIT_COLOR_ATTRIBUTE_VISUAL_EFFECT_HPP
#define GRAPHICS_RENDERING_VISUAL_EFFECTS_LIT_EFFECTS_LIT_COLOR_ATTRIBUTE_VISUAL_EFFECT_HPP

#include "Graphics/Rendering/VisualEffects/VisualEffect.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		/* Effect which applies a Lit vertex attribute color on geometry */
		class LitColorAttributeVisualEffect : public VisualEffect
		{
			GE_RTTI(GraphicsEngine::Graphics::LitColorAttributeVisualEffect)

		public:
			LitColorAttributeVisualEffect();
			~LitColorAttributeVisualEffect();

		private:
			NO_COPY_NO_MOVE_CLASS(LitColorAttributeVisualEffect)

			virtual void InitCustomEffect() override;
		};
	}
}

#endif // GRAPHICS_RENDERING_VISUAL_EFFECTS_LIT_EFFECTS_LIT_COLOR_ATTRIBUTE_VISUAL_EFFECT_HPP