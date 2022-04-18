#ifndef GRAPHICS_RENDERING_BACKENDS_VULKAN_VISUAL_PASSES_VULKAN_VISUAL_PASS_HPP
#define GRAPHICS_RENDERING_BACKENDS_VULKAN_VISUAL_PASSES_VULKAN_VISUAL_PASS_HPP

#include "Graphics/Rendering/Backends/Vulkan/Common/VulkanObject.hpp"
#include "Graphics/Rendering/VisualPasses/VisualPass.hpp"
#include <vector>
#include <unordered_map>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Renderer;
		class RenderQueue;
		class Camera;

		class VulkanRenderer;
		class VulkanDescriptorPool;
		class VulkanDescriptorSetLayout;
		class VulkanDescriptorSet;
		class VulkanPipelineLayout;
		class VulkanGraphicsPipeline;

		/* Base class for Graphics API Dependent Visual Pass */
		class GADVisualPass : public VisualPass, public VulkanObject
		{
			GE_RTTI(GraphicsEngine::Graphics::GADVisualPass)

		public:
			GADVisualPass();
			GADVisualPass(Renderer* pRenderer, VisualPass* pVisualPass);
			~GADVisualPass();

			virtual void RenderNode(uint32_t currentBufferIdx) override;
			virtual void UpdateNode(Camera* pCamera, float32_t crrTime) override;

		protected:
			void Init(Renderer* pRenderer, VisualPass* pVisualPass);

			void SetupDescriptorSets();

			void SetupPipeline();
			void SetupShaderStage(std::vector<VkPipelineShaderStageCreateInfo>& shaderStagesOut);
			void SetupVertexInputState(VkPipelineVertexInputStateCreateInfo& pipelineVertexInputStateCreateInfoOut);
			void SetupPrimitiveAssemblyState(VkPipelineInputAssemblyStateCreateInfo& pipelineInputAssemblyStateCreateInfoOut);
			void SetupViewportScissorState(VkPipelineViewportStateCreateInfo& pipelineViewportStateCreateInfoOut);
			void SetupRasterizationState(VkPipelineRasterizationStateCreateInfo& pipelineRasterizationStateCreateInfoOut);
			void SetupMultisampleState(VkPipelineMultisampleStateCreateInfo& pipelineMultisampleStateCreateInfoOut);
			void SetupDepthStencilState(VkPipelineDepthStencilStateCreateInfo& pipelineDepthStencilStateCreateInfoOut);
			void SetupColorBlendState(VkPipelineColorBlendStateCreateInfo& pipelineColorBlendStateCreateInfoOut);
			void SetupDynamicState(VkPipelineDynamicStateCreateInfo& pipelineDynamicStateCreateInfoOut);

			void AddWriteDescriptorSet(VkShaderStageFlagBits shaderStage, uint32_t setId, uint32_t binding, VkDescriptorType descriptorType,
										const VkDescriptorImageInfo* pDescriptorImageInfo, const VkDescriptorBufferInfo* pDescriptorBufferInfo);

			VulkanRenderer* mpVulkanRenderer;

			////////// descriptor/uniform metadata
			struct DescriptorSetBindingData
			{
				VkDescriptorSetLayoutBinding layoutBinding;
				VkWriteDescriptorSet writeSet;
				//	VkCopyDescriptorSet copySet; //TODO
			};

			// key - is descriptor setId, value - vector of descriptor binding data
			std::unordered_map<uint32_t, std::vector<DescriptorSetBindingData>> mDescriptorSetBindingMap;

			// descriptor
			VulkanDescriptorPool* mpDescriptorPool;
			VulkanDescriptorSetLayout* mpDescriptorSetLayout;
			VulkanDescriptorSet* mpDescriptorSet;

			// pipeline 
			VulkanPipelineLayout* mpPipelineLayout;
			VulkanGraphicsPipeline* mpGraphicsPipeline;

			VisualPass* mpVisualPass;
			bool_t mIsPresentPass; // default pass - to present to screen

		private:
			NO_COPY_NO_MOVE_CLASS(GADVisualPass)

		};
	}
}

#endif // GRAPHICS_RENDERING_BACKENDS_VULKAN_VISUAL_PASSES_VULKAN_VISUAL_PASS_HPP