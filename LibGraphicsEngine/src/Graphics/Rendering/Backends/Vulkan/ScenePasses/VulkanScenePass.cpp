#include "Graphics/Rendering/Backends/Vulkan/ScenePasses/VulkanScenePass.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Common/VulkanCommon.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Common/VulkanUtils.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanDevice.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanImage.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanImageView.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanRenderPass.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanFrameBuffer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanSwapChainBuffer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanCommandBuffer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanTexture.hpp"
#include "Graphics/Rendering/Backends/Vulkan/VulkanRenderer.hpp"
#include "Graphics/Rendering/ScenePasses/CompositeScenePass.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"
#include <cassert>

#define CLEARVALUES_COUNT 2

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

using namespace std::placeholders;  // for _1, _2, _3...

VulkanScenePass::VulkanScenePass()
	: mpVulkanRenderer(nullptr)
	, mpScenePass(nullptr)
{}

VulkanScenePass::VulkanScenePass(VulkanRenderer* pVulkanRenderer)
	: mpVulkanRenderer(pVulkanRenderer)
	, mpScenePass(nullptr)
{
	assert(mpVulkanRenderer != nullptr);
}

VulkanScenePass::~VulkanScenePass()
{
	for (auto& iter = mVulkanPassBuffersMap.begin(); iter != mVulkanPassBuffersMap.end(); ++iter)
	{
		auto& ref = iter->second;

		for (auto& fb : ref.frameBuffers)
		{
			GE_FREE(fb);
		}
		ref.frameBuffers.clear();

		GE_FREE(ref.pRenderPass);

		// NOTE! renderTargets are freed by the Renderer
		ref.renderTargets.clear();
	}
	mVulkanPassBuffersMap.clear();

	if (mpScenePass)
	{
		mpScenePass = nullptr;
	}
}

void VulkanScenePass::Init(ScenePass* pScenePass)
{
	assert(pScenePass != nullptr);
	mpScenePass = pScenePass;

	assert(mpVulkanRenderer != nullptr);

	VulkanDevice* pDevice = mpVulkanRenderer->GetDevice();
	assert(pDevice != nullptr);

	mpScenePass->Data().width = mpVulkanRenderer->GetWindowWidth();
	mpScenePass->Data().height = mpVulkanRenderer->GetWindowHeight();

	InitHelper(mpScenePass);
}

void VulkanScenePass::Reset()
{
	assert(mpScenePass != nullptr);

	ResetHelper(mpScenePass);
}

void VulkanScenePass::InitHelper(ScenePass* pScenePass)
{
	assert(pScenePass != nullptr);
	assert(mpVulkanRenderer != nullptr);

	switch (pScenePass->GetPassType())
	{
	case ScenePass::PassType::GE_PT_STANDARD:
	{
		// standard pass has the window width and height
		pScenePass->Data().width = mpVulkanRenderer->GetWindowWidth();
		pScenePass->Data().height = mpVulkanRenderer->GetWindowHeight();

		mVulkanPassBuffersMap[pScenePass->GetPassType()].pPass = pScenePass; // add pass

		SetupStandardRenderPass();
		SetupStandardFrameBuffers();
	}
		break;
	case ScenePass::PassType::GE_PT_OFFSCREEN:
	{
		// standard pass has the RT width and height
		auto* pColorRT = pScenePass->GetRenderTarget(RenderTarget::TargetType::GE_TT_COLOR);
		assert(pColorRT != nullptr);

		pScenePass->Data().width = pColorRT->GetWidth();
		pScenePass->Data().height = pColorRT->GetHeight();

		mVulkanPassBuffersMap[pScenePass->GetPassType()].pPass = pScenePass; // add pass

		SetupOffscreenRenderTargets();
		SetupOffscreenRenderPass();
		SetupOffscreenFrameBuffers();
	}
		break;
	case ScenePass::PassType::GE_PT_SHADOWS:
		//TODO
		break;
	case ScenePass::PassType::GE_PT_COMPOSITE:
	{
		auto& passes = pScenePass->GetPasses();
		ScenePass::ScenePassMap::iterator it;
		for (it = passes.begin(); it != passes.end(); ++it)
		{
			auto& pass = it->second;

			InitHelper(pass);
		}
	}
		break;
	default:
		LOG_ERROR("Invalid Pass Type!");
	}
}

