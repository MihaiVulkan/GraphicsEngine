#if defined(VULKAN_RENDERER)
#include "Graphics/Rendering/Backends/Vulkan/VulkanRenderer.hpp"

#include "Foundation/Platform/Platform.hpp"

#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"

#include "Graphics/Rendering/RenderQueue.hpp"


#include "Graphics/SceneGraph/GeometryNode.hpp"
#include "Graphics/SceneGraph/LightNode.hpp"
#include "Graphics/Lights/DirectionalLight.hpp"
#include "Graphics/Cameras/Camera.hpp"

#include "Graphics/Components/VisualComponent.hpp"
#include "Graphics/Components/MaterialComponent.hpp"

#include "Graphics/Rendering/VisualEffects/VisualEffect.hpp"


#include "Graphics/Rendering/Resources/Texture.hpp"
#include "Graphics/Rendering/Resources/RenderTarget.hpp"
#include "Graphics/Rendering/Resources/Material.hpp"
#include "Graphics/Rendering/Resources/Model.hpp"
#include "Graphics/Rendering/Resources/UniformBuffer.hpp"

#include "Graphics/ShaderTools/GLSL/GLSLShaderParser.hpp"

// Common
#include "Graphics/Rendering/Backends/Vulkan/Common/VulkanCommon.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Common/VulkanUtils.hpp"

// Resources
#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanVertexFormat.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanVertexBuffer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanIndexBuffer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanUniformBuffer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanShader.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanTexture.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanMaterial.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanModel.hpp"

//Internal
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanPassThroughAllocator.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanPoolAllocator.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanDevice.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanQueue.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanBuffer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanImage.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanCommandPool.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanCommandBuffer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanFrameBuffer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanSwapChainBuffer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanRenderPass.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanSemaphore.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanFence.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanShaderModule.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanBuffer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanBufferView.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanImage.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanImageView.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanDescriptorPool.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanDescriptorSetLayout.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanDescriptorSet.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanPipelineCache.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanPipelineLayout.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanGraphicsPipeline.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanQueryPool.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanInitializers.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanHelpers.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanDebug.hpp"

//#define PIPELINE_STATS

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanRenderer::VulkanRenderer()
	: Renderer()
	, mpDevice(nullptr)
	, mpRenderCompleteSemaphore(nullptr)
	, mpPresentCompleteSemaphore(nullptr)
	, mpCommandPool(nullptr)
	, mCurrentBufferIdx(0) 
	, mSubmitInfo{}
	, mpPipelineCache(nullptr)
{}

VulkanRenderer::VulkanRenderer(Platform::Window* pWindow)
	: Renderer(pWindow)
	, mpDevice(nullptr)
	, mpRenderCompleteSemaphore(nullptr)
	, mpPresentCompleteSemaphore(nullptr)
	, mpCommandPool(nullptr)
	, mCurrentBufferIdx(0)
	, mSubmitInfo{}
	, mpPipelineCache(nullptr)
{
	Init(pWindow);
}

VulkanRenderer::~VulkanRenderer()
{
	Terminate();
}

void VulkanRenderer::Init(Platform::Window* pWindow)
{
	mpDevice = GE_ALLOC(VulkanDevice)(pWindow, VULKAN_DEBUG);
	assert(mpDevice != nullptr);

	// NOTE! adjust view size as the surfa capabilities differ from the native window size
	auto surfCapabilities = mpDevice->GetSurfaceCapabilities();

	mWindowWidth = surfCapabilities.currentExtent.width;
	mWindowHeight = surfCapabilities.currentExtent.height;

	Prepare();
}

void VulkanRenderer::Terminate()
{
	assert(mpDevice != nullptr);

	// wait for the device to finish ongoing operations on all owned queues
	mpDevice->WaitIdle();


#ifdef _DEBUG
	LOG_INFO("[VulkanMemoryAllocator] AllocationCount: %u", mpDevice->GetAllocator()->GetAllocationCount());
#endif

	Renderer::Terminate();

#ifdef PIPELINE_STATS
	for (auto& it : mPipelineStatsMap)
	{
		auto& psBuff = it.second;

		GE_FREE(psBuff.pQueryPool);
		psBuff.pipelineStats.clear();
		psBuff.pipelineStatNames.clear();
	}
	mPipelineStatsMap.clear();
#endif

	for (auto& it : mVisualPassMap)
	{
		auto& rpBuff = it.second;

		GE_FREE(rpBuff.pRenderPass);
		for (auto& fb : rpBuff.frameBuffers)
		{
			GE_FREE(fb);
		}
	}
	mVisualPassMap.clear();

	// draw command buffers
	for (auto& commandBufferRef : mDrawCommandBuffers)
	{
		GE_FREE(commandBufferRef);
	}
	mDrawCommandBuffers.clear();

	GE_FREE(mpCommandPool);

	GE_FREE(mpPipelineCache);

	// syncronization 
	GE_FREE(mpRenderCompleteSemaphore);
	GE_FREE(mpPresentCompleteSemaphore);

	for (auto& fence : mWaitFences)
	{
		GE_FREE(fence);
	}
	mWaitFences.clear();

	GE_FREE(mpDevice);
}


void VulkanRenderer::Prepare()
{
	////////// GENERAL RESOURCES //////////

	// All these general resources should be managed by the renderer or the Device itself!

	SetupDrawCommandPool();

	SetupDrawCommandBuffers();

	SetupSynchronizationPrimitives();

	SetupPipelineCache();

	SetupSubmitInfo();

	mIsPrepared = true;
}

void VulkanRenderer::OnWindowResize(uint32_t width, uint32_t height)
{
	if (false == mIsPrepared)
		return;

	mIsPrepared = false;

	if (width > 0)
		mWindowWidth = width;

	if (height > 0)
		mWindowHeight = height;

	assert(mpDevice != nullptr);

	// Ensure all operations on the device have been finished before destroying resources
	mpDevice->WaitIdle();

	// Recreate swap chain
	mpDevice->ResetSwapChain();

	//TODO - re-init framebuffers !!!

	// Command buffers need to be recreated as they may store
	// references to the recreated frame buffer
	for (auto& commandBufferRef : mDrawCommandBuffers)
	{
		GE_FREE(commandBufferRef);
	}

	SetupDrawCommandBuffers();

	DrawSceneToCommandBuffer();

	mpDevice->WaitIdle();


	mIsPrepared = true;
}

void VulkanRenderer::SetupDrawCommandPool()
{
	assert(mpDevice != nullptr);

	auto pQueue = mpDevice->GetGraphicsQueue();
	assert(pQueue != nullptr);

	// we create a command pool for graphics commands
	mpCommandPool = GE_ALLOC(VulkanCommandPool)(mpDevice, pQueue->GetFamilyIndex());
	assert(mpCommandPool != nullptr);
}

void VulkanRenderer::SetupDrawCommandBuffers()
{
	assert(mpDevice != nullptr);
	assert(mpCommandPool != nullptr);

	// Create one command buffer for each swap chain image and reuse for rendering
	if (mDrawCommandBuffers.empty())
	{
		mDrawCommandBuffers.resize(mpDevice->GetSwapChainColorBufferCount());
	}

	for (auto& commandBufferRef : mDrawCommandBuffers)
	{
		commandBufferRef = GE_ALLOC(VulkanCommandBuffer)
		(
			mpDevice,
			mpCommandPool->GetHandle(),
			VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY
		);
		assert(commandBufferRef != nullptr);
	}
}

