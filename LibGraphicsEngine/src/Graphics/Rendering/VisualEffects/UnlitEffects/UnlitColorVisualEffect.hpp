#ifndef GRAPHICS_RENDERING_VISUAL_EFFECTS_UNLIT_EFFECTS_UNLIT_COLOR_VISUAL_EFFECT_HPP
#define GRAPHICS_RENDERING_VISUAL_EFFECTS_UNLIT_EFFECTS_UNLIT_COLOR_VISUAL_EFFECT_HPP

#include "Graphics/Rendering/VisualEffects/VisualEffect.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		/* Effect which applies a color on a geometry */
		class UnlitColorVisualEffect : public VisualEffect
		{
			GE_RTTI(GraphicsEngine::Graphics::UnlitColorVisualEffect)

		public:
			explicit UnlitColorVisualEffect(const Color4f& color);
			~UnlitColorVisualEffect();

		private:
			NO_COPY_NO_MOVE_CLASS(UnlitColorVisualEffect)

			virtual void InitCustomEffect() override;

			Color4f mColor;
		};
	}
}

#endif // GRAPHICS_RENDERING_VISUAL_EFFECTS_UNLIT_EFFECTS_UNLIT_COLOR_VISUAL_EFFECT_HPP