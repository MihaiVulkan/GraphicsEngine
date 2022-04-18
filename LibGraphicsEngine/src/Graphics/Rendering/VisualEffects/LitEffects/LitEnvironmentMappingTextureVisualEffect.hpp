#ifndef GRAPHICS_RENDERING_VISUAL_EFFECTS_LIT_EFFECTS_LIT_ENVIRONMENT_MAPPING_TEXTURE_VISUAL_EFFECT_HPP
#define GRAPHICS_RENDERING_VISUAL_EFFECTS_LIT_EFFECTS_LIT_ENVIRONMENT_MAPPING_TEXTURE_VISUAL_EFFECT_HPP

#include "Graphics/Rendering/VisualEffects/VisualEffect.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class TextureCubeMap;

		/* Effect which applies a Lit environment mapping texture on a cube geometry */
		class LitEnvironmentMappingTextureVisualEffect : public VisualEffect
		{
			GE_RTTI(GraphicsEngine::Graphics::LitEnvironmentMappingTextureVisualEffect)

		public:
			enum class MappingType
			{
				GE_MT_REFLECTIVE,
				GE_MT_REFRACTIVE,
				GE_MT_COUNT
			};

			explicit LitEnvironmentMappingTextureVisualEffect(TextureCubeMap* pTextureCubeMap, 
				LitEnvironmentMappingTextureVisualEffect::MappingType mappingType);
			~LitEnvironmentMappingTextureVisualEffect();

		private:
			NO_COPY_NO_MOVE_CLASS(LitEnvironmentMappingTextureVisualEffect)

			virtual void InitCustomEffect() override;

			MappingType mMappingType;
			TextureCubeMap* mpTextureCubeMap;
		};
	}
}

#endif // GRAPHICS_RENDERING_VISUAL_EFFECTS_LIT_EFFECTS_LIT_ENVIRONMENT_MAPPING_TEXTURE_VISUAL_EFFECT_HPP