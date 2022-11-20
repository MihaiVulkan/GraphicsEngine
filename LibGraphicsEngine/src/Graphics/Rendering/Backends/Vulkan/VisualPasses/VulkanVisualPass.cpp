#if defined(VULKAN_RENDERER)
#include "Graphics/Rendering/Backends/Vulkan/VisualPasses/VulkanVisualPass.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Common/VulkanCommon.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Common/VulkanUtils.hpp"
#include "Graphics/Rendering/Backends/Vulkan/VulkanRenderer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanDevice.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanCommandBuffer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanSwapChainBuffer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanShaderModule.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanBuffer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanImage.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanImageView.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanDescriptorPool.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanDescriptorSetLayout.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanDescriptorSet.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanPipelineLayout.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanGraphicsPipeline.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanInitializers.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanVertexFormat.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanShader.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanTexture.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanUniformBuffer.hpp"
#include "Graphics/Rendering/VisualEffects/VisualEffect.hpp"
#include "Graphics/Rendering/Resources/RenderTarget.hpp"
#include "Graphics/SceneGraph/GeometryNode.hpp"
#include "Graphics/ShaderTools/GLSL/GLSLShaderParser.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

using namespace std::placeholders;  // for _1, _2, _3...

GADVisualPass::GADVisualPass()
	: mpVulkanRenderer(nullptr)
	, mpDescriptorPool(nullptr)
	, mpDescriptorSetLayout(nullptr)
	, mpDescriptorSet(nullptr)
	, mpPipelineLayout(nullptr)
	, mpGraphicsPipeline(nullptr)
	, mpVisualPass(nullptr)
	, mIsPresentPass(false)
{}

GADVisualPass::GADVisualPass(Renderer* pRenderer, VisualPass* pVisualPass)
	: GADVisualPass()
{
	Init(pRenderer, pVisualPass);
}

GADVisualPass::~GADVisualPass()
{
	GE_FREE(mpGraphicsPipeline);
	GE_FREE(mpPipelineLayout);

	GE_FREE(mpDescriptorSet);
	GE_FREE(mpDescriptorSetLayout);
	GE_FREE(mpDescriptorPool);

	mpVulkanRenderer = nullptr;
}

void GADVisualPass::Init(Renderer* pRenderer, VisualPass* pVisualPass)
{
	assert(pRenderer != nullptr);
	assert(pVisualPass != nullptr);

	mpVulkanRenderer = dynamic_cast<VulkanRenderer*>(pRenderer);
	assert(mpVulkanRenderer != nullptr);

	mpVisualPass = pVisualPass;

	mIsPresentPass = (mpVisualPass->GetPassType() == VisualPass::PassType::GE_PT_STANDARD);

	SetupDescriptorSets();
	SetupPipeline();
}