void VulkanScenePass::ResetHelper(ScenePass* pScenePass)
{
	assert(pScenePass != nullptr);
	assert(mVulkanPassBuffersMap.size() > 0);

	auto findIter = mVulkanPassBuffersMap.find(pScenePass->GetPassType());
	assert(findIter != mVulkanPassBuffersMap.end());

	switch (pScenePass->GetPassType())
	{
	case ScenePass::PassType::GE_PT_STANDARD:
	{
		auto& frameBuffers = findIter->second.frameBuffers;
		for (auto& fb : frameBuffers)
		{
			GE_FREE(fb);
		}

		SetupStandardFrameBuffers();
	}
	break;
	case ScenePass::PassType::GE_PT_OFFSCREEN:
	{
		auto& frameBuffers = findIter->second.frameBuffers;
		for (auto& fb : frameBuffers)
		{
			GE_FREE(fb);
		}

		SetupOffscreenFrameBuffers();
	}
		break;
	case ScenePass::PassType::GE_PT_SHADOWS:
		//TODO
		break;
	case ScenePass::PassType::GE_PT_COMPOSITE:
	{
		auto& passes = pScenePass->GetPasses();
		ScenePass::ScenePassMap::const_iterator cIt;
		for (cIt = passes.begin(); cIt != passes.end(); ++cIt)
		{
			ResetHelper(cIt->second);
		}
	}
	break;
	default:
		LOG_ERROR("Invalid Pass Type!");
	}
}

void VulkanScenePass::SetupStandardRenderPass()
{
	/*
		In a standard render pass we have the following:
		- 1 color attachement
		- 1 depth + stencil attachment
		- 1 subpass
	*/

	assert(mpVulkanRenderer != nullptr);
	assert(mVulkanPassBuffersMap.size() > 0);

	auto standardIter = mVulkanPassBuffersMap.find(ScenePass::PassType::GE_PT_STANDARD);
	assert(standardIter != mVulkanPassBuffersMap.end());
	auto& standardRef = standardIter->second;

	VulkanDevice* pDevice = mpVulkanRenderer->GetDevice();
	assert(pDevice != nullptr);

	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = pDevice->GetSurfaceFormat().format;
	colorAttachment.samples = MIN_NUM_SAMPLES; // use at least 1 sample
	colorAttachment.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR; //clear color attachment on new frame
	colorAttachment.storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE; //store the color attachment in memory
	colorAttachment.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE; //color att, so we don't care avout the stencil op
	colorAttachment.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE; //color att, so we don't care avout the stencil op
	colorAttachment.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED; // we don't care of the previous layout the image was in
	colorAttachment.finalLayout = VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // the image needs to be presented to the swapchain

	VkAttachmentDescription depthStencilAttachment{};
	depthStencilAttachment.format = pDevice->GetDepthStencilFormat();
	depthStencilAttachment.samples = MIN_NUM_SAMPLES; // use at least 1 sample
	depthStencilAttachment.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR; //clear depth attachement on new frame
	depthStencilAttachment.storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE; //store the depth attachment on new frame
	depthStencilAttachment.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR; // we don't care about stencil, only depth
	depthStencilAttachment.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE; // we don't care about stencil, only depth
	depthStencilAttachment.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;  // we don't care of the previous layout the image was in
	depthStencilAttachment.finalLayout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; // the image needs to be presented to the swapchain

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
	subPassDep_1.srcStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	subPassDep_1.dstStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subPassDep_1.srcAccessMask = VkAccessFlagBits::VK_ACCESS_MEMORY_READ_BIT;
	subPassDep_1.dstAccessMask = VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	subPassDep_1.dependencyFlags = VkDependencyFlagBits::VK_DEPENDENCY_BY_REGION_BIT;

	VkSubpassDependency subPassDep_2;
	subPassDep_2.srcSubpass = SUBPASS_ID;
	subPassDep_2.dstSubpass = VK_SUBPASS_EXTERNAL;
	subPassDep_2.srcStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subPassDep_2.dstStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	subPassDep_2.srcAccessMask = VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	subPassDep_2.dstAccessMask = VkAccessFlagBits::VK_ACCESS_MEMORY_READ_BIT;
	subPassDep_2.dependencyFlags = VkDependencyFlagBits::VK_DEPENDENCY_BY_REGION_BIT;

	standardRef.pRenderPass = GE_ALLOC(VulkanRenderPass)(pDevice, { colorAttachment, depthStencilAttachment }, { subPass }, { subPassDep_1, subPassDep_2 });
	assert(standardRef.pRenderPass != nullptr);
}

