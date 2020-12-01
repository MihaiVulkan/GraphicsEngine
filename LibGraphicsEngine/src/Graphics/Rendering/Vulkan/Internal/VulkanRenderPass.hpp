#ifndef GRAPHICS_RENDERING_VULKAN_RENDERPASS_HPP
#define GRAPHICS_RENDERING_VULKAN_RENDERPASS_HPP

#include "Foundation/TypeDefs.hpp"
#include "vulkan/vulkan.h"
#include <vector>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class VulkanDevice;
		class VulkanRenderPassAttachment;
		class VulkanSubPass;

		/*
			*Wrapper for VKRenderPass object*

			A render pass represents a collection of attachments, subpasses, and dependencies between the subpasses, and describes how the attachments
			are used over the course of the subpasses. The use of a render pass in a command buffer is a render pass instance.

			An attachment description describes the properties of an attachment including its format, sample count, and how its contents are treated at
			the beginning and end of each render pass instance.

			A subpass represents a phase of rendering that reads and writes a subset of the attachments in a render pass. Rendering commands are recorded
			into a particular subpass of a render pass instance.

			Subpass dependencies describe execution and memory dependencies between subpasses.

			A subpass dependency chain is a sequence of subpass dependencies in a render pass, where the source subpass of each subpass dependency 
			(after the first) equals the destination subpass of the previous dependency.

			A render pass describes the structure of subpasses and attachments independent of any specific image views for the attachments. 
			The specific image views that will be used for the attachments, and their dimensions, are specified in VkFramebuffer objects. 
			Framebuffers are created with respect to a specific render pass that the framebuffer is compatible with (see Render Pass Compatibility).
			Collectively, a render pass and a framebuffer define the complete render target state for one or more subpasses as well as the algorithmic
			dependencies between the subpasses.

			An application records the commands for a render pass instance one subpass at a time, by beginning a render pass instance, iterating over the
			subpasses to record commands for that subpass, and then ending the render pass instance.
		
		*/
		class VulkanRenderPass
		{
		public:
			VulkanRenderPass();
			explicit VulkanRenderPass(VulkanDevice* pDevice, const std::vector<VulkanRenderPassAttachment*>& attachments, const std::vector<VulkanSubPass*>& subPasses,
				const std::vector<VkSubpassDependency>& subPassDependencies);
			virtual ~VulkanRenderPass();

			void Begin(VkCommandBuffer commandBufferHandle, VkFramebuffer frameBufferHandle, const VkRect2D& renderArea, const std::vector<VkClearValue>& clearValues,
				VkSubpassContents contents = VK_SUBPASS_CONTENTS_INLINE);
			void End(VkCommandBuffer commandBufferHandle);

			const VkRenderPass& GetHandle() const;

		private:
			void Create(const std::vector<VulkanRenderPassAttachment*>& attachments, const std::vector<VulkanSubPass*>& subPasses,
				const std::vector<VkSubpassDependency>& subPassDependencies);
			void Destroy();

			VulkanDevice* mpDevice;
			VkRenderPass mRenderPassHandle;

		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_RENDERPASS_HPP