void GADVisualPass::SetupDescriptorSets()
{
	assert(mpVulkanRenderer != nullptr);
	assert(mpVisualPass != nullptr);

	VulkanDevice* pDevice = mpVulkanRenderer->GetDevice();
	assert(pDevice != nullptr);

	// collect descriptor sets
	const auto& shaders = mpVisualPass->GetShaders();
	for (auto iter = shaders.begin(); iter != shaders.end(); ++iter)
	{
		auto* pShader = iter->second;
		if (pShader)
		{
			//NOTE! At this point the shader parser already parsed the shader source code

			auto* pParser = pShader->GetGLSLParser();
			assert(pParser != nullptr);

			// COMPUTE DESCRIPTOR SET INFO

			VkShaderStageFlagBits shaderStage = VulkanUtils::ShaderStageToVulkanShaderStage(pShader->GetShaderStage());

			// uniform buffer is present
			if (pParser->GetUniformBlock().IsValid())
			{
				// uniform data
				GADRUniformBuffer* pGadrUniformBuffer = nullptr;
				if (mpVisualPass->HasUniformBuffers()) //get available one
				{
					pGadrUniformBuffer = mpVulkanRenderer->Get(mpVisualPass->GetUniformBuffer(pShader->GetShaderStage()));
				}
				else
				{
					//otherwise make one of our own
					//pVisComp->AddUniformBuffer();

				}
				assert(pGadrUniformBuffer != nullptr);

				// descriptor metadata
				VkDescriptorType descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

				AddWriteDescriptorSet(shaderStage, pParser->GetUniformBlock().setId, pParser->GetUniformBlock().binding,
					descriptorType, nullptr, &(pGadrUniformBuffer->GetVkBuffer()->GetDescriptorInfo()));
			}

			// texture sampler(s) are present
			if (pParser->GetUniforms().empty() == false)
			{
				//TODO - for now we know its only samplers
				VkDescriptorType descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

				auto& uniforms = pParser->GetUniforms();

				if (mpVisualPass->HasTextures())
				{
					auto& textureMap = mpVisualPass->GetTextures();
					auto& textureArray = textureMap.at(pShader->GetShaderStage());

					assert(textureArray.size() == uniforms.size()); //TODO - improve this

					auto texIter = textureArray.begin();
					auto unifIter = uniforms.begin();
					for (; texIter != textureArray.end(); ++texIter, ++unifIter)
					{
						auto* pTexture = *texIter;

						if (pTexture->GetUsageType() == Texture::UsageType::GE_UT_RENDER)
						{
							auto* gadrTexture = mpVulkanRenderer->Get(pTexture);
							assert(gadrTexture != nullptr);

							AddWriteDescriptorSet(shaderStage, unifIter->second.setId, unifIter->second.binding,
								descriptorType, &(gadrTexture->GetVkDescriptorInfo()), nullptr);
						}
						else if (pTexture->GetUsageType() == Texture::UsageType::GE_UT_RENDER_TARGET)
						{
							auto* pVisualEffect = mpVisualPass->GetVisualEffect();
							assert(pVisualEffect != nullptr);

							const auto& passMap = pVisualEffect->GetPasses();

							bool_t foundRT = false;
							VisualEffect::PassMap::const_iterator cPassIt;
							VisualPass::RenderTargetMap::const_iterator cRTIt;
							for (cPassIt = passMap.begin(); cPassIt != passMap.end(); ++cPassIt)
							{
								auto& passVector = cPassIt->second;
								for (auto* pPass : passVector)
								{
									if (pPass)
									{
										const auto& renderTargets = pPass->GetRenderTargets();
										for (cRTIt = renderTargets.begin(); cRTIt != renderTargets.end(); ++cRTIt)
										{
											auto* pRT = cRTIt->second;
											assert(pRT != nullptr);

											if (pRT->GetTexture() == pTexture)
											{
												foundRT = true;
												break;
											}
										}
									}
								}

								if (foundRT)
									break;
							}

							if (foundRT)
							{
								auto* pRT = cRTIt->second;

								auto* gadrTexture = mpVulkanRenderer->Get(pRT);
								assert(gadrTexture != nullptr);

								AddWriteDescriptorSet(shaderStage, unifIter->second.setId, unifIter->second.binding,
									descriptorType, &(gadrTexture->GetVkDescriptorInfo()), nullptr);
							}
						}
					}
				}
			}
		}
	}

	//// setup Descriptor pool
	{
		std::vector<VkDescriptorPoolSize> poolSizes;
		for (auto iter = mDescriptorSetBindingMap.begin(); iter != mDescriptorSetBindingMap.end(); ++iter)
		{
			auto descriptorSetId = iter->first;
			uint32_t descriptorCount = static_cast<uint32_t>(iter->second.size());

			auto& setBindingDataVector = iter->second;

			for (auto& bindingData : setBindingDataVector)
			{
				if (poolSizes.empty())
				{
					VkDescriptorPoolSize poolSize{};
					poolSize.type = bindingData.layoutBinding.descriptorType;
					poolSize.descriptorCount = bindingData.layoutBinding.descriptorCount;

					poolSizes.push_back(poolSize);
				}
				else
				{
					bool_t found = false;
					for (auto& poolSize : poolSizes)
					{
						if (bindingData.layoutBinding.descriptorType == poolSize.type)
						{
							poolSize.descriptorCount += bindingData.layoutBinding.descriptorCount;
							found = true;
							break;
						}
					}

					if (found == false)
					{
						VkDescriptorPoolSize poolSize{};
						poolSize.type = bindingData.layoutBinding.descriptorType;
						poolSize.descriptorCount = bindingData.layoutBinding.descriptorCount;

						poolSizes.push_back(poolSize);
					}
				}
			}
		}

		//////////////

		uint32_t descriptorSetMaxCount = poolSizes.size();

		mpDescriptorPool = GE_ALLOC(VulkanDescriptorPool)(pDevice, descriptorSetMaxCount, poolSizes);
		assert(mpDescriptorPool != nullptr);
	}

	// setup Descriptor sets
	{
		for (auto iter = mDescriptorSetBindingMap.begin(); iter != mDescriptorSetBindingMap.end(); ++iter)
		{
			auto& setId = iter->first;
			auto& setBindingDataVector = iter->second;

			// Basically connects the different shader stages to descriptors for binding uniform buffers, image samplers, etc.
			std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
			for (auto& bindingData : setBindingDataVector)
			{
				layoutBindings.push_back(bindingData.layoutBinding);
			}

			//TODO - for now we reuse the same descriptor set layout for all descriptor sets!
			mpDescriptorSetLayout = GE_ALLOC(VulkanDescriptorSetLayout)(pDevice, layoutBindings);
			assert(mpDescriptorSetLayout != nullptr);

			// Allocate a new descriptor set from the descriptor pool
			mpDescriptorSet = GE_ALLOC(VulkanDescriptorSet)(pDevice, mpDescriptorPool, setId, { mpDescriptorSetLayout });
			assert(mpDescriptorSet != nullptr);

			std::vector<VkWriteDescriptorSet> writeDescriptorSets;
			for (auto& bindingData : setBindingDataVector)
			{
				bindingData.writeSet.dstSet = mpDescriptorSet->GetHandle();

				writeDescriptorSets.push_back(bindingData.writeSet);
			}

			//TODO - for now only write descriptor sets
			mpDescriptorSet->Update(writeDescriptorSets, {});
		}
	}
}

