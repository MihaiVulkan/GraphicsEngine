#ifndef GRAPHICS_RENDERING_VISUAL_EFFECTS_UNLIT_EFFECTS_UNLIT_CUBEMAP_TEXTURE_VISUAL_EFFECT_HPP
#define GRAPHICS_RENDERING_VISUAL_EFFECTS_UNLIT_EFFECTS_UNLIT_CUBEMAP_TEXTURE_VISUAL_EFFECT_HPP

#include "Graphics/Rendering/VisualEffects/VisualEffect.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class TextureCubeMap;

		/* Effect which applies a cubemap texture on a cube/sphere geometry */
		class UnlitCubemapTextureVisualEffect : public VisualEffect
		{
			GE_RTTI(GraphicsEngine::Graphics::UnlitCubemapTextureVisualEffect)

		public:
			explicit UnlitCubemapTextureVisualEffect(TextureCubeMap* pTextureCubeMap);
			~UnlitCubemapTextureVisualEffect();

		private:
			NO_COPY_NO_MOVE_CLASS(UnlitCubemapTextureVisualEffect)

			virtual void InitCustomEffect() override;

			TextureCubeMap* mpTextureCubeMap;
		};
	}
}

#endif // GRAPHICS_RENDERING_VISUAL_EFFECTS_UNLIT_EFFECTS_UNLIT_CUBEMAP_TEXTURE_VISUAL_EFFECT_HPP