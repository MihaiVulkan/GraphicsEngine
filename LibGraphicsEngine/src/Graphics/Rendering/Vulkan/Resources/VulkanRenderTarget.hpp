#ifndef GRAPHICS_RENDERING_VULKAN_RESOURCES_VULKAN_RENDER_TARGET_HPP
#define GRAPHICS_RENDERING_VULKAN_RESOURCES_VULKAN_RENDER_TARGET_HPP

#include "VulkanResource.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Renderer;
		class RenderTarget;

		//class VulkanFrameBufferAttachment;

		// Vulkan implementation of the Graphics API Dependent Resource
		class GADRRenderTarget : public GADRResource
		{
			GE_RTTI(GraphicsEngine::Graphics::GADRRenderTarget)

		public:
			GADRRenderTarget();
			explicit GADRRenderTarget(Renderer* pRenderer, RenderTarget* pRenderTarget);
			virtual ~GADRRenderTarget();

			//void Bind(uint32_t currentBufferIdx);


		private:
			void Create(Renderer* pRenderer);
			void Destroy();

			
			//VulkanFrameBufferAttachment* mpVulkanFrameBufferAttachment;

			RenderTarget* mpRenderTarget;

		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_RESOURCES_VULKAN_RENDER_TARGET_HPP