void VulkanScenePass::SetupStandardFrameBuffers()
{
	assert(mpVulkanRenderer != nullptr);
	assert(mVulkanPassBuffersMap.size() > 0);

	auto standardIter = mVulkanPassBuffersMap.find(ScenePass::PassType::GE_PT_STANDARD);
	assert(standardIter != mVulkanPassBuffersMap.end());
	auto& standardRef = standardIter->second;

	VulkanDevice* pDevice = mpVulkanRenderer->GetDevice();
	assert(pDevice != nullptr);

	assert(standardIter->second.pRenderPass != nullptr);
	assert(mpVulkanRenderer->GetWindowWidth() > 0);
	assert(mpVulkanRenderer->GetWindowHeight() > 0);

	// prepare color + depth image views as attachments for the framebuffer
	std::vector<VkImageView> frameBufferAttachments(ATT_COUNT);

	// depth stencil
	auto* pDepthStencilBuffer = pDevice->GetSwapChainDepthStencilBuffer();
	assert(pDepthStencilBuffer != nullptr);
	frameBufferAttachments[DEPTH_ATT] = pDepthStencilBuffer->GetImageViewHandle();

	// color
	auto& colorBuffers = pDevice->GetSwapChainColorBuffers();
	assert(colorBuffers.empty() == false);

	// Create one frame buffer for each swap chain image and reuse for rendering
	if (standardRef.frameBuffers.empty())
	{
		standardRef.frameBuffers.resize(colorBuffers.size());
	}

	for (uint32_t i = 0; i < standardRef.frameBuffers.size(); ++i)
	{
		frameBufferAttachments[COLOR_ATT] = colorBuffers[i]->GetImageViewHandle();

		standardRef.frameBuffers[i] = GE_ALLOC(VulkanFrameBuffer)
			(
				pDevice,
				standardRef.pRenderPass,
				frameBufferAttachments,
				mpVulkanRenderer->GetWindowWidth(),
				mpVulkanRenderer->GetWindowHeight()
			);
		assert(standardRef.frameBuffers[i] != nullptr);
	}
}

void VulkanScenePass::SetupOffscreenRenderTargets()
{
	assert(mpVulkanRenderer != nullptr);
	assert(mVulkanPassBuffersMap.size() > 0);

	auto offscreenIter = mVulkanPassBuffersMap.find(ScenePass::PassType::GE_PT_OFFSCREEN);
	assert(offscreenIter != mVulkanPassBuffersMap.end());
	auto& offscreenRef = offscreenIter->second;

	VulkanDevice* pDevice = mpVulkanRenderer->GetDevice();
	assert(pDevice != nullptr);

	//// attachments 
	offscreenRef.renderTargets.resize(ATT_COUNT);


	auto* pColorRT = offscreenRef.pPass->GetRenderTarget(RenderTarget::TargetType::GE_TT_COLOR);
	assert(pColorRT != nullptr);

	auto* pDepthRT = offscreenRef.pPass->GetRenderTarget(RenderTarget::TargetType::GE_TT_DEPTH_STENCIL);
	assert(pDepthRT != nullptr);

	//TODO improve - check that all render target hace the same width and height
	assert(pColorRT->GetWidth() == pDepthRT->GetWidth());
	assert(pColorRT->GetHeight() == pDepthRT->GetHeight());

	//// color attachment
	auto& colorRef = offscreenRef.renderTargets[COLOR_ATT];

	colorRef = mpVulkanRenderer->Get(pColorRT);
	assert(colorRef != nullptr);

	//// depth attachment
	auto& depthRef = offscreenRef.renderTargets[DEPTH_ATT];

	depthRef = mpVulkanRenderer->Get(pDepthRT);
	assert(depthRef != nullptr);
}

