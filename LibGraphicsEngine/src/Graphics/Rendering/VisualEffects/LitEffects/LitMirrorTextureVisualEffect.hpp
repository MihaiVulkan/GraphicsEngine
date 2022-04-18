#ifndef GRAPHICS_RENDERING_VISUAL_EFFECTS_LIT_EFFECTS_LIT_MIRROR_TEXTURE_VISUAL_EFFECT_HPP
#define GRAPHICS_RENDERING_VISUAL_EFFECTS_LIT_EFFECTS_LIT_MIRROR_TEXTURE_VISUAL_EFFECT_HPP

#include "Graphics/Rendering/VisualEffects/VisualEffect.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		// aka Planar reflective material

		/* Effect which applies a Lit Mirror effect 2D texture on a geometry */
		class LitMirrorTextureVisualEffect : public VisualEffect
		{
			GE_RTTI(GraphicsEngine::Graphics::LitMirrorTextureVisualEffect)

		public:
			// mirrorRoughness = 0 - perfect mirror
			// mirrorRoughness = 1 - really rough mirror
			explicit LitMirrorTextureVisualEffect(float32_t mirrorRoughness = 0.0f);
			~LitMirrorTextureVisualEffect();

		private:
			NO_COPY_NO_MOVE_CLASS(LitMirrorTextureVisualEffect)

			virtual void InitCustomEffect() override;

			float32_t mMirrorRoughness;
		};
	}
}

#endif // GRAPHICS_RENDERING_VISUAL_EFFECTS_LIT_EFFECTS_LIT_MIRROR_TEXTURE_VISUAL_EFFECT_HPP