void GADVisualPass::SetupPipeline()
{
	assert(mpVulkanRenderer != nullptr);
	assert(mpVisualPass != nullptr);

	////  Shaders state
	std::vector<VkPipelineShaderStageCreateInfo> pipelineShaderStages;
	SetupShaderStage(pipelineShaderStages);

	//// Vertex input state
	VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{};
	SetupVertexInputState(pipelineVertexInputStateCreateInfo);

	//// Input assembly state describes how primitives are assembled
	//// This pipeline will assemble vertex data as a triangle lists
	VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};
	SetupPrimitiveAssemblyState(pipelineInputAssemblyStateCreateInfo);

	//// Viewport state sets the number of viewports and scissor used in this pipeline
	//// Note: This is actually overriden by the dynamic states (see below)
	VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{};
	SetupViewportScissorState(pipelineViewportStateCreateInfo);

	//// Rasterization state
	VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{};
	SetupRasterizationState(pipelineRasterizationStateCreateInfo);

	// Multi sampling state
	// This example does not make use fo multi sampling (for anti-aliasing), the state must still be set and passed to the pipeline
	VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo{};
	SetupMultisampleState(pipelineMultisampleStateCreateInfo);

	//// Depth and stencil state containing depth and stencil compare and test operations
	//// We only use depth tests and want depth tests and writes to be enabled and compare with less or equal
	VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo{};
	SetupDepthStencilState(pipelineDepthStencilStateCreateInfo);

	// Color blend state describes how blend factors are calculated (if used)
	// We need one blend attachment state per color attachment (even if blending is not used)
	VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{};
	SetupColorBlendState(pipelineColorBlendStateCreateInfo);

	// Enable dynamic states
	// Most states are baked into the pipeline, but there are still a few dynamic states that can be changed within a command buffer
	// To be able to change these we need do specify which dynamic states will be changed using this pipeline. Their actual states are set later on in the command buffer.
	// For this example we will set the viewport and scissor using dynamic states
	VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
	SetupDynamicState(pipelineDynamicStateCreateInfo);

	///////////////////
	VulkanDevice* pDevice = mpVulkanRenderer->GetDevice();
	assert(pDevice != nullptr);

	mpPipelineLayout = GE_ALLOC(VulkanPipelineLayout)
		(
			pDevice,
			{ mpDescriptorSetLayout }, {}
	);
	assert(mpPipelineLayout != nullptr);

	//// Assign the pipeline states to the pipeline creation info structure
	mpGraphicsPipeline = GE_ALLOC(VulkanGraphicsPipeline)
		(
			pDevice,
			mpVulkanRenderer->GetPipelineCache(),
			pipelineShaderStages,
			pipelineVertexInputStateCreateInfo, pipelineInputAssemblyStateCreateInfo, {},
			pipelineViewportStateCreateInfo, pipelineRasterizationStateCreateInfo,
			pipelineMultisampleStateCreateInfo, pipelineDepthStencilStateCreateInfo,
			pipelineColorBlendStateCreateInfo, pipelineDynamicStateCreateInfo,
			mpPipelineLayout, 
			mpVulkanRenderer->GetRenderPass(mpVisualPass)
			);
	assert(mpGraphicsPipeline != nullptr);
}

