#include "VulkanMaterial.hpp"
#include "Graphics/Rendering/Vulkan/VulkanRenderer.hpp"
#include "VulkanVertexFormat.hpp"
#include "VulkanVertexBuffer.hpp"
#include "VulkanUniformBuffer.hpp"
#include "VulkanShader.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanInitializers.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanBuffer.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanShaderModule.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanDescriptorPool.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanDescriptorSet.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanDescriptorSetLayout.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanPipelineLayout.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanGraphicsPipeline.hpp"
#include "Graphics/Rendering/Resources/Material.hpp"
#include "Graphics/SceneGraph/GeometryNode.hpp"
#include "Graphics/GeometricPrimitives/GeometricPrimitive.hpp"
#include "Graphics/Rendering/Vulkan/VulkanUtils.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

#define NUM_DESCRIPTOR_SETS 1
#define UNIFORM_BINDING_ID 0

#define SHADER_ENTRY_POINT "main"
#define VERTEX_INPUT_BIDING_ID 0

/* Number of samples needs to be the same at image creation,      */
/* renderpass creation and pipeline creation.                     */
#define NUM_SAMPLES VK_SAMPLE_COUNT_1_BIT

/* Number of viewports and number of scissors have to be the same */
/* at pipeline creation and in any call to set them dynamically   */
/* They also have to be the same as each other                    */
#define NUM_VIEWPORTS 1
#define NUM_SCISSORS NUM_VIEWPORTS


GADRMaterial::GADRMaterial()
	: mpRenderer(nullptr)
	, mpVulkanDescriptorPool(nullptr)
	, mpVulkanDescriptorSetLayout(nullptr)
	, mpVulkanDescriptorSet(nullptr)
	, mpVulkanPipelineLayout(nullptr)
	, mpVulkanGraphicsPipeline(nullptr)
	, mpMaterial(nullptr)
{}

GADRMaterial::GADRMaterial(Renderer* pRenderer, Material* pMaterial, GeometricPrimitive* pGeoPrimitive)
	: mpRenderer(nullptr)
	, mpVulkanDescriptorPool(nullptr)
	, mpVulkanDescriptorSetLayout(nullptr)
	, mpVulkanDescriptorSet(nullptr)
	, mpVulkanPipelineLayout(nullptr)
	, mpVulkanGraphicsPipeline(nullptr)
	, mpMaterial(pMaterial)
{
	Create(pRenderer, pGeoPrimitive);
}

GADRMaterial::~GADRMaterial()
{
	Destroy();
}

