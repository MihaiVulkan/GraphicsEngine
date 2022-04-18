#ifndef GRAPHICS_RENDERING_VISUAL_EFFECTS_UNLIT_EFFECTS_UNLIT_2D_TEXTURE_ARRAY_VISUAL_EFFECT_HPP
#define GRAPHICS_RENDERING_VISUAL_EFFECTS_UNLIT_EFFECTS_UNLIT_2D_TEXTURE_ARRAY_VISUAL_EFFECT_HPP

#include "Graphics/Rendering/VisualEffects/VisualEffect.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Texture2DArray;

		/* Effect which applies a 2D texture array on a cube geometry */
		class Unlit2DTextureArrayVisualEffect : public VisualEffect
		{
			GE_RTTI(GraphicsEngine::Graphics::Unlit2DTextureArrayVisualEffect)

		public:
			explicit Unlit2DTextureArrayVisualEffect(Texture2DArray* pTexture2DArray);
			~Unlit2DTextureArrayVisualEffect();

		private:
			NO_COPY_NO_MOVE_CLASS(Unlit2DTextureArrayVisualEffect)

			virtual void InitCustomEffect() override;

			Texture2DArray* mpTexture2DArray;
		};
	}
}

#endif // GRAPHICS_RENDERING_VISUAL_EFFECTS_UNLIT_EFFECTS_UNLIT_2D_TEXTURE_ARRAY_VISUAL_EFFECT_HPP