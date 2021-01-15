#ifndef GRAPHICS_RENDERING_RESOURCES_RENDER_FRAME_BUFFER_HPP
#define GRAPHICS_RENDERING_RESOURCES_RENDER_FRAME_BUFFER_HPP

#include "Graphics/Rendering/Resources/Resource.hpp"
#include <unordered_map>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class RenderTarget;

		// Shader - used to store render target data to pass to a specific Graphics API
		class RenderFrameBuffer : public Resource
		{
			GE_RTTI(GraphicsEngine::Graphics::RenderFrameBuffer)

		public:

			typedef std::unordered_map<std::string, RenderTarget*> RenderTargetMap;

			enum class ClearFlag : uint8_t
			{
				GE_CF_NONE = 0,
				GE_CF_COLOR,
				GE_CF_DEPTH,
				GE_CF_COLOD_DEPTH,
				GE_CF_COUNT
			};

			RenderFrameBuffer();
			explicit RenderFrameBuffer(uint32_t width, uint32_t height);
			virtual ~RenderFrameBuffer();

			const RenderFrameBuffer::RenderTargetMap& GetRenderTargets() const;

			uint32_t GetWidth() const;
			uint32_t GetHeight() const;
			const Color3f& GetClearColor() const;
			RenderFrameBuffer::ClearFlag GetClearFlag() const;

			void SetClearColor(const Color3f& color);
			void SetClearFlag(RenderFrameBuffer::ClearFlag flag);
				 
		private:
			void Create();
			void Destroy();

			RenderTargetMap mRenderTargetMap;
			uint32_t mWidth, mHeight;
			Color3f mClearColor;
			ClearFlag mClearFlag;
		};
	}
}

#endif // GRAPHICS_RENDERING_RESOURCES_RENDER_FRAME_BUFFER_HPP