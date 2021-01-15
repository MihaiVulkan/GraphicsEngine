#ifndef GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_COMPUTE_PIPELINE_HPP
#define GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_COMPUTE_PIPELINE_HPP

#include "Graphics/Rendering/Backends/Vulkan/Common/VulkanObject.hpp"
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
		class VulkanComputePipeline : public VulkanObject
		{
			GE_RTTI(GraphicsEngine::Graphics::VulkanComputePipeline)

		public:
			VulkanComputePipeline();
			explicit VulkanComputePipeline(VulkanDevice* pDevice, VkPipelineCache cacheHandle, const VkPipelineShaderStageCreateInfo& shaderStage,
											VkPipelineLayout layoutHandle, VkPipeline basePipelineHandle = VK_NULL_HANDLE, int32_t basePipelineIndex = -1,
											VkPipelineCreateFlags flags = 0);
			virtual ~VulkanComputePipeline();


			const VkPipeline& GetHandle() const;

		private:
			void Create(VkPipelineCache cacheHandle, const VkComputePipelineCreateInfo& graphicsPipelineCreateInfo);
			void Destroy();

			VulkanDevice* mpDevice;

			VkPipeline mHandle;
		};
	}
}

#endif // GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_COMPUTE_PIPELINE_HPP