void GADRMaterial::Create(Renderer* pRenderer, GeometricPrimitive* pGeoPrimitive)
{
	assert(pRenderer != nullptr);
	assert(pGeoPrimitive != nullptr);
	assert(mpMaterial != nullptr);

	// pRenderer must be a pointer to VulkanRenderer otherwise the cast will fail!
	mpRenderer = dynamic_cast<VulkanRenderer*>(pRenderer);
	assert(mpRenderer != nullptr);

	VulkanDevice* pDevice = mpRenderer->GetDevice();
	assert(pDevice != nullptr);

	///////////////////////////////

	// setup Shaders
	const auto& shaders = mpMaterial->GetShaders();

	// Shaders
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
	shaderStages.resize(shaders.size());


	for (size_t i = 0; i < shaders.size(); ++i)
	{
		auto GADRShader = mpRenderer->Bind(shaders[i]);
		assert(GADRShader != nullptr);

		shaderStages[i] = VulkanInitializers::PipelineShaderStageCreateInfo(GADRShader->TypeToVulkanType(shaders[i]->GetType()), GADRShader->GetVkShaderModule()->GetHandle(), SHADER_ENTRY_POINT);
	}

	// setup Uniforms - TODO 
	auto gadrUniformBuffer = mpRenderer->Bind(mpMaterial->GetUniformBuffer());
	assert(gadrUniformBuffer != nullptr);

	// setup Textures

	////////////////////////////////////

	// setup Shader Bindings - uniforms, samplers, etc.

	//// setup Descriptor pool
	// This example only uses one descriptor type (uniform buffer) and only requests one descriptor of this type
	std::vector<VkDescriptorPoolSize> descriptorPoolSizes;
	descriptorPoolSizes.push_back(VulkanInitializers::DescriptorPoolSize(VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, NUM_DESCRIPTOR_SETS));

	// For additional types you need to add new entries in the type count list
	// E.g. for two combined image samplers :
	// typeCounts[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	// typeCounts[1].descriptorCount = 2;

	// Create the global descriptor pool
	// All descriptors used in this example are allocated from this pool
	mpVulkanDescriptorPool = GE_ALLOC(VulkanDescriptorPool)(pDevice, NUM_DESCRIPTOR_SETS, descriptorPoolSizes);

	///////////

	// setup Descriptor sets
	// Basically connects the different shader stages to descriptors for binding uniform buffers, image samplers, etc.
	// So every shader binding should map to one descriptor set layout binding

	// Binding 0: Uniform buffer (Vertex shader)
	std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
	descriptorSetLayoutBindings.push_back(VulkanInitializers::DescriptorSetLayoutBinding(UNIFORM_BINDING_ID, VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
											1, VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT));

	mpVulkanDescriptorSetLayout = GE_ALLOC(VulkanDescriptorSetLayout)(pDevice, descriptorSetLayoutBindings);

	// Allocate a new descriptor set from the global descriptor pool
	mpVulkanDescriptorSet = GE_ALLOC(VulkanDescriptorSet)(pDevice, mpVulkanDescriptorPool, { mpVulkanDescriptorSetLayout });

	// Update the descriptor set determining the shader binding points
	// For every binding point used in a shader there needs to be one
	// descriptor set matching that binding point

	// Binding 0 : Uniform buffer
	//// Binds this uniform buffer to binding point 0
	std::vector<VkWriteDescriptorSet> writeDescriptorSets;
	writeDescriptorSets.push_back(VulkanInitializers::WriteDescriptorSet(mpVulkanDescriptorSet->GetHandle(), UNIFORM_BINDING_ID, 0, 1, 
		VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, nullptr, &(gadrUniformBuffer->GetVKBuffer()->GetDescriptorInfo())));

	mpVulkanDescriptorSet->Update(writeDescriptorSets, {});


	////////////

	// Create the graphics pipeline used in this example
	// Vulkan uses the concept of rendering pipelines to encapsulate fixed states, replacing OpenGL's complex state machine
	// A pipeline is then stored and hashed on the GPU making pipeline changes very fast
	// Note: There are still a few dynamic states that are not directly part of the pipeline (but the info that they are used is)

	// Construct the differnent states making up the pipeline

	// Shader info has been setup above !!!

	//std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
	//shaderStages.resize(SHADER_COUNT);

	//// Vertex shader
	//shaderStages[VERTEX_STAGE] = VulkanInitializers::PipelineShaderStageCreateInfo(VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT, mShaderModules[VERTEX_STAGE]->GetHandle(), "main");

	//// Fragment shader
	//shaderStages[FRAGMENT_STAGE] = VulkanInitializers::PipelineShaderStageCreateInfo(VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT, mShaderModules[FRAGMENT_STAGE]->GetHandle(), "main");

	// Vertex input descriptions 
	// Specifies the vertex input parameters for a pipeline

	////////// Vertex data is setup in GADRVertexBuffer !!!!

	// Vertex input binding
	// This example uses a single vertex input binding at binding point 0 (see vkCmdBindVertexBuffers)
	//VkVertexInputBindingDescription vertexInputBindingDescription =
	//	VulkanInitializers::VertexInputBindingDescription(0, sizeof(Vertex), VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX);

	//// Inpute attribute bindings describe shader attribute locations and memory layouts
	//std::array<VkVertexInputAttributeDescription, 2> vertexInputAttributes;
	//// These match the following shader layout (see triangle.vert):
	////	layout (location = 0) in vec3 inPos;
	////	layout (location = 1) in vec3 inColor;

	//vertexInputAttributes[0] = VulkanInitializers::VertexInputAttributeDescription(0, 0, VkFormat::VK_FORMAT_R32G32B32_SFLOAT, OFFSETOF(Vertex, position));
	//vertexInputAttributes[1] = VulkanInitializers::VertexInputAttributeDescription(1, 0, VkFormat::VK_FORMAT_R32G32B32_SFLOAT, OFFSETOF(Vertex, color));

	/////////////

	//bind vertex format
	GADRVertexFormat* gadrVertexFormat = mpRenderer->Bind(pGeoPrimitive->GetVertexFormat());
	assert(gadrVertexFormat != nullptr);

	const auto& inputAtrribs = gadrVertexFormat->GetVkInputAttributes();

	// bind vertex buffer
	GADRVertexBuffer* gadrVertexBuffer = mpRenderer->Bind(pGeoPrimitive->GetVertexBuffer());
	assert(gadrVertexBuffer != nullptr);

	const auto& inputBinding = gadrVertexBuffer->GetVkInputBinding();

	VkPrimitiveTopology vulkanTopology = VulkanUtils::PrimitiveTopologyToVulkanTopolgy(pGeoPrimitive->GetTopology());


	/////////////////////////////////
	VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo =
		VulkanInitializers::PipelineVertexInputStateCreateInfo(1, &inputBinding, inputAtrribs.size(), inputAtrribs.data());

	// Input assembly state describes how primitives are assembled
	// This pipeline will assemble vertex data as a triangle lists (though we only use one triangle)
	VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo =
		VulkanInitializers::PipelineInputAssemblyStateCreateInfo(vulkanTopology, VK_FALSE);


	// Viewport state sets the number of viewports and scissor used in this pipeline
	// Note: This is actually overriden by the dynamic states (see below)
	VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo =
		VulkanInitializers::PipelineViewportStateCreateInfo(NUM_VIEWPORTS, nullptr, NUM_SCISSORS, nullptr);

	// Rasterization state
	VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo =
		VulkanInitializers::PipelineRasterizationStateCreateInfo(VK_FALSE, VK_FALSE, VkPolygonMode::VK_POLYGON_MODE_FILL, VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT,
			VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_FALSE,
			0, 0, 0, 1.0f);

	// Set widning depending on who model vertices are arranged
	pipelineRasterizationStateCreateInfo.frontFace = VkFrontFace::VK_FRONT_FACE_CLOCKWISE;

	// Multi sampling state
	// This example does not make use fo multi sampling (for anti-aliasing), the state must still be set and passed to the pipeline
	VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo =
		VulkanInitializers::PipelineMultisampleStateCreateInfo(VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT, VK_FALSE, 0.0f, nullptr, VK_FALSE, VK_FALSE);

	// Depth and stencil state containing depth and stencil compare and test operations
	// We only use depth tests and want depth tests and writes to be enabled and compare with less or equal

	VkStencilOpState stencilOpState =
		VulkanInitializers::StencilOpState(VkStencilOp::VK_STENCIL_OP_KEEP, VkStencilOp::VK_STENCIL_OP_KEEP, VkStencilOp::VK_STENCIL_OP_KEEP,
			VkCompareOp::VK_COMPARE_OP_ALWAYS, 0, 0, 0);

	VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo =
		VulkanInitializers::PipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, VkCompareOp::VK_COMPARE_OP_LESS_OR_EQUAL, VK_FALSE, VK_FALSE, stencilOpState, stencilOpState, 0, 0);

	// Color blend state describes how blend factors are calculated (if used)
	// We need one blend attachment state per color attachment (even if blending is not used
	VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState =
		VulkanInitializers::PipelineColorBlendAttachmentState(VK_FALSE, VkBlendFactor::VK_BLEND_FACTOR_ZERO, VkBlendFactor::VK_BLEND_FACTOR_ZERO, VkBlendOp::VK_BLEND_OP_ADD,
			VkBlendFactor::VK_BLEND_FACTOR_ZERO, VkBlendFactor::VK_BLEND_FACTOR_ZERO, VkBlendOp::VK_BLEND_OP_ADD, 0xf);

	float blendConstants[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo =
		VulkanInitializers::PipelineColorBlendStateCreateInfo(VK_FALSE, VkLogicOp::VK_LOGIC_OP_NO_OP, 1, &pipelineColorBlendAttachmentState, blendConstants);

	// Enable dynamic states
	// Most states are baked into the pipeline, but there are still a few dynamic states that can be changed within a command buffer
	// To be able to change these we need do specify which dynamic states will be changed using this pipeline. Their actual states are set later on in the command buffer.
	// For this example we will set the viewport and scissor using dynamic states
	std::vector<VkDynamicState> dynamicStateEnables;
	dynamicStateEnables.push_back(VkDynamicState::VK_DYNAMIC_STATE_VIEWPORT);
	dynamicStateEnables.push_back(VkDynamicState::VK_DYNAMIC_STATE_SCISSOR);

	VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo =
		VulkanInitializers::PipelineDynamicStateCreateInfo(static_cast<uint32_t>(dynamicStateEnables.size()), dynamicStateEnables.data());

	///////////////////

	mpVulkanPipelineLayout = GE_ALLOC(VulkanPipelineLayout)(pDevice, { mpVulkanDescriptorSetLayout }, {});

	//// Assign the pipeline states to the pipeline creation info structure
	mpVulkanGraphicsPipeline = GE_ALLOC(VulkanGraphicsPipeline)(pDevice, mpRenderer->GetVkPipelineCache(), shaderStages, pipelineVertexInputStateCreateInfo, pipelineInputAssemblyStateCreateInfo,
		{}, pipelineViewportStateCreateInfo, pipelineRasterizationStateCreateInfo, pipelineMultisampleStateCreateInfo,
		pipelineDepthStencilStateCreateInfo, pipelineColorBlendStateCreateInfo, pipelineDynamicStateCreateInfo,
		mpVulkanPipelineLayout, mpRenderer->GetVkDefaultRenderPass());

}

void GADRMaterial::Destroy()
{
	if (mpMaterial)
	{
		mpMaterial = nullptr;
	}

	// pipeline objects
	GE_FREE(mpVulkanPipelineLayout);
	GE_FREE(mpVulkanGraphicsPipeline);

	// descriptor set objects
	GE_FREE(mpVulkanDescriptorSetLayout);
	GE_FREE(mpVulkanDescriptorSet);
	GE_FREE(mpVulkanDescriptorPool);


}

void GADRMaterial::Bind(uint32_t currentBufferIdx)
{
	assert(mpRenderer != nullptr);

	mpRenderer->BindShaderBindings(currentBufferIdx, mpVulkanDescriptorSet, mpVulkanPipelineLayout, mpVulkanGraphicsPipeline);
}