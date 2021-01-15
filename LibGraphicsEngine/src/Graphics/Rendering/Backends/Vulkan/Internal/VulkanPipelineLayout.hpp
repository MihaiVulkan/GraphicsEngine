#ifndef GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_PIPELINE_LAYOUT_HPP
#define GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_PIPELINE_LAYOUT_HPP

#include "Graphics/Rendering/Backends/Vulkan/Common/VulkanObject.hpp"
#include <vector>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class VulkanDevice;
		class VulkanDescriptorSetLayout;

		/*
			*Wrapper for VkPipelineLayout object*

			Access to descriptor sets from a pipeline is accomplished through a pipeline layout.
			Zero or more descriptor set layouts and zero or more push constant ranges are combined 
			to form a pipeline layout object describing the complete set of resources that can be 
			accessed by a pipeline. The pipeline layout represents a sequence of descriptor sets 
			with each having a specific layout. This sequence of layouts is used to determine the 
			interface between shader stages and shader resources. Each pipeline is created using a
			pipeline layout.
		*/
		class VulkanPipelineLayout : public VulkanObject
		{
			GE_RTTI(GraphicsEngine::Graphics::VulkanPipelineLayout)

		public:
			VulkanPipelineLayout();
			explicit VulkanPipelineLayout(VulkanDevice* pDevice, const std::vector<VulkanDescriptorSetLayout*>& descriptorSetLayouts, const std::vector<VkPushConstantRange>& pushConstantRanges,
									VkPipelineLayoutCreateFlags flags = 0);
			virtual ~VulkanPipelineLayout();

			const VkPipelineLayout& GetHandle() const;

		private:
			void Create(const VkPipelineLayoutCreateInfo& pipelineLayoutCreateInfo);
			void Destroy();

			VulkanDevice* mpDevice;

			VkPipelineLayout mHandle;
		};
	}
}

#endif // GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_PIPELINE_LAYOUT_HPP