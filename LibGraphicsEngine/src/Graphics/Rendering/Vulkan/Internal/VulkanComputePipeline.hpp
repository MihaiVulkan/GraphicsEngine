#ifndef GRAPHICS_RENDERING_VULKAN_COMPUTE_PIPELINE_HPP
#define GRAPHICS_RENDERING_VULKAN_COMPUTE_PIPELINE_HPP

#include "Foundation/TypeDefs.hpp"
#include "vulkan/vulkan.h"
#include <vector>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class VulkanDevice;
		class VulkanPipelineCache;
		class VulkanPipelineLayout;

		/*
			*Wrapper for Compute VkPipeline object*

			Compute pipelines consist of a single static compute shader stage and the pipeline layout.

		*/
		class VulkanComputePipeline
		{
		public:
			VulkanComputePipeline();
			explicit VulkanComputePipeline(VulkanDevice* pDevice, VulkanPipelineCache* pCache, const VkPipelineShaderStageCreateInfo& shaderStage,
								  VulkanPipelineLayout* pLayout, VkPipeline basePipelineHandle = VK_NULL_HANDLE, int32_t basePipelineIndex = -1,
				                  VkPipelineCreateFlags flags = 0);
			virtual ~VulkanComputePipeline();


			const VkPipeline& GetHandle() const;

		private:
			void Create(VulkanPipelineCache* pCache, const VkComputePipelineCreateInfo& graphicsPipelineCreateInfo);
			void Destroy();

			VulkanDevice* mpDevice;

			VkPipeline mHandle;
		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_COMPUTE_PIPELINE_HPP