void VulkanScenePass::SetupOffscreenRenderPass()
{
	/*
		In a standard render pass we have the following:
		- 1 color attachement
		- 1 depth + stencil attachment
		- 1 subpass
	*/

	assert(mpVulkanRenderer != nullptr);
	assert(mVulkanPassBuffersMap.size() > 0);

	auto offscreenIter = mVulkanPassBuffersMap.find(ScenePass::PassType::GE_PT_OFFSCREEN);
	assert(offscreenIter != mVulkanPassBuffersMap.end());
	auto& offscreenRef = offscreenIter->second;

	VulkanDevice* pDevice = mpVulkanRenderer->GetDevice();
	assert(pDevice != nullptr);

	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = offscreenRef.renderTargets[COLOR_ATT]->GetImage()->GetData().format; // color formata
	colorAttachment.samples = MIN_NUM_SAMPLES; // use at least 1 sample
	colorAttachment.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR; //clear color attachment on new frame
	colorAttachment.storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE; //store the color attachment in memory
	colorAttachment.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE; //color att, so we don't care avout the stencil op
	colorAttachment.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE; //color att, so we don't care avout the stencil op
	colorAttachment.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED; // we don't care of the previous layout the image was in
	colorAttachment.finalLayout = VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // to be able to read the image in the shader

	VkAttachmentDescription depthStencilAttachment{};
	depthStencilAttachment.format = offscreenRef.renderTargets[DEPTH_ATT]->GetImage()->GetData().format; //depth format
	depthStencilAttachment.samples = MIN_NUM_SAMPLES; // use at least 1 sample
	depthStencilAttachment.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR; //clear depth attachement on new frame
	depthStencilAttachment.storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE; //store the depth attachment on new frame
	depthStencilAttachment.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR; // we don't care about stencil, only depth
	depthStencilAttachment.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE; // we don't care about stencil, only depth
	depthStencilAttachment.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;  // we don't care of the previous layout the image was in
	depthStencilAttachment.finalLayout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; // the image needs to be presented to the swapchain

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
	subPassDep_1.srcStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	subPassDep_1.dstStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subPassDep_1.srcAccessMask = VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT;
	subPassDep_1.dstAccessMask = VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	subPassDep_1.dependencyFlags = VkDependencyFlagBits::VK_DEPENDENCY_BY_REGION_BIT;

	VkSubpassDependency subPassDep_2;
	subPassDep_2.srcSubpass = SUBPASS_ID;
	subPassDep_2.dstSubpass = VK_SUBPASS_EXTERNAL;
	subPassDep_2.srcStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subPassDep_2.dstStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	subPassDep_2.srcAccessMask = VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	subPassDep_2.dstAccessMask = VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT;
	subPassDep_2.dependencyFlags = VkDependencyFlagBits::VK_DEPENDENCY_BY_REGION_BIT;

	offscreenRef.pRenderPass = GE_ALLOC(VulkanRenderPass)(pDevice, { colorAttachment, depthStencilAttachment }, { subPass }, { subPassDep_1, subPassDep_2 });
	assert(offscreenRef.pRenderPass != nullptr);
}

void VulkanScenePass::SetupOffscreenFrameBuffers()
{
	assert(mpVulkanRenderer != nullptr);
	assert(mVulkanPassBuffersMap.size() > 0);

	auto offscreenIter = mVulkanPassBuffersMap.find(ScenePass::PassType::GE_PT_OFFSCREEN);
	assert(offscreenIter != mVulkanPassBuffersMap.end());
	auto& offscreenRef = offscreenIter->second;

	VulkanDevice* pDevice = mpVulkanRenderer->GetDevice();
	assert(pDevice != nullptr);

	assert(offscreenRef.renderTargets.size() == ATT_COUNT);
	assert(offscreenRef.pRenderPass != nullptr);

	// get RT width and height for the framebuffer size
	auto& extent = offscreenRef.renderTargets[COLOR_ATT]->GetImage()->GetData().extent;

	// prepare color + depth image views as attachments for the framebuffer
	std::vector<VkImageView> frameBufferAttachments(ATT_COUNT);
	frameBufferAttachments[COLOR_ATT] = offscreenRef.renderTargets[COLOR_ATT]->GetImageView()->GetHandle();
	frameBufferAttachments[DEPTH_ATT] = offscreenRef.renderTargets[DEPTH_ATT]->GetImageView()->GetHandle();

	// we need only 1 framebuffer
	offscreenRef.frameBuffers.push_back(GE_ALLOC(VulkanFrameBuffer)
		(
			pDevice,
			offscreenRef.pRenderPass,
			frameBufferAttachments,
			extent.width,
			extent.height
		));
	assert(offscreenRef.frameBuffers[0] != nullptr);
}