void GADVisualPass::SetupShaderStage(std::vector<VkPipelineShaderStageCreateInfo>& shaderStagesOut)
{
	assert(mpVulkanRenderer != nullptr);
	assert(mpVisualPass != nullptr);

	shaderStagesOut.clear();

	// setup Shaders
	const auto& shaders = mpVisualPass->GetShaders();

	// pipeline shader stages data
	shaderStagesOut.resize(shaders.size());

	size_t i = 0;
	for (auto iter = shaders.begin(); iter != shaders.end(); ++iter)
	{
		auto* pShader = iter->second;
		assert(pShader != nullptr);

		VkShaderStageFlagBits shaderStage = VulkanUtils::ShaderStageToVulkanShaderStage(pShader->GetShaderStage());

		// pipeline shader state setup
		auto GADRShader = mpVulkanRenderer->Get(pShader);
		assert(GADRShader != nullptr);

		auto refVkShader = GADRShader->GetVkShaderModule();
		assert(refVkShader != nullptr);

		shaderStagesOut[i++] = VulkanInitializers::PipelineShaderStageCreateInfo
		(
			shaderStage,
			refVkShader->GetHandle(),
			SHADER_ENTRY_POINT
		);
	}
}

void GADVisualPass::SetupVertexInputState(VkPipelineVertexInputStateCreateInfo& pipelineVertexInputStateCreateInfoOut)
{
	// NOTE! We need a geometry independent vertex format !!!
	// https://vulkan.lunarg.com/doc/view/1.2.148.0/mac/chunked_spec/chap22.html#fxvertex-example

	assert(mpVulkanRenderer != nullptr);
	assert(mpVisualPass != nullptr);

	if (mpVisualPass->GetIsDebug())
	{
		pipelineVertexInputStateCreateInfoOut =
			VulkanInitializers::PipelineVertexInputStateCreateInfo(0, nullptr, 0, nullptr);
	}

	auto* pGeoNode = mpVisualPass->GetNode();
	assert(pGeoNode != nullptr);

	auto* pGeometry = pGeoNode->GetGeometry();
	assert(pGeometry != nullptr);

	auto* pVertexFormat = pGeometry->GetVertexFormat();
	assert(pVertexFormat != nullptr);

	// setup vertex input
	GADRVertexFormat* gadrVertexFormat = mpVulkanRenderer->Get(pVertexFormat);
	assert(gadrVertexFormat != nullptr);

	Shader* pVertexShader = mpVisualPass->GetShader(Shader::ShaderStage::GE_SS_VERTEX);
	assert(pVertexShader != nullptr);

	GLSLShaderParser* pGlslParser = pVertexShader->GetGLSLParser();
	assert(pGlslParser != nullptr);

	auto shaderAttribs = pGlslParser->GetVertexAttributes();

	// update attribute locations based on the current used vertex shader
	auto inputAttribs = gadrVertexFormat->GetVkInputAttributes();

	// the shader can use a smaller number of attributes compared to input geometry data
	assert(shaderAttribs.size() <= inputAttribs.size());

	std::vector<VkVertexInputAttributeDescription> updatedShaderAttribs(shaderAttribs.size());

	size_t index = 0;
	for (auto shaderAttIter = shaderAttribs.begin(); shaderAttIter != shaderAttribs.end(); ++shaderAttIter)
	{
		auto& refAtt = updatedShaderAttribs[index];
		refAtt = inputAttribs[shaderAttIter->first];
		auto refInput = shaderAttIter->second.pInput;
		if (refInput)
			refAtt.location = refInput->location;

		index++;
	}

	const auto& inputBinding = gadrVertexFormat->GetVkInputBinding();

	/////////////////////////////////
	pipelineVertexInputStateCreateInfoOut =
		VulkanInitializers::PipelineVertexInputStateCreateInfo(1, &inputBinding, static_cast<uint32_t>(updatedShaderAttribs.size()), updatedShaderAttribs.data());
}

