#ifndef GRAPHICS_RENDERING_VISUAL_EFFECTS_UNLIT_EFFECTS_UNLIT_COLOR_ATTRIBUTE_VISUAL_EFFECT_HPP
#define GRAPHICS_RENDERING_VISUAL_EFFECTS_UNLIT_EFFECTS_UNLIT_COLOR_ATTRIBUTE_VISUAL_EFFECT_HPP

#include "Graphics/Rendering/VisualEffects/VisualEffect.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		/* Effect which applies a vertex attribute color on geometry */
		class UnlitColorAttributeVisualEffect : public VisualEffect
		{
			GE_RTTI(GraphicsEngine::Graphics::UnlitColorAttributeVisualEffect)

		public:
			UnlitColorAttributeVisualEffect();
			~UnlitColorAttributeVisualEffect();

		private:
			NO_COPY_NO_MOVE_CLASS(UnlitColorAttributeVisualEffect)

			virtual void InitCustomEffect() override;
		};
	}
}

#endif // GRAPHICS_RENDERING_VISUAL_EFFECTS_UNLIT_EFFECTS_UNLIT_COLOR_ATTRIBUTE_VISUAL_EFFECT_HPP