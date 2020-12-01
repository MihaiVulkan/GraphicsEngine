#ifndef GRAPHICS_RENDERING_RESOURCES_RENDER_TARGET_HPP
#define GRAPHICS_RENDERING_RESOURCES_RENDER_TARGET_HPP

#include "Resource.hpp"
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
				enum class Type : uint8_t
				{
					T_COLOR_RGB = 0,
					T_COLOR_RGBA,
					T_DEPTH_16,
					T_DEPTH_24,
					T_DEPTH_32,
					T_COUNT
				};

				enum class Output : uint8_t
				{
					O_RENDER = 0,
					O_TEXTURE,
					O_RENDER_TEXTURE,
					O_COUNT
				};

				RenderTarget();
				explicit RenderTarget(Type type, Output output, uint32_t width, uint32_t height, bool_t isFloatData);
				virtual ~RenderTarget();

				const RenderTarget::Type& GetType() const;
				const RenderTarget::Output& GetOutput() const;

				uint32_t GetWidth() const;
				uint32_t GetHeight() const;

				Texture2D* GetTexture() const;

		private:
			void Create();
			void Destroy();

			Type mType;
			Output mOutput;
			Texture2D* mpTexture;
			uint32_t mWidth, mHeight;
			bool_t mIsFloatData;
		};
	}
}

#endif // GRAPHICS_RENDERING_RESOURCES_RENDER_TARGET_HPP