void GADVisualPass::SetupPrimitiveAssemblyState(VkPipelineInputAssemblyStateCreateInfo& pipelineInputAssemblyStateCreateInfoOut)
{
	assert(mpVisualPass != nullptr);

	// Input assembly state describes how primitives are assembled
	// This pipeline will assemble vertex data as a triangle lists

	//TODO - decouple topology from geometry
	VkPrimitiveTopology vulkanTopology = VulkanUtils::PrimitiveTopologyToVulkanTopolgy(mpVisualPass->GetPrimitiveTopology());

	pipelineInputAssemblyStateCreateInfoOut =
		VulkanInitializers::PipelineInputAssemblyStateCreateInfo(vulkanTopology, VK_FALSE);
}

void GADVisualPass::SetupViewportScissorState(VkPipelineViewportStateCreateInfo& pipelineViewportStateCreateInfoOut)
{
	assert(mpVulkanRenderer != nullptr);
	assert(mpVisualPass != nullptr);

	// viewport & scicors state
	auto& dynamicState = mpVisualPass->GetDynamicState();

	// Viewport state sets the number of viewports and scissor used in this pipeline
	// Note: This is actually overriden by the dynamic states (see below)

	// if viewport and scisosrs are part of dynamic state, we don't handlle them here!
	if (dynamicState.HasStates())
	{
		pipelineViewportStateCreateInfoOut = VulkanInitializers::PipelineViewportStateCreateInfo(MIN_NUM_VIEWPORTS, nullptr, MIN_NUM_SCISSORS, nullptr);
	}
	else
	{
		VkExtent2D extent { mpVulkanRenderer->GetWindowWidth(), mpVulkanRenderer->GetWindowHeight() };

		VkViewport viewport{};
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = static_cast<float32_t>(extent.width);
		viewport.height = static_cast<float32_t>(extent.width);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = VkOffset2D{ 0, 0 };
		scissor.extent = extent;

		pipelineViewportStateCreateInfoOut = VulkanInitializers::PipelineViewportStateCreateInfo(1, &viewport, 1, &scissor);
	}
}

void GADVisualPass::SetupRasterizationState(VkPipelineRasterizationStateCreateInfo& pipelineRasterizationStateCreateInfoOut)
{
	assert(mpVisualPass != nullptr);

	// Rasterization state

	VkPolygonMode vulkanPolygonMode = VulkanUtils::PrimitivePolygonModeToVulkanPolygonMode(mpVisualPass->GetPolygonMode());

	VkFrontFace vulkanFaceWinding = VulkanUtils::PrimitiveFaceWindingToVulkanFaceWinding(mpVisualPass->GetFaceWinding());

	VkCullModeFlagBits vulkanCullMode = VulkanUtils::FaceCullModeToVulkanFaceCullMode(mpVisualPass->GetCullFaceState().GetCullMode());

	if (mpVisualPass->GetIsDebug())
	{
		vulkanCullMode = VkCullModeFlagBits::VK_CULL_MODE_NONE;
	}

	VkBool32 depthBiasEnable = VK_FALSE;
	if (mpVisualPass->GetPassType() == VisualPass::PassType::GE_PT_SHADOWS)
	{
		depthBiasEnable = VK_TRUE;
	}

	pipelineRasterizationStateCreateInfoOut =
		VulkanInitializers::PipelineRasterizationStateCreateInfo(VK_FALSE, VK_FALSE, vulkanPolygonMode, vulkanCullMode,
			vulkanFaceWinding, depthBiasEnable, 0, 0, 0, RASTER_MIN_LINE_WIDTH);
}

void GADVisualPass::SetupMultisampleState(VkPipelineMultisampleStateCreateInfo& pipelineMultisampleStateCreateInfoOut)
{
	//TODO

	// Multi sampling state
	// This example does not make use fo multi sampling (for anti-aliasing), the state must still be set and passed to the pipeline
	pipelineMultisampleStateCreateInfoOut =
		VulkanInitializers::PipelineMultisampleStateCreateInfo(MIN_NUM_SAMPLES, VK_FALSE, 0.0f, nullptr, VK_FALSE, VK_FALSE);
}

