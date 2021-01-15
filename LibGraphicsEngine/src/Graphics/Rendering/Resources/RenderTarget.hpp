#ifndef GRAPHICS_RENDERING_RESOURCES_RENDER_TARGET_HPP
#define GRAPHICS_RENDERING_RESOURCES_RENDER_TARGET_HPP

#include "Graphics/Rendering/Resources/Resource.hpp"
#include <vector>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Texture2D;

		// Shader - used to store render target data to pass to a specific Graphics API
		class RenderTarget : public Resource
		{
			GE_RTTI(GraphicsEngine::Graphics::RenderTarget)

			public:
				enum class TargetType : uint8_t
				{
					GE_TT_COLOR_RGB = 0,
					GE_TT_COLOR_RGBA,
					GE_TT_DEPTH_16,
					GE_TT_DEPTH_24,
					GE_TT_DEPTH_32,
					GE_TT_COUNT
				};

				enum class TargetOutput : uint8_t
				{
					GE_TO_RENDER = 0,
					GE_TO_TEXTURE,
					GE_TO_RENDER_TEXTURE,
					GE_TO_COUNT
				};

				RenderTarget();
				explicit RenderTarget(TargetType type, TargetOutput output, uint32_t width, uint32_t height, bool_t isFloatData);
				virtual ~RenderTarget();

				const RenderTarget::TargetType& GetType() const;
				const RenderTarget::TargetOutput& GetOutput() const;

				uint32_t GetWidth() const;
				uint32_t GetHeight() const;

				Texture2D* GetTexture() const;

		private:
			void Create();
			void Destroy();

			TargetType mType;
			TargetOutput mOutput;
			Texture2D* mpTexture;
			uint32_t mWidth, mHeight;
			bool_t mIsFloatData;
		};
	}
}

#endif // GRAPHICS_RENDERING_RESOURCES_RENDER_TARGET_HPP