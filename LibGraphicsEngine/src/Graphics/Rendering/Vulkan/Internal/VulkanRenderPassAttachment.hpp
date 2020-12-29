#ifndef GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_RENDER_PASS_ATTACHMENT_HPP
#define GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_RENDER_PASS_ATTACHMENT_HPP

#include "Graphics/Rendering/Vulkan/Common/VulkanObject.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		/*
			*Wrapper for the VkAttachmentDescription + AttachmentType*
			
			Just encapsulates all the vulkan render pass attachment data
		*/
		class VulkanRenderPassAttachment : public VulkanObject
		{
			GE_RTTI(GraphicsEngine::Graphics::VulkanRenderPassAttachment)

		public:
			enum class AttachmentType : uint8_t
			{
				GE_AT_COLOR = 0,
				GE_AT_DEPTH_STENCIL,
				GE_AT_COUNT
			};

			VulkanRenderPassAttachment();
			explicit VulkanRenderPassAttachment(VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp,
												VkAttachmentLoadOp stencilLoadOp, VkAttachmentStoreOp stencilStoreOp, VkImageLayout initialLayout, 
												VkImageLayout finalLayout, AttachmentType type);

			virtual ~VulkanRenderPassAttachment();

			const VkAttachmentDescription& GetAttachmentDescription() const;;
			const VulkanRenderPassAttachment::AttachmentType& GetAttachmentType() const;

		private:
			VkAttachmentDescription mAttachmentDescription;
			AttachmentType mAttachmentType;
		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_RENDER_PASS_ATTACHMENT_HPP