void GADVisualPass::SetupDepthStencilState(VkPipelineDepthStencilStateCreateInfo& pipelineDepthStencilStateCreateInfoOut)
{
	assert(mpVisualPass != nullptr);

	// Depth and stencil state containing depth and stencil compare and test operations
	// We only use depth tests and want depth tests and writes to be enabled and compare with less or equal
	auto& depthStencilState = mpVisualPass->GetDepthStencilState();

	auto depthEnabled = depthStencilState.GetIsDepthEnabled();
	auto depthWritable = depthStencilState.GetIsDepthWritable();
	auto depthCompareOp = VulkanUtils::CompareOpToVulkanCompareOp(depthStencilState.GetDepthCompareOp());

	VkStencilOpState stencilOpState{};
	auto stencilEnabled = depthStencilState.GetIsStencilEnabled();

	if (stencilEnabled)
	{
		stencilOpState.failOp = VulkanUtils::StencilOpToVulkanStencilOp(depthStencilState.GetStencilFailOp());
		stencilOpState.passOp = VulkanUtils::StencilOpToVulkanStencilOp(depthStencilState.GetStencilPassDepthPassOp());
		stencilOpState.depthFailOp = VulkanUtils::StencilOpToVulkanStencilOp(depthStencilState.GetStencilPassDepthFailOp());
		stencilOpState.compareOp = VulkanUtils::CompareOpToVulkanCompareOp(depthStencilState.GetStencilCompareOp());
		stencilOpState.compareMask = depthStencilState.GetStencilCompareMask();
		stencilOpState.writeMask = depthStencilState.GetStencilWriteMask();
		stencilOpState.reference = depthStencilState.GetStencilReference();
	}

	// NOTE! Depth can be writable only if enabled!
	pipelineDepthStencilStateCreateInfoOut =
		VulkanInitializers::PipelineDepthStencilStateCreateInfo(depthEnabled, depthEnabled && depthWritable, depthCompareOp, VK_FALSE, stencilEnabled, stencilOpState, stencilOpState, 0, 0);
}

void GADVisualPass::SetupColorBlendState(VkPipelineColorBlendStateCreateInfo& pipelineColorBlendStateCreateInfoOut)
{
	assert(mpVisualPass != nullptr);

	if (mpVisualPass->GetPassType() == VisualPass::PassType::GE_PT_SHADOWS)
	{
		pipelineColorBlendStateCreateInfoOut =
			VulkanInitializers::PipelineColorBlendStateCreateInfo(VK_FALSE, VkLogicOp::VK_LOGIC_OP_NO_OP, 0, nullptr, {});

		return;
	}

	auto& colorBlendState = mpVisualPass->GetColorBlendState();

	auto blendEnabled = colorBlendState.GetIsBlendEnabled();

	// NOTE! We need one blend attachment state per color attachment (even if blending is not used)
	auto srcColorBlendFactor = VulkanUtils::BlendFactorToVulkanBlendFactor(colorBlendState.GetSrcColorBlendFactor());
	auto dstColorBlendFactor = VulkanUtils::BlendFactorToVulkanBlendFactor(colorBlendState.GetDstColorBlendFactor());
	auto colorBlendOp = VulkanUtils::BlendOpToVulkanBlendOp(colorBlendState.GetColorBlendOp());
	auto srcAlphaBlendFactor = VulkanUtils::BlendFactorToVulkanBlendFactor(colorBlendState.GetSrcAlphaBlendFactor());
	auto dstAlphaBlendFactor = VulkanUtils::BlendFactorToVulkanBlendFactor(colorBlendState.GetDstAlphaBlendFactor());
	auto alphaBlendOp = VulkanUtils::BlendOpToVulkanBlendOp(colorBlendState.GetAlphaBlendOp());
	auto colorWriteMask = colorBlendState.GetColorWriteMask();

	VkPipelineColorBlendAttachmentState	pipelineColorBlendAttachmentState{};
	pipelineColorBlendAttachmentState.blendEnable = blendEnabled;
	pipelineColorBlendAttachmentState.srcColorBlendFactor = srcColorBlendFactor;
	pipelineColorBlendAttachmentState.dstColorBlendFactor = dstColorBlendFactor;
	pipelineColorBlendAttachmentState.colorBlendOp = colorBlendOp;
	pipelineColorBlendAttachmentState.srcAlphaBlendFactor = srcAlphaBlendFactor;
	pipelineColorBlendAttachmentState.dstAlphaBlendFactor = dstAlphaBlendFactor;
	pipelineColorBlendAttachmentState.alphaBlendOp = alphaBlendOp;
	pipelineColorBlendAttachmentState.colorWriteMask = colorWriteMask;

	auto constantColor = colorBlendState.GetConstantColor();

	pipelineColorBlendStateCreateInfoOut =
		VulkanInitializers::PipelineColorBlendStateCreateInfo(VK_FALSE, VkLogicOp::VK_LOGIC_OP_NO_OP, 1, &pipelineColorBlendAttachmentState, &constantColor[0]);
}

