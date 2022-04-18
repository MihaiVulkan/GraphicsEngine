#ifndef GRAPHICS_RENDERING_VISUAL_EFFECTS_LIT_EFFECTS_LIT_COLOR_VISUAL_EFFECT_HPP
#define GRAPHICS_RENDERING_VISUAL_EFFECTS_LIT_EFFECTS_LIT_COLOR_VISUAL_EFFECT_HPP

#include "Graphics/Rendering/VisualEffects/VisualEffect.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		/* Effect which applies a Lit color on a geometry */
		class LitColorVisualEffect : public VisualEffect
		{
			GE_RTTI(GraphicsEngine::Graphics::LitColorVisualEffect)

		public:
			explicit LitColorVisualEffect(const Color4f& color);
			~LitColorVisualEffect();

		private:
			NO_COPY_NO_MOVE_CLASS(LitColorVisualEffect)

			virtual void InitCustomEffect() override;

			Color4f mColor;
		};
	}
}

#endif // GRAPHICS_RENDERING_VISUAL_EFFECTS_LIT_EFFECTS_LIT_COLOR_VISUAL_EFFECT_HPP