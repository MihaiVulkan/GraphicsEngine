#ifndef GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_FRAMEBUFFER_HPP
#define GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_FRAMEBUFFER_HPP

#include "Graphics/Rendering/Vulkan/Common/VulkanObject.hpp"
#include <vector>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class VulkanDevice;
		class VulkanRenderPass;

		/*
			*Wrapper for VkFramebuffer object*

			Render passes operate in conjunction with framebuffers. Framebuffers represent a collection of specific memory attachments 
			that a render pass instance uses.

			A framebuffer holds the image views for the attachments the render pass uses.

			Framebuffers and graphics pipelines are created based on a specific render pass object. They must only be used with that
			render pass object, or one compatible with it.

			A framebuffer is compatible with a render pass if it was created using the same render pass or a compatible render pass.


		*/
		class VulkanFrameBuffer : public VulkanObject
		{
			GE_RTTI(GraphicsEngine::Graphics::VulkanDescriptorSet)

		public:
			VulkanFrameBuffer();
			explicit VulkanFrameBuffer(VulkanDevice* pDevice, VulkanRenderPass* pRenderPass, const std::vector<VkImageView>& attachments, uint32_t width, uint32_t height);
			virtual ~VulkanFrameBuffer();

			const VkFramebuffer& GetHandle() const;

		private:
			void Create(const std::vector<VkImageView>& attachments, uint32_t width, uint32_t height);
			void Destroy();

			VulkanDevice* mpDevice;
			VulkanRenderPass* mpRenderPass;
			VkFramebuffer mHandle;
		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_FRAMEBUFFER_HPP