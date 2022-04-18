#ifndef GRAPHICS_RENDERING_VISUAL_EFFECTS_LIT_EFFECTS_LIT_2D_TEXTURE_VISUAL_EFFECT_HPP
#define GRAPHICS_RENDERING_VISUAL_EFFECTS_LIT_EFFECTS_LIT_2D_TEXTURE_VISUAL_EFFECT_HPP

#include "Graphics/Rendering/VisualEffects/VisualEffect.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Texture2D;

		/* Effect which applies a Lit 2D texture on a cube geometry */
		class Lit2DTextureVisualEffect : public VisualEffect
		{
			GE_RTTI(GraphicsEngine::Graphics::Lit2DTextureVisualEffect)

		public:
			explicit Lit2DTextureVisualEffect(Texture2D* pTexture2D);
			~Lit2DTextureVisualEffect();

		private:
			NO_COPY_NO_MOVE_CLASS(Lit2DTextureVisualEffect)

			virtual void InitCustomEffect() override;

			Texture2D* mpTexture2D;
		};
	}
}

#endif // GRAPHICS_RENDERING_VISUAL_EFFECTS_LIT_EFFECTS_LIT_2D_TEXTURE_VISUAL_EFFECT_HPP