VulkanRenderPass* VulkanRenderer::SetupRenderPass(VisualPass* pVisualPass)
{
	assert(pVisualPass != nullptr);
	assert(mpDevice != nullptr);

	/*
		In a standard render pass we have the following:
		- 1 color attachement
		- 1 depth + stencil attachment
		- 1 subpass
	*/

	VulkanRenderPass* pRenderPass = nullptr;

	VkFormat colorFormat, depthFormat;
	switch (pVisualPass->GetPassType())
	{
		case VisualPass::PassType::GE_PT_STANDARD:
		{
			// NOTE! In this case the color and depth formats are the one used by the swapchain buffers as well !!!
			colorFormat = mpDevice->GetSurfaceFormat().format;
			depthFormat = mpDevice->GetDepthStencilFormat();
		} break;
		case VisualPass::PassType::GE_PT_OFFSCREEN:
		{
			auto* pColorRT = pVisualPass->GetRenderTarget(RenderTarget::TargetType::GE_TT_COLOR);
			if (pColorRT)
			{
				colorFormat = VulkanUtils::TextureFormatToVulkanFormat(pColorRT->GetTexture()->GetMetaData().format);
			}
			auto* pDepthRT = pVisualPass->GetRenderTarget(RenderTarget::TargetType::GE_TT_DEPTH_STENCIL);
			if (pDepthRT)
			{
				depthFormat = VulkanUtils::TextureFormatToVulkanFormat(pDepthRT->GetTexture()->GetMetaData().format);
			}
		} break;
		case VisualPass::PassType::GE_PT_SHADOWS:
		{
			auto* pDepthRT = pVisualPass->GetRenderTarget(RenderTarget::TargetType::GE_TT_DEPTH);
			if (pDepthRT)
			{
				depthFormat = VulkanUtils::TextureFormatToVulkanFormat(pDepthRT->GetTexture()->GetMetaData().format);
			}
		}
		break;
		case VisualPass::PassType::GE_PT_COUNT:
		default:
			LOG_ERROR("Invalid visual pass type!");
			return pRenderPass;
	}

	if (pVisualPass->GetPassType() == VisualPass::PassType::GE_PT_STANDARD ||
		pVisualPass->GetPassType() == VisualPass::PassType::GE_PT_OFFSCREEN)
	{
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = colorFormat;
		colorAttachment.samples = MIN_NUM_SAMPLES; // use at least 1 sample
		colorAttachment.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR; //clear color attachment on new frame
		colorAttachment.storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE; //store the color attachment in memory
		colorAttachment.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE; //color att, so we don't care avout the stencil op
		colorAttachment.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE; //color att, so we don't care avout the stencil op
		colorAttachment.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED; // we don't care of the previous layout the image was in
		// the image needs to be presented to the swapchain or read from the shader
		colorAttachment.finalLayout = (pVisualPass->GetPassType() == VisualPass::PassType::GE_PT_STANDARD ?
			VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		VkAttachmentDescription depthStencilAttachment{};
		depthStencilAttachment.format = depthFormat;
		depthStencilAttachment.samples = MIN_NUM_SAMPLES; // use at least 1 sample
		depthStencilAttachment.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR; //clear depth attachement on new frame
		depthStencilAttachment.storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE; //store the depth attachment on new frame
		depthStencilAttachment.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE; // we don't care about stencil, only depth
		depthStencilAttachment.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE; // we don't care about stencil, only depth
		depthStencilAttachment.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;  // we don't care of the previous layout the image was in
		depthStencilAttachment.finalLayout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; // we just need depth/stencil buffer without sampling it in shader

		VkAttachmentReference colorReference;
		colorReference.attachment = COLOR_ATT;
		colorReference.layout = VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthStencilReference;
		depthStencilReference.attachment = DEPTH_ATT;
		depthStencilReference.layout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		// subPasses
		VkSubpassDescription subPass{};
		subPass.pipelineBindPoint = VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS;
		subPass.colorAttachmentCount = 1;
		subPass.pColorAttachments = &colorReference;
		subPass.pDepthStencilAttachment = &depthStencilReference;

		VkSubpassDependency subPassDep_1;
		subPassDep_1.srcSubpass = VK_SUBPASS_EXTERNAL;
		subPassDep_1.dstSubpass = SUBPASS_ID;

		VkSubpassDependency subPassDep_2;
		subPassDep_2.srcSubpass = SUBPASS_ID;
		subPassDep_2.dstSubpass = VK_SUBPASS_EXTERNAL;

		if (pVisualPass->GetPassType() == VisualPass::PassType::GE_PT_STANDARD)
		{
			subPassDep_1.srcStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			subPassDep_1.dstStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subPassDep_1.srcAccessMask = VkAccessFlagBits::VK_ACCESS_MEMORY_READ_BIT;
			subPassDep_1.dstAccessMask = VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			subPassDep_1.dependencyFlags = VkDependencyFlagBits::VK_DEPENDENCY_BY_REGION_BIT;

			subPassDep_2.srcStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subPassDep_2.dstStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			subPassDep_2.srcAccessMask = VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			subPassDep_2.dstAccessMask = VkAccessFlagBits::VK_ACCESS_MEMORY_READ_BIT;
			subPassDep_2.dependencyFlags = VkDependencyFlagBits::VK_DEPENDENCY_BY_REGION_BIT;
		}
		else  // offscreen
		{
			subPassDep_1.srcStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			subPassDep_1.dstStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subPassDep_1.srcAccessMask = VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT;
			subPassDep_1.dstAccessMask = VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			subPassDep_1.dependencyFlags = VkDependencyFlagBits::VK_DEPENDENCY_BY_REGION_BIT;

			subPassDep_2.srcStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subPassDep_2.dstStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			subPassDep_2.srcAccessMask = VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			subPassDep_2.dstAccessMask = VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT;
			subPassDep_2.dependencyFlags = VkDependencyFlagBits::VK_DEPENDENCY_BY_REGION_BIT;
		}

		pRenderPass = GE_ALLOC(VulkanRenderPass)(mpDevice, { colorAttachment, depthStencilAttachment }, { subPass }, { subPassDep_1, subPassDep_2 });
	}
	else if (pVisualPass->GetPassType() == VisualPass::PassType::GE_PT_SHADOWS)
	{
		VkAttachmentDescription depthStencilAttachment{};
		depthStencilAttachment.format = depthFormat;
		depthStencilAttachment.samples = MIN_NUM_SAMPLES; // use at least 1 sample
		depthStencilAttachment.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR; //clear depth attachement on new frame
		depthStencilAttachment.storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE; //store the depth attachment on new frame
		depthStencilAttachment.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE; // we don't care about stencil, only depth
		depthStencilAttachment.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE; // we don't care about stencil, only depth
		depthStencilAttachment.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;  // we don't care of the previous layout the image was in
		depthStencilAttachment.finalLayout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL; // Attachment will be transitioned to shader read at render pass end

		VkAttachmentReference depthStencilReference;
		depthStencilReference.attachment = 0; //TODO
		depthStencilReference.layout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		// subPasses
		VkSubpassDescription subPass{};
		subPass.pipelineBindPoint = VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS;
		subPass.colorAttachmentCount = 0; // no color attachmnent, only depth
		subPass.pColorAttachments = nullptr;
		subPass.pDepthStencilAttachment = &depthStencilReference;

		VkSubpassDependency subPassDep_1;
		subPassDep_1.srcSubpass = VK_SUBPASS_EXTERNAL;
		subPassDep_1.dstSubpass = SUBPASS_ID;

		subPassDep_1.srcStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		subPassDep_1.dstStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		subPassDep_1.srcAccessMask = VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT;
		subPassDep_1.dstAccessMask = VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		subPassDep_1.dependencyFlags = VkDependencyFlagBits::VK_DEPENDENCY_BY_REGION_BIT;

		VkSubpassDependency subPassDep_2;
		subPassDep_2.srcSubpass = SUBPASS_ID;
		subPassDep_2.dstSubpass = VK_SUBPASS_EXTERNAL;

		subPassDep_2.srcStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		subPassDep_2.dstStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		subPassDep_2.srcAccessMask = VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		subPassDep_2.dstAccessMask = VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT;
		subPassDep_2.dependencyFlags = VkDependencyFlagBits::VK_DEPENDENCY_BY_REGION_BIT;

		pRenderPass = GE_ALLOC(VulkanRenderPass)(mpDevice, { depthStencilAttachment }, { subPass }, { subPassDep_1, subPassDep_2 });
	}

	assert(pRenderPass != nullptr);

	return pRenderPass;
}

void VulkanRenderer::SetupFrameBuffers(VisualPass* pVisualPass, VulkanRenderPass* pRenderPass, std::vector<VulkanFrameBuffer*>& frameBuffersOut,
	VisualPassBeginData& visualPassBeginDataOut)
{
	assert(pVisualPass != nullptr);
	assert(pRenderPass != nullptr);
	assert(mpDevice != nullptr);

	frameBuffersOut.clear();

	// prepare color + depth image views as attachments for the framebuffer
	std::vector<VkImageView> frameBufferAttachments;
	VkExtent2D extent;

	//NOTE! Currently Depth attachment is supported by default

	switch (pVisualPass->GetPassType())
	{
		case VisualPass::PassType::GE_PT_STANDARD:
		{
			assert(GetWindowWidth() > 0);
			assert(GetWindowHeight() > 0);

			// color
			auto& colorBuffers = mpDevice->GetSwapChainColorBuffers();
			assert(colorBuffers.empty() == false);

			// depth stencil
			auto* pDepthStencilBuffer = mpDevice->GetSwapChainDepthStencilBuffer();
			assert(pDepthStencilBuffer != nullptr);

			// Create one frame buffer for each swap chain image and reuse for rendering
			frameBuffersOut.resize(colorBuffers.size());
			frameBufferAttachments.resize(ATT_COUNT * frameBuffersOut.size());

			for (uint32_t i = 0; i < frameBuffersOut.size(); ++i)
			{
				uint32_t idx = i * ATT_COUNT;
				frameBufferAttachments[idx + COLOR_ATT] = colorBuffers[i]->GetImageViewHandle();
				frameBufferAttachments[idx + DEPTH_ATT] = pDepthStencilBuffer->GetImageViewHandle();;
			}

			extent.width = GetWindowWidth();
			extent.height = GetWindowHeight();

			visualPassBeginDataOut.width = extent.width;
			visualPassBeginDataOut.height = extent.height;
			visualPassBeginDataOut.depth = 1.0f;
			visualPassBeginDataOut.stencil = 0;
			//TODO - clear color

		assert(frameBuffersOut.size() == frameBufferAttachments.size() / ATT_COUNT);

		for (uint32_t i = 0; i < frameBuffersOut.size(); ++i)
		{
			uint32_t idx = i * ATT_COUNT;

			frameBuffersOut[i] = GE_ALLOC(VulkanFrameBuffer)
				(
					mpDevice,
					pRenderPass,
					{ frameBufferAttachments[idx + COLOR_ATT], frameBufferAttachments[idx + DEPTH_ATT] },
					extent.width,
					extent.height
					);
			assert(frameBuffersOut[i] != nullptr);
		}

		} break;
		case VisualPass::PassType::GE_PT_OFFSCREEN:
		{
			auto& renderTargets = pVisualPass->GetRenderTargets();

			assert(renderTargets.size() == ATT_COUNT);

			auto colorIt = renderTargets.find(RenderTarget::TargetType::GE_TT_COLOR);
			assert(colorIt != renderTargets.end());
			auto* pColorRT = Get(colorIt->second);
			assert(pColorRT != nullptr);

			auto depthIt = renderTargets.find(RenderTarget::TargetType::GE_TT_DEPTH_STENCIL);
			assert(depthIt != renderTargets.end());
			auto* pDepthRT = Get(depthIt->second);
			assert(pDepthRT != nullptr);

			frameBuffersOut.resize(1);

			// prepare color + depth image views as attachments for the framebuffer
			frameBufferAttachments.resize(ATT_COUNT);
			frameBufferAttachments[COLOR_ATT] = pColorRT->GetVkImageView()->GetHandle();
			frameBufferAttachments[DEPTH_ATT] = pDepthRT->GetVkImageView()->GetHandle();

			// get RT width and height for the framebuffer size
			const auto& extent3D = pColorRT->GetVkImage()->GetData().extent;

			assert(extent3D.width > 0);
			assert(extent3D.height > 0);

			extent.width = extent3D.width;
			extent.height = extent3D.height;

			visualPassBeginDataOut.width = extent.width;
			visualPassBeginDataOut.height = extent.height;
			visualPassBeginDataOut.depth = 1.0f;
			visualPassBeginDataOut.stencil = 0;
			//TODO - clear color

			assert(frameBuffersOut.size() == frameBufferAttachments.size() / ATT_COUNT);

			for (uint32_t i = 0; i < frameBuffersOut.size(); ++i)
			{
				frameBuffersOut[i] = GE_ALLOC(VulkanFrameBuffer)
					(
						mpDevice,
						pRenderPass,
						frameBufferAttachments,
						extent.width,
						extent.height
						);
				assert(frameBuffersOut[i] != nullptr);
			}

		} break;
		case VisualPass::PassType::GE_PT_SHADOWS:
		{
			auto& renderTargets = pVisualPass->GetRenderTargets();

			assert(renderTargets.size() == 1);

			auto depthIt = renderTargets.find(RenderTarget::TargetType::GE_TT_DEPTH);
			assert(depthIt != renderTargets.end());
			auto* pDepthRT = Get(depthIt->second);
			assert(pDepthRT != nullptr);

			frameBuffersOut.resize(1);

			// prepare depth image view as attachment for the framebuffer
			frameBufferAttachments.resize(1);
			frameBufferAttachments[0] = pDepthRT->GetVkImageView()->GetHandle();

			// get RT width and height for the framebuffer size
			const auto& extent3D = pDepthRT->GetVkImage()->GetData().extent;

			assert(extent3D.width > 0);
			assert(extent3D.height > 0);

			extent.width = extent3D.width;
			extent.height = extent3D.height;

			visualPassBeginDataOut.width = extent.width;
			visualPassBeginDataOut.height = extent.height;
			visualPassBeginDataOut.depth = 1.0f;
			visualPassBeginDataOut.stencil = 0;
			//TODO - clear color

			assert(frameBuffersOut.size() == frameBufferAttachments.size());

			for (uint32_t i = 0; i < frameBuffersOut.size(); ++i)
			{
				frameBuffersOut[i] = GE_ALLOC(VulkanFrameBuffer)
					(
						mpDevice,
						pRenderPass,
						frameBufferAttachments,
						extent.width,
						extent.height
						);
				assert(frameBuffersOut[i] != nullptr);
			}
		} break;
		case VisualPass::PassType::GE_PT_COUNT:
		default:
			LOG_ERROR("Invalid visual pass type!");
			return;
	}
}

void VulkanRenderer::SetupSynchronizationPrimitives()
{
	assert(mpDevice != nullptr);

	// Semaphores (Used for correct command ordering)
	//// Semaphore used to ensures that all commands submitted have been finished before submitting the image to the queue
	mpRenderCompleteSemaphore = GE_ALLOC(VulkanSemaphore)(mpDevice);
	assert(mpRenderCompleteSemaphore != nullptr);
	//// Semaphore used to ensures that image presentation is complete before starting to submit again
	mpPresentCompleteSemaphore = GE_ALLOC(VulkanSemaphore)(mpDevice);
	assert(mpPresentCompleteSemaphore != nullptr);

	// Fences (Used to check draw command buffer completion)
	// Create in signaled state so we don't wait on first render of each command buffer
	mWaitFences.resize(mDrawCommandBuffers.size());
	for (auto& fence : mWaitFences)
	{
		fence = GE_ALLOC(VulkanFence)(mpDevice, VkFenceCreateFlagBits::VK_FENCE_CREATE_SIGNALED_BIT);
		assert(fence != nullptr);
	}
}

void VulkanRenderer::SetupPipelineCache()
{
	assert(mpDevice != nullptr);

	mpPipelineCache = GE_ALLOC(VulkanPipelineCache)(mpDevice, {});
	assert(mpPipelineCache != nullptr);
}

void VulkanRenderer::SetupSubmitInfo()
{
	assert(mDrawCommandBuffers.size() > 0);
	assert(mCurrentBufferIdx < mDrawCommandBuffers.size());
	assert(mpPresentCompleteSemaphore != nullptr);
	assert(mpRenderCompleteSemaphore != nullptr);

	auto pCrrDrawCommandBuffer = mDrawCommandBuffers[mCurrentBufferIdx];
	assert(pCrrDrawCommandBuffer != nullptr);

	/** @brief Pipeline stages used to wait at for graphics queue submissions */
	mSubmitPipelineStages = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	// The submit info structure specifices a command buffer queue submission batch
	mSubmitInfo = VulkanInitializers::SubmitInfo
	(
		1, &pCrrDrawCommandBuffer->GetHandle(),
		1, &mpPresentCompleteSemaphore->GetHandle(),
		&mSubmitPipelineStages,
		1, &mpRenderCompleteSemaphore->GetHandle()
	);
}

void VulkanRenderer::SetupPipelineStats()
{
#ifdef PIPELINE_STATS
	if (! mPipelineStatsMap.empty())
		return;

	assert(mpDevice != nullptr);
	assert(false == mVisualPassMap.empty());

	PipelineStatsData pipelineStatsData;
	pipelineStatsData.pipelineStatNames = {
			"Input assembly vertex count        ",
			"Input assembly primitives count    ",
			"Vertex shader invocations          ",
			"Clipping stage primitives processed",
			"Clipping stage primtives output    ",
			"Fragment shader invocations        "
	};

	pipelineStatsData.pipelineStats.resize(pipelineStatsData.pipelineStatNames.size());

	VkQueryPipelineStatisticFlags pipelineStatsFlags =
		VkQueryPipelineStatisticFlagBits::VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_VERTICES_BIT |
		VkQueryPipelineStatisticFlagBits::VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_PRIMITIVES_BIT |
		VkQueryPipelineStatisticFlagBits::VK_QUERY_PIPELINE_STATISTIC_VERTEX_SHADER_INVOCATIONS_BIT |
		VkQueryPipelineStatisticFlagBits::VK_QUERY_PIPELINE_STATISTIC_CLIPPING_INVOCATIONS_BIT |
		VkQueryPipelineStatisticFlagBits::VK_QUERY_PIPELINE_STATISTIC_CLIPPING_PRIMITIVES_BIT |
		VkQueryPipelineStatisticFlagBits::VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT;

	for (auto& it : mVisualPassMap)
	{
		auto& passType = it.first;

		pipelineStatsData.pQueryPool = GE_ALLOC(VulkanQueryPool)
			(
				mpDevice,
				VkQueryType::VK_QUERY_TYPE_PIPELINE_STATISTICS,
				static_cast<uint32_t>(pipelineStatsData.pipelineStats.size()),
				pipelineStatsFlags
				);
		assert(pipelineStatsData.pQueryPool != nullptr);

		mPipelineStatsMap.emplace(passType, pipelineStatsData);
	}


#endif
}

// Retrieves the results of the pipeline statistics query submitted to the command buffer
void VulkanRenderer::GetQueryResults()
{
#ifdef PIPELINE_STATS
	assert(false == mPipelineStatsMap.empty());

	for (auto& it : mPipelineStatsMap)
	{
		auto& passType = it.first;
		auto& queryData = it.second;
		
		if (queryData.pQueryPool)
		{
			uint32_t count = static_cast<uint32_t>(queryData.pipelineStats.size());
			VkQueryResultFlags flags = VkQueryResultFlagBits::VK_QUERY_RESULT_64_BIT;// | VkQueryResultFlagBits::VK_QUERY_RESULT_WAIT_BIT;

			queryData.pQueryPool->GetQueryResults(1, queryData.pipelineStats.data(), count * sizeof(uint64_t), sizeof(uint64_t), flags);

			// log query stats
			std::string passTypeStr = (passType == VisualPass::PassType::GE_PT_SHADOWS ? "SHADOW" : (passType == VisualPass::PassType::GE_PT_OFFSCREEN ? "OFFSCREEN" : "STANDARD"));
			LOG_INFO("Render pass: %s", passTypeStr.c_str());
			LOG_INFO("####### Pipeline Stats ########");
			for (uint32_t i = 0; i < queryData.pipelineStats.size(); ++i)
			{
				LOG_INFO("%s : %lu", queryData.pipelineStatNames[i].c_str(), queryData.pipelineStats[i]);
			}
			LOG_INFO("");
		}
	}
#endif
}

void VulkanRenderer::ResetQuery(VisualPass::PassType passType, uint32_t currentBufferIdx)
{
#ifdef PIPELINE_STATS
	assert(false == mPipelineStatsMap.empty());
	assert(currentBufferIdx < mDrawCommandBuffers.size());
	
	auto it = mPipelineStatsMap.find(passType);
	if (it != mPipelineStatsMap.end())
	{
		auto* pQueryPool = it->second.pQueryPool;
		if (pQueryPool)
		{
			pQueryPool->ResetQuery(mDrawCommandBuffers[currentBufferIdx]->GetHandle(), static_cast<uint32_t>(it->second.pipelineStats.size()));
		}
	}
#endif
}

void VulkanRenderer::BeginQuery(VisualPass::PassType passType, uint32_t currentBufferIdx)
{
#ifdef PIPELINE_STATS
	// before every use and in between uses the query pool must be reset
	ResetQuery(passType, currentBufferIdx);

	assert(false == mPipelineStatsMap.empty());
	assert(currentBufferIdx < mDrawCommandBuffers.size());

	auto pCrrDrawCommandBuffer = mDrawCommandBuffers[currentBufferIdx];
	assert(pCrrDrawCommandBuffer != nullptr);

	auto it = mPipelineStatsMap.find(passType);
	if (it != mPipelineStatsMap.end())
	{
		auto* pQueryPool = it->second.pQueryPool;
		if (pQueryPool)
		{
			pQueryPool->BeginQuery(pCrrDrawCommandBuffer->GetHandle());
		}
	}
#endif
}

void VulkanRenderer::EndQuery(VisualPass::PassType passType, uint32_t currentBufferIdx)
{
#ifdef PIPELINE_STATS
	assert(false == mPipelineStatsMap.empty());
	assert(currentBufferIdx < mDrawCommandBuffers.size());

	auto pCrrDrawCommandBuffer = mDrawCommandBuffers[currentBufferIdx];
	assert(pCrrDrawCommandBuffer != nullptr);

	auto it = mPipelineStatsMap.find(passType);
	if (it != mPipelineStatsMap.end())
	{
		auto* pQueryPool = it->second.pQueryPool;
		if (pQueryPool)
		{
			pQueryPool->EndQuery(pCrrDrawCommandBuffer->GetHandle());
		}
	}
 #endif
}

void VulkanRenderer::RenderFrame(RenderQueue* pRenderQueue)
{
	if (false == mIsPrepared)
		return;

	assert(pRenderQueue != nullptr);

	BeginFrame();

	DrawSceneToCommandBuffer();

	EndFrame();
}

void VulkanRenderer::UpdateFrame(Camera* pCamera, float32_t crrTime)
{
	if (false == mIsPrepared)
		return;

	assert(mpRenderQueue != nullptr);

	assert(pCamera != nullptr);
	mpCamera = pCamera;

	UpdateNodes(pCamera, crrTime);
}

void VulkanRenderer::BeginFrame()
{
	//TODO
}

void VulkanRenderer::EndFrame()
{
	// TODO
}

void VulkanRenderer::SubmitFrame()
{
	// HERE THE FRAME IS JUST SUBMITED TO THE GRAPHICS QUEUE
	// THE IMAGE IS PRESENTED TO THE SWAPCHAIN
	// THE ACTUAL RENDERING IS DONE WHEN THE COMMAND BUFFER IS BUILT/ALL COMMANDS RECORDED

	if (false == mIsPrepared)
		return;

	assert(mpDevice != nullptr);

	// Get next image in the swap chain (back/front buffer)
	// updates mCurrentBuffer
	PrepareFrame();

	// we do the checks after we acquire the new mCurrentBufferIdx of the current swapchain image
	// done in PrepareFrame()

	auto pCrrDrawCommandBuffer = mDrawCommandBuffers[mCurrentBufferIdx];
	assert(pCrrDrawCommandBuffer != nullptr);
	auto pCrrWaitFence = mWaitFences[mCurrentBufferIdx];
	assert(pCrrWaitFence != nullptr);

	auto pQueue = mpDevice->GetGraphicsQueue();
	assert(pQueue != nullptr);

	// Use a fence to wait until the command buffer has finished execution before using it again
	VK_CHECK_RESULT(pCrrWaitFence->WaitIdle(VK_TRUE, UINT64_MAX));
	VK_CHECK_RESULT(pCrrWaitFence->Reset());

	// account for the new value of mCurrentBufferIdx
	mSubmitInfo.pCommandBuffers = &pCrrDrawCommandBuffer->GetHandle();

	// Submit to the graphics queue
	VK_CHECK_RESULT(pQueue->Submit(1, &mSubmitInfo, pCrrWaitFence->GetHandle()));

	// Present the current buffer to the swap chain
	// Pass the semaphore signaled by the command buffer submission from the submit info as the wait semaphore for swap chain presentation
	// This ensures that the image is not presented to the windowing system until all commands have been submitted
	PresentFrame();
}

void VulkanRenderer::PrepareFrame()
{
	assert(mpDevice != nullptr);
	assert(mpPresentCompleteSemaphore != nullptr);

	// Acquire the next image from the swap chain - we get the next image index
	VkResult res = mpDevice->AcquireNextImage(&mCurrentBufferIdx, mpPresentCompleteSemaphore->GetHandle());

	// Recreate the swapchain if it's no longer compatible with the surface (OUT_OF_DATE) or no longer optimal for presentation (SUBOPTIMAL)
	if ((res == VkResult::VK_ERROR_OUT_OF_DATE_KHR) || (res == VkResult::VK_SUBOPTIMAL_KHR))
	{
		OnWindowResize();
	}
	else
	{
		VK_CHECK_RESULT(res);
	}
}

void VulkanRenderer::PresentFrame()
{
	assert(mpDevice != nullptr);
	assert(mpRenderCompleteSemaphore != nullptr);

	// Return the image to the swap chain for presentation
	VkPresentInfoKHR presentInfo = 
		VulkanInitializers::PresentInfo(1, &mpDevice->GetSwapChainHandle(), 1, &mpRenderCompleteSemaphore->GetHandle(), &mCurrentBufferIdx);

	auto pQueue = mpDevice->GetGraphicsQueue();
	assert(pQueue != nullptr);

	VkResult res = VkResult::VK_SUCCESS;
	if (mpDevice->IsPresentQueueSupported()) // separate present queue
	{
		pQueue = mpDevice->GetPresentQueue();
		assert(pQueue != nullptr);

		res = pQueue->Present(presentInfo);
	}
	else // graphics and present queue are the same
	{
		res = pQueue->Present(presentInfo);
	}
	if (false == ((res == VkResult::VK_SUCCESS) || (res == VkResult::VK_SUBOPTIMAL_KHR)))
	{
		if (res == VkResult::VK_ERROR_OUT_OF_DATE_KHR)
		{
			// Swap chain is no longer compatible with the surface and needs to be recreated
			OnWindowResize();
			return;
		}
		else
		{
			VK_CHECK_RESULT(res);
		}
	}
	// wait until queue is idle
	VK_CHECK_RESULT(pQueue->WaitIdle());
}

void VulkanRenderer::ComputeGraphicsResources(RenderQueue* pRenderQueue)
{
	if (false == mIsPrepared)
		return;

	assert(pRenderQueue != nullptr);

	mpRenderQueue = pRenderQueue;

	// TODO - compute resources for all renderable types
	auto& renderableList = mpRenderQueue->GetRenderables(RenderQueue::RenderableType::GE_RT_OPAQUE);

	mpRenderQueue->ForEach(renderableList,
		[&, this](const RenderQueue::Renderable* pRenderable)
		{
			assert(pRenderable != nullptr);

			auto* pGeoNode = pRenderable->pGeometryNode;
			assert(pGeoNode != nullptr);

			auto* pVisComp = pGeoNode->GetComponent<VisualComponent>();
			assert(pVisComp != nullptr);
			auto* pVisEffect = pVisComp->GetVisualEffect();
			assert(pVisEffect != nullptr);

			// we didn't call this earlier as we needed to have
			// the node info first
			pVisEffect->Init();

			// TODO - Compute mVisualPasses list of passes in their proper order
			const auto& passMap = pVisEffect->GetPasses();
			for (auto& it : passMap)
			{
				auto& passVector = it.second;
				for (auto* pPass : passVector)
				{
					AddVisualPass(pPass);
				}
			}

			pVisEffect->InitPasses(this);
		}
	);

	SetupPipelineStats();
}

void VulkanRenderer::UpdateDynamicStates(VisualPass* pVisualPass, uint32_t currentBufferIdx)
{
	assert(pVisualPass != nullptr);

	// update dynamic states if any
	const auto& dynamicState = pVisualPass->GetDynamicState();

	if (false == dynamicState.HasStates())
	{
		LOG_INFO("No dynamic states!");
		return;
	}

	assert(pVisualPass->GetRenderData().width > 0);
	assert(pVisualPass->GetRenderData().height > 0);

	assert(currentBufferIdx < mDrawCommandBuffers.size());
	auto pCrrDrawCommandBuffer = mDrawCommandBuffers[currentBufferIdx];
	assert(pCrrDrawCommandBuffer != nullptr);

	auto dynStates = dynamicState.GetStates();
	std::vector<VkDynamicState> dynamicStateEnables;
	dynamicStateEnables.resize(dynStates.size());

	for (size_t i = 0; i < dynamicStateEnables.size(); ++i)
	{
		dynamicStateEnables[i] = VulkanUtils::DynamicStateToVulkanDynamicState(dynStates[i]);
	}

	for (auto& state : dynamicStateEnables)
	{
		switch (state)
		{
		case VkDynamicState::VK_DYNAMIC_STATE_VIEWPORT:
		{
			VkViewport viewport{};
			viewport.x = 0;
			viewport.y = 0;
			viewport.width = static_cast<float32_t>(pVisualPass->GetRenderData().width);
			viewport.height = static_cast<float32_t>(pVisualPass->GetRenderData().height);
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			vkCmdSetViewport(pCrrDrawCommandBuffer->GetHandle(), 0, 1, &viewport);
		}
		break;
		case VkDynamicState::VK_DYNAMIC_STATE_SCISSOR:
		{
			VkExtent2D extent { pVisualPass->GetRenderData().width, pVisualPass->GetRenderData().width };

			VkRect2D scissor{};
			scissor.offset = VkOffset2D{ 0, 0 };
			scissor.extent = extent;

			vkCmdSetScissor(pCrrDrawCommandBuffer->GetHandle(), 0, 1, &scissor);
		}
		break;
		case VkDynamicState::VK_DYNAMIC_STATE_DEPTH_BIAS:
		{
			// Set depth bias (aka "Polygon offset")
			// Required to avoid shadow mapping artifacts
			vkCmdSetDepthBias(pCrrDrawCommandBuffer->GetHandle(), DEPTH_BIAS_CONSTANT, DEPTH_CLAMP, DEPTH_BIAS_SLOPE);
		} break;

		//TODO other cases

		default:
			LOG_ERROR("Invalid dynamic state!");
		}
	}
}

void VulkanRenderer::UpdateUniformBuffers(VisualPass* pVisualPass, GeometryNode* pGeoNode, Camera* pCamera, float32_t crrTime)
{
	assert(pVisualPass != nullptr);
	assert(pGeoNode != nullptr);
	assert(pCamera != nullptr);

	auto shaders = pVisualPass->GetShaders();
	for (auto iter = shaders.begin(); iter != shaders.end(); ++iter)
	{
		auto shaderStage = iter->first;
		auto uniformBuffer = pVisualPass->GetUniformBuffer(shaderStage);

		if (uniformBuffer)
		{
			const auto& uniforms = uniformBuffer->GetUniforms();

			for (const auto& uni : uniforms)
			{
				const auto& uniformType = uni.first;

				switch(uniformType)
				{
					case GLSLShaderTypes::UniformType::GE_UT_PVM_MATRIX4:
					{
						uniformBuffer->SetUniform(GLSLShaderTypes::UniformType::GE_UT_PVM_MATRIX4,
							pCamera->GetProjectionViewMatrix() * pVisualPass->GetTransform() * pGeoNode->GetModelMatrix());
					} break;
					case GLSLShaderTypes::UniformType::GE_UT_PV_CUBEMAP_MATRIX4:
					{
						glm::mat4 PV = pCamera->GetProjectionViewMatrix();

						// we remove the translation transform to allow the cubemap to never change position in world space
						PV[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

						uniformBuffer->SetUniform(GLSLShaderTypes::UniformType::GE_UT_PVM_MATRIX4, PV * pGeoNode->GetModelMatrix());
					} break;
					case GLSLShaderTypes::UniformType::GE_UT_MODEL_MATRIX4:
					{
						uniformBuffer->SetUniform(GLSLShaderTypes::UniformType::GE_UT_MODEL_MATRIX4, pVisualPass->GetTransform() * pGeoNode->GetModelMatrix());
					} break;
					case GLSLShaderTypes::UniformType::GE_UT_NORMAL_MATRIX4:
					{
						uniformBuffer->SetUniform(GLSLShaderTypes::UniformType::GE_UT_NORMAL_MATRIX4, pVisualPass->GetTransform() * pGeoNode->GetNormalMatrix());
					} break;
					case GLSLShaderTypes::UniformType::GE_UT_CAMERA_POS:
					{
						uniformBuffer->SetUniform(GLSLShaderTypes::UniformType::GE_UT_CAMERA_POS, glm::vec4(pCamera->GetPosition(), 0.0f));
					} break;
					case GLSLShaderTypes::UniformType::GE_UT_PROJECTION_MATRIX4:
					case GLSLShaderTypes::UniformType::GE_UT_VIEW_MATRIX4:
					case GLSLShaderTypes::UniformType::GE_UT_COLOR_VEC4:
					case GLSLShaderTypes::UniformType::GE_UT_LIGHT_DIR:
					case GLSLShaderTypes::UniformType::GE_UT_LIGHT_COLOR:
					case GLSLShaderTypes::UniformType::GE_UT_CRR_TIME:
					// TODO
						break;
					case GLSLShaderTypes::UniformType::GE_UT_COUNT:
					default:
						//LOG_ERROR("Invalid uniform type! Failed to update the uniform!");
						break;
				}
			}

			Bind(uniformBuffer);
		}
	}
}

void VulkanRenderer::BindLight(VisualPass* pVisualPass, const LightNode* pLightNode, GeometryNode* pGeoNode)
{
	assert(pVisualPass != nullptr);
	assert(pLightNode != nullptr);
	assert(pGeoNode != nullptr);

	if (false == pGeoNode->IsLit())
	{
		LOG_INFO("No light to bind for this node as it is unlit!");
		return;
	}

	auto* pLight = pLightNode->GetLight();
	assert(pLight != nullptr);

	switch (pVisualPass->GetPassType())
	{
		case VisualPass::PassType::GE_PT_STANDARD:
		case VisualPass::PassType::GE_PT_OFFSCREEN:
		{
			auto* pVertUBO = pVisualPass->GetUniformBuffer(Shader::ShaderStage::GE_SS_VERTEX);
			auto* pFragUBO = pVisualPass->GetUniformBuffer(Shader::ShaderStage::GE_SS_FRAGMENT);

			switch (pLight->GetLightType())
			{
			case Light::LightType::GE_LT_DIRECTIONAL:
			{
				if (pFragUBO)
				{
					pFragUBO->SetUniform(GLSLShaderTypes::UniformType::GE_UT_LIGHT_DIR, glm::vec4(pLight->GetDirection(), 0.0f));
					pFragUBO->SetUniform(GLSLShaderTypes::UniformType::GE_UT_LIGHT_COLOR, glm::vec4(pLight->GetColor(), 1.0f));
				}
			}
			break;
			case Light::LightType::GE_LT_POINT:
			{
				if (pGeoNode->IsPassAllowed(VisualPass::PassType::GE_PT_SHADOWS))
				{
					if (pVertUBO)
					{
						//NOTE! For the Vulkan renderer we also need a biasMat
						// The biasMat  is not specific to shadow mapping,
						// it just makes it so that the resulting projection considers 
						// the particularities of how the Vulkan coordinate system is defined 
						// (Y axis is inversed, Z range is halved).
						const glm::mat4 biasMat = glm::mat4(
							0.5, 0.0, 0.0, 0.0,
							0.0, 0.5, 0.0, 0.0,
							0.0, 0.0, 1.0, 0.0,
							0.5, 0.5, 0.0, 1.0
						);

						pVertUBO->SetUniform(GLSLShaderTypes::UniformType::GE_UT_LIGHT_PVM_MATRIX4, biasMat * pLight->GetLightPVM());
					}
				}

				if (pFragUBO)
				{
					pFragUBO->SetUniform(GLSLShaderTypes::UniformType::GE_UT_LIGHT_POS, glm::vec4(pLight->GetPosition(), 1.0f));
					pFragUBO->SetUniform(GLSLShaderTypes::UniformType::GE_UT_LIGHT_COLOR, glm::vec4(pLight->GetColor(), 1.0f));
				}
			} break;
			case Light::LightType::GE_LT_SPOT:
				//TODO
				break;
			case Light::LightType::GE_PT_COUNT:
			default:
				LOG_ERROR("Invalid light type!");
			}
		} break;
		case VisualPass::PassType::GE_PT_SHADOWS:
		{
			auto* pUBO = pVisualPass->GetUniformBuffer(Shader::ShaderStage::GE_SS_VERTEX);

			switch (pLight->GetLightType())
			{
			case Light::LightType::GE_LT_POINT:
			{
				if (pUBO)
					pUBO->SetUniform(GLSLShaderTypes::UniformType::GE_UT_LIGHT_PVM_MATRIX4, pLight->GetLightPVM());
			} break;
			case Light::LightType::GE_LT_DIRECTIONAL:
			case Light::LightType::GE_LT_SPOT:
				//TODO
				break;
			case Light::LightType::GE_PT_COUNT:
			default:
				LOG_ERROR("Invalid light type!");
			}
		} break;
	}
}

void VulkanRenderer::DrawSceneToCommandBuffer()
{
	assert(mpRenderQueue != nullptr);

	// THE ACTUAL RENDERING IS DONE HERE !!!!

	for (uint32_t i = 0; i < mDrawCommandBuffers.size(); ++i)
	{
		assert(mDrawCommandBuffers[i] != nullptr);

		// Begin command buffer recording
		VK_CHECK_RESULT(mDrawCommandBuffers[i]->Begin());

		DrawNodes(i);

		// End command buffer recording
		VK_CHECK_RESULT(mDrawCommandBuffers[i]->End());
	}
}

void VulkanRenderer::DrawNodes(uint32_t currentBufferIdx)
{
	assert(currentBufferIdx < mDrawCommandBuffers.size());

	for (auto& it : mVisualPassMap)
	{
		auto& passType = it.first;
		auto& passData = it.second;

		//
		BeginQuery(passType, currentBufferIdx);
		//

		BeginRenderPass(passData, currentBufferIdx);

		for (auto* pPass : passData.passes)
		{
			if (pPass)
			{
				pPass->RenderNode(currentBufferIdx);
			}
		}

		EndRenderPass(passData, currentBufferIdx);

		//
		EndQuery(passType, currentBufferIdx);
		//
	}
}


void VulkanRenderer::UpdateNodes(Camera* pCamera, float32_t crrTime)
{
	assert(pCamera != nullptr);

	for (auto& it : mVisualPassMap)
	{
		auto& passType = it.first;
		auto& passData = it.second;

		for (auto* pPass : passData.passes)
		{
			if (pPass)
			{
				pPass->UpdateNode(pCamera, crrTime);
			}
		}
	}

	GetQueryResults();
}

void VulkanRenderer::DrawNode(VisualPass* pVisualPass, GeometryNode* pGeoNode, uint32_t currentBufferIdx)
{
	assert(pVisualPass != nullptr);
	assert(pGeoNode != nullptr);
	assert(currentBufferIdx < mDrawCommandBuffers.size());

	UpdateDynamicStates(pVisualPass, currentBufferIdx);

	// debug case
	if (pVisualPass->GetIsDebug())
	{
		DrawDirect(3, 0, 1, currentBufferIdx);
		return;
	}

	// draw geometric primitives
	auto* pGeometry = pGeoNode->GetGeometry();
	assert(pGeometry != nullptr);

	// bind vertex buffer
	auto* pVertexBuffer = pGeometry->GetVertexBuffer();
	assert(pVertexBuffer != nullptr);

	Bind(pVertexBuffer, currentBufferIdx);

	// bind index buffer (if available)
	bool isIndexedDrawing = pGeometry->IsIndexed();
	auto* pIndexBuffer = pGeometry->GetIndexBuffer();
	if (nullptr == pIndexBuffer)
	{
		isIndexedDrawing = false; // failsafe
	}

	if (isIndexedDrawing)
	{
		Bind(pIndexBuffer, currentBufferIdx);
	}

	uint32_t count = 0;
	if (isIndexedDrawing)
	{
		if (pIndexBuffer)
			count = pIndexBuffer->GetIndexCount();
	}
	else
	{
		count = pVertexBuffer->GetVertexCount();
	}

	auto* pVertexFormat = pGeometry->GetVertexFormat();
	assert(pVertexFormat != nullptr);

	uint32_t instanceCount = (pVertexFormat->GetVertexInputRate() == VertexFormat::VertexInputRate::GE_VIR_VERTEX ? 1 : 0); //TODO

	// in case of model loading
	if (pGeometry->IsModel())
	{
		Model* pModel = dynamic_cast<Model*>(pGeometry);
		if (pModel)
		{
			auto* gadrModel = Get(pModel);
			assert(gadrModel != nullptr);

			gadrModel->Draw([this, &currentBufferIdx, &instanceCount, &isIndexedDrawing](uint32_t indexCount, uint32_t firstIndex)
				{
					DrawDirect(indexCount, firstIndex, instanceCount, currentBufferIdx, isIndexedDrawing);
				});
		}
	}
	else
	{
		DrawDirect(count, 0, instanceCount, currentBufferIdx, isIndexedDrawing);
	}
}

void VulkanRenderer::UpdateNode(VisualPass* pVisualPass, GeometryNode* pGeoNode, Camera* pCamera, float32_t crrTime)
{
	assert(pVisualPass != nullptr);
	assert(pGeoNode != nullptr);
	assert(pCamera != nullptr);

	UpdateUniformBuffers(pVisualPass, pGeoNode, pCamera, crrTime);

	//TODO - other stuff to update
}

void VulkanRenderer::DrawDirect(uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount, uint32_t currentBufferIdx, bool_t isIndexedDrawing)
{
	assert(currentBufferIdx < mDrawCommandBuffers.size());

	auto pCrrDrawCommandBuffer = mDrawCommandBuffers[currentBufferIdx];
	assert(pCrrDrawCommandBuffer != nullptr);

	if (isIndexedDrawing)
	{
		// TODO - for now firstInstance, vertexOffset are both 0
		vkCmdDrawIndexed(pCrrDrawCommandBuffer->GetHandle(), indexCount, instanceCount, firstIndex, 0, 0);
	}
	else
	{
		// TODO - for now firstVertex/Index, firstInstance are both 0
		vkCmdDraw(pCrrDrawCommandBuffer->GetHandle(), indexCount, instanceCount, 0, 0);
	}
}

void VulkanRenderer::DrawIndirect(uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount, uint32_t currentBufferIdx, bool_t isIndexedDrawing)
{
	// TODO - indirect draw
}


/////////////////////////////////////

void VulkanRenderer::BeginRenderPass(const VisualPassData& visualPassData, uint32_t currentBufferIdx)
{
	assert(visualPassData.pRenderPass != nullptr);
	if (visualPassData.frameBuffers.size() > 1)
	{
		assert(currentBufferIdx < visualPassData.frameBuffers.size());
	}

	auto& dataRef = visualPassData.passBeginData;

	std::vector<VkClearValue> clearValues;

	if (visualPassData.passes[0]->GetPassType() == VisualPass::PassType::GE_PT_SHADOWS)
	{
		clearValues.resize(1);

		clearValues[0].depthStencil = { dataRef.depth, dataRef.stencil };
	}
	else // standard or offscreen
	{
		clearValues.resize(CLEARVALUES_COUNT);

		//TODO - allow custom clear color
		if (visualPassData.passes[0]->GetPassType() == VisualPass::PassType::GE_PT_STANDARD)
		{
			clearValues[0].color = { 0.5, 0.5, 0.5, 1.0 };
		}
		else
		{
			clearValues[0].color = { dataRef.clearColor.r, dataRef.clearColor.g, dataRef.clearColor.b, dataRef.clearColor.a };
		}

		clearValues[1].depthStencil = { dataRef.depth, dataRef.stencil };
	}


	// TODO - for now the render area size is the size of: screen/framebuffer/render target
	VkRect2D renderArea = { { 0, 0 }, { dataRef.width, dataRef.height } };

	auto* pVulkanCmdBuff = GetCommandBuffer(currentBufferIdx);
	assert(pVulkanCmdBuff != nullptr);

	auto& refFB = (visualPassData.frameBuffers.size() > 1 ? visualPassData.frameBuffers[currentBufferIdx] : visualPassData.frameBuffers[0]);
	visualPassData.pRenderPass->Begin(pVulkanCmdBuff->GetHandle(), refFB->GetHandle(), renderArea, clearValues);
}

void VulkanRenderer::EndRenderPass(const VisualPassData& visualPassData, uint32_t currentBufferIdx)
{
	assert(visualPassData.pRenderPass != nullptr);

	auto* pVulkanCmdBuff = GetCommandBuffer(currentBufferIdx);
	assert(pVulkanCmdBuff != nullptr);

	// NOTE! Ending the render pass will add an implicit barrier transitioning the frame buffer color attachment to 
	// VK_IMAGE_LAYOUT_PRESENT_SRC_KHR for presenting it to the windowing system (if it's the final pass)

	visualPassData.pRenderPass->End(pVulkanCmdBuff->GetHandle());

	// NOTE! Explicit synchronization is not required between the render pass, as this is done implicit via sub pass dependencies
}

void VulkanRenderer::AddVisualPass(VisualPass* pVisualPass)
{
	assert(pVisualPass != nullptr);

	auto* pGeoNode = pVisualPass->GetNode();
	assert(pGeoNode != nullptr);

	if (pGeoNode->IsPassAllowed(pVisualPass->GetPassType()))
	{
		auto it = mVisualPassMap.find(pVisualPass->GetPassType());
		if (it != mVisualPassMap.end())
		{
			bool foundNode = false;
			auto& passData = it->second;
			for (auto* pPass : passData.passes)
			{
				if (pPass && pPass->GetNode() == pGeoNode)
				{
					foundNode = true;
					break;
				}
			}

			if (false == foundNode)
			{
				passData.passes.push_back(pVisualPass);
			}
		}
		else
		{
			auto* pRenderPass = SetupRenderPass(pVisualPass);

			std::vector<VulkanFrameBuffer*> frameBuffers;
			VisualPassBeginData passBeginData;
			SetupFrameBuffers(pVisualPass, pRenderPass, frameBuffers, passBeginData);

			VisualPassData passData;
			passData.passBeginData = passBeginData;
			passData.pRenderPass = pRenderPass;
			passData.frameBuffers = frameBuffers;

			passData.passes.push_back(pVisualPass);

			mVisualPassMap.emplace(pVisualPass->GetPassType(), passData);
		}
	}
};

//////////////////////

VulkanDevice* VulkanRenderer::GetDevice() const
{
	return mpDevice;
}

VulkanPipelineCache* VulkanRenderer::GetPipelineCache() const
{
	return mpPipelineCache;
}

VulkanCommandPool* VulkanRenderer::GetCommandPool() const
{
	return mpCommandPool;
}

VulkanCommandBuffer* VulkanRenderer::GetCommandBuffer(uint32_t currentBufferIdx) const
{
	assert(currentBufferIdx < mDrawCommandBuffers.size());

	return mDrawCommandBuffers[currentBufferIdx];
}

VulkanRenderPass* VulkanRenderer::GetRenderPass(VisualPass* pVisualPass)
{
	assert(pVisualPass != nullptr);

	auto it = mVisualPassMap.find(pVisualPass->GetPassType());
	if (it != mVisualPassMap.end())
	{
		auto& passBuff = it->second;
		return passBuff.pRenderPass;
	}

	return nullptr;
}
#endif // VULKAN_RENDERER