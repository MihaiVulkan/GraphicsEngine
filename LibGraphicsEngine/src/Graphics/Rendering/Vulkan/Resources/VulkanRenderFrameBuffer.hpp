#ifndef GRAPHICS_RENDERING_VULKAN_RESOURCES_RENDER_FRAME_BUFFER_HPP
#define GRAPHICS_RENDERING_VULKAN_RESOURCES_RENDER_FRAME_BUFFER_HPP

#include "VulkanResource.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Renderer;
		class RenderFrameBuffer;

		class VulkanFrameBuffer;

		// Vulkan implementation of the Graphics API Dependent Resource
		class GADRRenderFrameBuffer : public GADRResource
		{
			GE_RTTI(GraphicsEngine::Graphics::GADRRenderFrameBuffer)

		public:
			GADRRenderFrameBuffer();
			explicit GADRRenderFrameBuffer(Renderer* pRenderer, RenderFrameBuffer* pRenderFrameBuffer);
			virtual ~GADRRenderFrameBuffer();


		private:
			void Create(Renderer* pRenderer);
			void Destroy();


			VulkanFrameBuffer* mpVulkanFrameBuffer;

			RenderFrameBuffer* mpRenderFrameBuffer;

		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_RESOURCES_RENDER_FRAME_BUFFER_HPP