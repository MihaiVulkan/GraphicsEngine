#ifndef GRAPHICS_RENDERING_RESOURCES_RENDER_FRAME_BUFFER_HPP
#define GRAPHICS_RENDERING_RESOURCES_RENDER_FRAME_BUFFER_HPP

#include "Resource.hpp"
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

			typedef std::unordered_map<std::string, RenderTarget*, std::hash<std::string>> RenderTargetMap;

			enum class ClearFlag : uint8_t
			{
				CF_NONE = 0,
				CF_COLOR,
				CF_DEPTH,
				CF_COLOD_DEPTH,
				CF_COUNT
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