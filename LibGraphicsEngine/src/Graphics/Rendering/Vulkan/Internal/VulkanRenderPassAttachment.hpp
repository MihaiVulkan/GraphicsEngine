#ifndef GRAPHICS_RENDERING_VULKAN_RENDERPASS_ATTACHMENT_HPP
#define GRAPHICS_RENDERING_VULKAN_RENDERPASS_ATTACHMENT_HPP

#include "Foundation/TypeDefs.hpp"
#include "vulkan/vulkan.h"

namespace GraphicsEngine
{
	namespace Graphics
	{
		/*
			Wrapper for the VkAttachmentDescription + id
		*/
		class VulkanRenderPassAttachment
		{
		public:
			enum Type
			{
				COLOR = 0,
				DEPTH_STENCIL,
				COUNT
			};

			VulkanRenderPassAttachment();
			explicit VulkanRenderPassAttachment( VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp,
												VkAttachmentLoadOp stencilLoadOp, VkAttachmentStoreOp stencilStoreOp, VkImageLayout initialLayout, 
												VkImageLayout finalLayout, Type type);

			virtual ~VulkanRenderPassAttachment();


			const VkAttachmentDescription& GetAttachmentDescription() const;;
			const VulkanRenderPassAttachment::Type& GetType() const;

		private:
			VkAttachmentDescription mAttachmentDescription;
			Type mType;
		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_RENDERPASS_ATTACHMENT_HPP