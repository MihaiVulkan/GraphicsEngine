#ifndef GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_GRAPHICS_PIPELINE_HPP
#define GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_GRAPHICS_PIPELINE_HPP

#include "Graphics/Rendering/Backends/Vulkan/Common/VulkanObject.hpp"
#include <vector>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class VulkanDevice;
		class VulkanRenderPass;
		class VulkanPipelineCache;
		class VulkanPipelineLayout;

		/*
			*Wrapper for Graphics VkPipeline object*

			Vulkan API supports mainly 3 pipeline types: Graphics, Compute and Raytracing (via extension)

			Graphics pipelines consist of multiple shader stages, multiple fixed - function pipeline stages, and a pipeline layout.

			Multiple pipelines can be created simultaneously.

			A pipeline derivative is a child pipeline created from a parent pipeline, where the child and parent are expected to have much 
			commonality. The goal of derivative pipelines is that they be cheaper to create using the parent as a starting point, and that 
			it be more efficient (on either host or device) to switch/bind between children of the same parent.
		*/
		class VulkanGraphicsPipeline : public VulkanObject
		{
			GE_RTTI(GraphicsEngine::Graphics::VulkanGraphicsPipeline)

		public:
			VulkanGraphicsPipeline();
			explicit VulkanGraphicsPipeline(VulkanDevice* pDevice, VulkanPipelineCache* pCache,
				const std::vector<VkPipelineShaderStageCreateInfo>& shaderStages,
				const VkPipelineVertexInputStateCreateInfo& vertexInputState,
				const VkPipelineInputAssemblyStateCreateInfo& inputAssemblyState,
				const VkPipelineTessellationStateCreateInfo& tessellationState,
				const VkPipelineViewportStateCreateInfo& viewportState,
				const VkPipelineRasterizationStateCreateInfo& rasterizationState,
				const VkPipelineMultisampleStateCreateInfo& MmultisampleState,
				const VkPipelineDepthStencilStateCreateInfo& depthStencilState,
				const VkPipelineColorBlendStateCreateInfo& colorBlendState,
				const VkPipelineDynamicStateCreateInfo& dynamicState,
				VulkanPipelineLayout*                 pLayout,
				VulkanRenderPass*                     pRenderPass,
				uint32_t                              subpassId = 0,
				VkPipeline                            basePipelineHandle = VK_NULL_HANDLE,
				int32_t                               basePipelineIndex = -1,
				VkPipelineCreateFlags                 flags = 0);
			virtual ~VulkanGraphicsPipeline();

			void Bind(VkCommandBuffer commandBufferHandle, VkPipelineBindPoint pipelineBindPoint);

			const VkPipeline& GetHandle() const;

		private:
			void Create(VulkanPipelineCache* pCache, const VkGraphicsPipelineCreateInfo& graphicsPipelineCreateInfo);
			void Destroy();

			VulkanDevice* mpDevice;

			VkPipeline mHandle;
		};
	}
}

#endif // GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_GRAPHICS_PIPELINE_HPP