void GADVisualPass::SetupDynamicState(VkPipelineDynamicStateCreateInfo& pipelineDynamicStateCreateInfoOut)
{
	assert(mpVisualPass != nullptr);

	std::vector<VkDynamicState> vulkanDynamicStates;

	auto& dynamicState = mpVisualPass->GetDynamicState();

	if (dynamicState.HasStates())
	{
		auto dynamicStates = dynamicState.GetStates();
		vulkanDynamicStates.resize(dynamicStates.size());

		for (size_t i = 0; i < vulkanDynamicStates.size(); ++i)
		{
			vulkanDynamicStates[i] = VulkanUtils::DynamicStateToVulkanDynamicState(dynamicStates[i]);
		}

		pipelineDynamicStateCreateInfoOut = VulkanInitializers::PipelineDynamicStateCreateInfo(static_cast<uint32_t>(vulkanDynamicStates.size()), vulkanDynamicStates.data());
	}
	else
	{
		pipelineDynamicStateCreateInfoOut = VulkanInitializers::PipelineDynamicStateCreateInfo(0, nullptr);
	}
}

void GADVisualPass::AddWriteDescriptorSet(VkShaderStageFlagBits shaderStage, uint32_t setId, uint32_t binding, VkDescriptorType descriptorType,
										 const VkDescriptorImageInfo* pDescriptorImageInfo, const VkDescriptorBufferInfo* pDescriptorBufferInfo)
{
	assert(shaderStage < VkShaderStageFlagBits::VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM);

	DescriptorSetBindingData descriptorSetBindingData {};

	// So every shader binding should map to one descriptor set layout binding
	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{};
	descriptorSetLayoutBinding.binding = binding; // taken from the shader
	descriptorSetLayoutBinding.descriptorType = descriptorType;
	descriptorSetLayoutBinding.descriptorCount = 1;
	descriptorSetLayoutBinding.stageFlags = shaderStage; //NOTE! Can have more than one shader stage!

	descriptorSetBindingData.layoutBinding = descriptorSetLayoutBinding;

	VkWriteDescriptorSet writeDescriptorSet = VulkanInitializers::WriteDescriptorSet
	(
		VK_NULL_HANDLE,
		binding,
		0, 1,
		descriptorType,
		pDescriptorImageInfo ? pDescriptorImageInfo : nullptr,
		pDescriptorBufferInfo ? pDescriptorBufferInfo : nullptr
	);

	descriptorSetBindingData.writeSet = writeDescriptorSet;

	auto& setBidingDataVector = mDescriptorSetBindingMap[setId];
	setBidingDataVector.push_back(descriptorSetBindingData);
}

void GADVisualPass::RenderNode(uint32_t currentBufferIdx)
{
	assert(mpVisualPass != nullptr);
	assert(mpVulkanRenderer != nullptr);

	auto* pGeoNode = mpVisualPass->GetNode();
	assert(pGeoNode != nullptr);

	auto* pVulkanCmdBuff = mpVulkanRenderer->GetCommandBuffer(currentBufferIdx);
	assert(pVulkanCmdBuff != nullptr);

	// at this point all processed nodes are allowed for this visual pass !

	// uniform -> descriptorsets bindings
	// Bind descriptor sets describing shader binding points
	mpDescriptorSet->Bind(pVulkanCmdBuff->GetHandle(), VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, mpPipelineLayout->GetHandle());

	//bind pipeline 
	// Bind the rendering pipeline
	// The pipeline (state object) contains all states of the rendering pipeline, binding it will set all the states specified at pipeline creation time

	mpGraphicsPipeline->Bind(pVulkanCmdBuff->GetHandle(), VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS);

	mpVulkanRenderer->DrawNode(mpVisualPass, pGeoNode, currentBufferIdx);
}

void GADVisualPass::UpdateNode(Camera* pCamera, float32_t crrTime)
{
	assert(mpVisualPass != nullptr);
	assert(mpVulkanRenderer != nullptr);

	auto* pGeoNode = mpVisualPass->GetNode();
	assert(pGeoNode != nullptr);

	// at this point all processed nodes are allowed for this visual pass !
	mpVulkanRenderer->UpdateNode(mpVisualPass, pGeoNode, pCamera, crrTime);
}
#endif // VULKAN_RENDERER