void VulkanScenePass::Begin(ScenePass* pScenePass, uint32_t currentBufferIdx)
{
	assert(mpVulkanRenderer != nullptr);
	assert(pScenePass != nullptr);
	assert(mVulkanPassBuffersMap.size() > 0);

	auto iter = mVulkanPassBuffersMap.find(pScenePass->GetPassType());
	assert(iter != mVulkanPassBuffersMap.end());

	auto& ref = iter->second;

	assert(ref.pRenderPass != nullptr);
	if (ref.frameBuffers.size() > 1)
	{
		assert(currentBufferIdx < ref.frameBuffers.size());
	}

	auto& dataRef = pScenePass->Data();

	std::vector<VkClearValue> clearValues(CLEARVALUES_COUNT);
	clearValues[0].color = { dataRef.clearColor.r, dataRef.clearColor.g, dataRef.clearColor.b, dataRef.clearColor.a };
	clearValues[1].depthStencil = { dataRef.depth, dataRef.stencil };


	// TODO - for now the render area size is the size of: screen/framebuffer/render target
	VkRect2D renderArea = { { 0.0f, 0.0f }, { dataRef.width, dataRef.height } };

	auto* pVulkanCmdBuff = mpVulkanRenderer->GetCommandBuffer(currentBufferIdx);
	assert(pVulkanCmdBuff != nullptr);

	auto& refFB = (ref.frameBuffers.size() > 1 ? ref.frameBuffers[currentBufferIdx] : ref.frameBuffers[0]);
	ref.pRenderPass->Begin(pVulkanCmdBuff->GetHandle(), refFB->GetHandle(), renderArea, clearValues);
}

void VulkanScenePass::End(ScenePass* pScenePass, uint32_t currentBufferIdx)
{
	assert(mpVulkanRenderer != nullptr);
	assert(pScenePass != nullptr);
	assert(mVulkanPassBuffersMap.size() > 0);

	auto iter = mVulkanPassBuffersMap.find(pScenePass->GetPassType());
	assert(iter != mVulkanPassBuffersMap.end());

	auto& ref = iter->second;

	assert(ref.pRenderPass != nullptr);

	auto* pVulkanCmdBuff = mpVulkanRenderer->GetCommandBuffer(currentBufferIdx);
	assert(pVulkanCmdBuff != nullptr);

	// NOTE! Ending the render pass will add an implicit barrier transitioning the frame buffer color attachment to 
	// VK_IMAGE_LAYOUT_PRESENT_SRC_KHR for presenting it to the windowing system (if it's the final pass)

	ref.pRenderPass->End(pVulkanCmdBuff->GetHandle());

	// NOTE! Explicit synchronization is not required between the render pass, as this is done implicit via sub pass dependencies
}

void VulkanScenePass::Render(Renderer* pRenderer, RenderQueue* pRenderQueue, uint32_t currentBufferIdx)
{
	assert(mpScenePass != nullptr);

	mpScenePass->Render(pRenderer, pRenderQueue, currentBufferIdx,
		std::bind(&VulkanScenePass::Begin, this, _1, _2),
		std::bind(&VulkanScenePass::End, this, _1, _2));
}

void VulkanScenePass::Update(Renderer* pRenderer, RenderQueue* pRenderQueue, Camera* pCamera, float32_t crrTime)
{
	assert(mpScenePass != nullptr);

	mpScenePass->Update(pRenderer, pRenderQueue, pCamera, crrTime);
}

VulkanRenderPass* VulkanScenePass::GetRenderPass(ScenePass::PassType passType)
{
	assert(mVulkanPassBuffersMap.size() > 0);

	auto iter = mVulkanPassBuffersMap.find(passType);
	assert(iter != mVulkanPassBuffersMap.end());

	return iter->second.pRenderPass;
}

ScenePass* VulkanScenePass::GetScenePass()
{
	return mpScenePass;
}

const std::vector<GADRTexture*>& VulkanScenePass::GetRenderTargets(ScenePass::PassType passType) const
{
	assert(mVulkanPassBuffersMap.size() > 0);

	auto iter = mVulkanPassBuffersMap.find(passType);
	assert(iter != mVulkanPassBuffersMap.end());

	return iter->second.renderTargets;
}