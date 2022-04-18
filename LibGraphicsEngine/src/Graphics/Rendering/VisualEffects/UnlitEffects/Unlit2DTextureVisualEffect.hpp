#ifndef GRAPHICS_RENDERING_VISUAL_EFFECTS_UNLIT_EFFECTS_UNLIT_2D_TEXTURE_VISUAL_EFFECT_HPP
#define GRAPHICS_RENDERING_VISUAL_EFFECTS_UNLIT_EFFECTS_UNLIT_2D_TEXTURE_VISUAL_EFFECT_HPP

#include "Graphics/Rendering/VisualEffects/VisualEffect.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Texture2D;

		/* Effect which applies a 2D texture on a cube geometry */
		class Unlit2DTextureVisualEffect : public VisualEffect
		{
			GE_RTTI(GraphicsEngine::Graphics::Unlit2DTextureVisualEffect)

		public:
			explicit Unlit2DTextureVisualEffect(Texture2D* pTexture2D);
			~Unlit2DTextureVisualEffect();

		private:
			NO_COPY_NO_MOVE_CLASS(Unlit2DTextureVisualEffect)

			virtual void InitCustomEffect() override;

			Texture2D* mpTexture2D;
		};
	}
}

#endif // GRAPHICS_RENDERING_VISUAL_EFFECTS_UNLIT_EFFECTS_UNLIT_2D_TEXTURE_VISUAL_EFFECT_HPP