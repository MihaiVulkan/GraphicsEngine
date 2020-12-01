#ifndef GRAPHICS_RENDERING_VULKAN_SUBPASS_HPP
#define GRAPHICS_RENDERING_VULKAN_SUBPASS_HPP

#include "Foundation/TypeDefs.hpp"
#include "vulkan/vulkan.h"
#include <vector>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class VulkanSubPass
		{
		public:

			/*
				A subpass represents a phase of rendering that reads and writes a subset of the attachments in a render pass. Rendering commands are recorded
				into a particular subpass of a render pass instance.

				A subpass description describes the subset of attachments that is involved in the execution of a subpass. Each subpass can read from some 
				attachments as input attachments, write to some as color attachments or depth/stencil attachments, perform shader resolve operations to 
				color_attachments or depth/stencil_attachments, and perform multisample resolve operations to resolve attachments. A subpass description 
				can also include a set of preserve attachments, which are attachments that are not read or written by the subpass but whose contents must
				be preserved throughout the subpass.
			*/
			VulkanSubPass();
			explicit VulkanSubPass(const std::vector<VkAttachmentReference>& colorReferences, const VkAttachmentReference& depthStencilReference,
				const std::vector<VkAttachmentReference>& inputReferences, VkPipelineBindPoint pipelineBindPoint, uint32_t id);

			virtual ~VulkanSubPass();

			const VkSubpassDescription& GetSubpassDescription() const;
			uint32_t GetId() const;

		private:
			VkSubpassDescription mSubpassDescription;
			uint32_t mId;
		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_SUBPASS_HPP