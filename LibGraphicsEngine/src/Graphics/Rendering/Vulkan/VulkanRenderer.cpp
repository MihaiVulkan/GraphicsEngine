#include "VulkanRenderer.hpp"

#include "Foundation/Platform/Platform.hpp"

#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"

#include "Graphics/Rendering/RenderQueue.hpp"
#include "Graphics/Rendering/RenderPasses/RenderPass.hpp"

#include "Graphics/SceneGraph/GeometryNode.hpp"
#include "Graphics/GeometricPrimitives/GeometricPrimitive.hpp"
#include "Graphics/Camera/Camera.hpp"

#include "Graphics/Components/VisualComponent.hpp"
#include "Graphics/Components/MaterialComponent.hpp"

#include "Graphics/Rendering/Resources/Material.hpp"
#include "Graphics/Rendering/Resources/UniformBuffer.hpp"

#include "Graphics/ShaderTools/GLSL/GLSLShaderParser.hpp"

// Common
#include "Graphics/Rendering/Vulkan/Common/VulkanCommon.hpp"
#include "Graphics/Rendering/Vulkan/Common/VulkanUtils.hpp"

// Resources
#include "Graphics/Rendering/Vulkan/Resources/VulkanVertexFormat.hpp"
#include "Graphics/Rendering/Vulkan/Resources/VulkanVertexBuffer.hpp"
#include "Graphics/Rendering/Vulkan/Resources/VulkanIndexBuffer.hpp"
#include "Graphics/Rendering/Vulkan/Resources/VulkanShader.hpp"
#include "Graphics/Rendering/Vulkan/Resources/VulkanTexture.hpp"
#include "Graphics/Rendering/Vulkan/Resources/VulkanMaterial.hpp"
#include "Graphics/Rendering/Vulkan/Resources/VulkanUniformBuffer.hpp"

//Internal
#include "Graphics/Rendering/Vulkan/Internal/VulkanPassThroughAllocator.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanPoolAllocator.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanDevice.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanQueue.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanBuffer.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanCommandPool.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanCommandBuffer.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanFrameBuffer.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanSwapChainBuffer.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanRenderPass.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanSemaphore.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanFence.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanShaderModule.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanDescriptorPool.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanDescriptorSetLayout.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanDescriptorSet.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanPipelineCache.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanPipelineLayout.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanGraphicsPipeline.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanQueryPool.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanInitializers.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanHelpers.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanDebug.hpp"

#include <unordered_map>

//#define PIPELINE_STATS


using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanRenderer::VulkanRenderer()
	: Renderer(RendererType::GE_RT_FORWARD)
	, mpDevice(nullptr)
	, mpDefaultRenderPass(nullptr)
	, mpRenderCompleteSemaphore(nullptr)
	, mpPresentCompleteSemaphore(nullptr)
	, mpCommandPool(nullptr)
	, mCurrentBufferIdx(0) 
	, mSubmitInfo{}
	, mpPipelineCache(nullptr)
	, mpQueryPool(nullptr)
{}


VulkanRenderer::~VulkanRenderer()
{
	Terminate();
}

void VulkanRenderer::Init(Platform::Window* pWindow)
{
	Renderer::Init(pWindow);

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

	//////////
	// pipeline objects
	for (auto iter = mPipelineDataCollection.begin(); iter != mPipelineDataCollection.end(); ++iter)
	{
		auto& ref = iter->second;
		GE_FREE(ref.pPipelineLayout);
		GE_FREE(ref.pGraphicsPipeline);
	}
	mPipelineDataCollection.clear();

	// descriptor set objects
	for (auto iter = mDescriptorSetDataCollection.begin(); iter != mDescriptorSetDataCollection.end(); ++iter)
	{
		auto& ref = iter->second;
		GE_FREE(ref.pDescriptorSetLayout);
		GE_FREE(ref.pDescriptorSet);
	}
	mDescriptorSetDataCollection.clear();

	GE_FREE(mpDescriptorPool);

	/////////////



#ifdef PIPELINE_STATS
	GE_FREE(mpQueryPool);
#endif

	// draw command buffers
	for (auto& commandBufferRef : mDrawCommandBuffers)
	{
		GE_FREE(commandBufferRef);
	}

	GE_FREE(mpCommandPool);

	GE_FREE(mpPipelineCache);

	// syncronization 
	GE_FREE(mpRenderCompleteSemaphore);
	GE_FREE(mpPresentCompleteSemaphore);

	for (auto& fence : mWaitFences)
	{
		GE_FREE(fence);
	}

	for (auto& fb : mDefaultFrameBuffers)
	{
		GE_FREE(fb);
	}

	GE_FREE(mpDefaultRenderPass);


	GE_FREE(mpDevice);
}


void VulkanRenderer::Prepare()
{
	////////// GENERAL RESOURCES //////////

	// All these general resources should be managed by the renderer or the Device itself!

	setupDefaultRenderPass();

	setupDefaultFrameBuffer();

	setupDrawCommandPool();

	setupDrawCommandBuffers();

	setupSynchronizationPrimitives();

	setupPipelineCache();

	setupSubmitInfo();

	setupPipelineStats();

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

	for (auto& fb : mDefaultFrameBuffers)
	{
		GE_FREE(fb);
	}

	setupDefaultFrameBuffer();

	// Command buffers need to be recreated as they may store
	// references to the recreated frame buffer
	for (auto& commandBufferRef : mDrawCommandBuffers)
	{
		GE_FREE(commandBufferRef);
	}

	setupDrawCommandBuffers();

	DrawSceneToCommandBuffer();

	mpDevice->WaitIdle();


	mIsPrepared = true;
}

void VulkanRenderer::setupDefaultRenderPass()
{
	assert(mpDevice != nullptr);

	// COLOR_AT
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = mpDevice->GetSurfaceFormat().format;
	colorAttachment.samples = MIN_NUM_SAMPLES;
	colorAttachment.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; /////////////

	VkAttachmentDescription depthStencilAttachment{};
	depthStencilAttachment.format = mpDevice->GetDepthStencilFormat();
	depthStencilAttachment.samples = MIN_NUM_SAMPLES;
	depthStencilAttachment.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthStencilAttachment.storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
	depthStencilAttachment.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR; //
	depthStencilAttachment.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthStencilAttachment.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
	depthStencilAttachment.finalLayout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; /////////

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


	mpDefaultRenderPass = GE_ALLOC(VulkanRenderPass)(mpDevice, { colorAttachment, depthStencilAttachment }, { subPass }, { subPassDep_1, subPassDep_2 });
	assert(mpDefaultRenderPass != nullptr);
}

void VulkanRenderer::setupDefaultFrameBuffer()
{
	assert(mpDevice != nullptr);
	assert(mpDefaultRenderPass != nullptr);
	assert(mWindowWidth > 0);
	assert(mWindowHeight > 0);

	// prepare color + depth image views as attachments for the framebuffer
	std::vector<VkImageView> frameBufferAttachments(ATT_COUNT);

	// depth stencil
	auto pDepthStencilBuffer = mpDevice->GetSwapChainDepthStencilBuffer();
	assert(pDepthStencilBuffer != nullptr);
	frameBufferAttachments[DEPTH_ATT] = pDepthStencilBuffer->GetImageViewHandle();

	// color
	auto colorBuffers = mpDevice->GetSwapChainColorBuffers();
	assert(colorBuffers.empty() == false);

	// Create one frame buffer for each swap chain image and reuse for rendering
	if (mDefaultFrameBuffers.empty())
	{
		mDefaultFrameBuffers.resize(colorBuffers.size());
	}

	for (uint32_t i = 0; i < mDefaultFrameBuffers.size(); ++i)
	{
		frameBufferAttachments[COLOR_ATT] = colorBuffers[i]->GetImageViewHandle();

		mDefaultFrameBuffers[i] = GE_ALLOC(VulkanFrameBuffer)
		(
			mpDevice,
			mpDefaultRenderPass,
			frameBufferAttachments,
			mWindowWidth, mWindowHeight
		);
		assert(mDefaultFrameBuffers[i] != nullptr);
	}
}

void VulkanRenderer::setupDrawCommandPool()
{
	assert(mpDevice != nullptr);

	auto pQueue = mpDevice->GetGraphicsQueue();
	assert(pQueue != nullptr);

	// we create a command pool for graphics commands
	mpCommandPool = GE_ALLOC(VulkanCommandPool)(mpDevice, pQueue->GetFamilyIndex());
	assert(mpCommandPool != nullptr);
}

void VulkanRenderer::setupDrawCommandBuffers()
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

void VulkanRenderer::setupSynchronizationPrimitives()
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

void VulkanRenderer::setupPipelineCache()
{
	assert(mpDevice != nullptr);

	mpPipelineCache = GE_ALLOC(VulkanPipelineCache)(mpDevice, {});
	assert(mpPipelineCache != nullptr);
}

void VulkanRenderer::setupSubmitInfo()
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

void VulkanRenderer::setupPipelineStats()
{
#ifdef PIPELINE_STATS
	assert(mpDevice != nullptr);

	mPipelineStatNames = {
			"Input assembly vertex count        ",
			"Input assembly primitives count    ",
			"Vertex shader invocations          ",
			"Clipping stage primitives processed",
			"Clipping stage primtives output    ",
			"Fragment shader invocations        "
	};
	
	mPipelineStats.resize(mPipelineStatNames.size());


	VkQueryPipelineStatisticFlags pipelineStatsFlags = 
		VkQueryPipelineStatisticFlagBits::VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_VERTICES_BIT |
		VkQueryPipelineStatisticFlagBits::VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_PRIMITIVES_BIT |
		VkQueryPipelineStatisticFlagBits::VK_QUERY_PIPELINE_STATISTIC_VERTEX_SHADER_INVOCATIONS_BIT |
		VkQueryPipelineStatisticFlagBits::VK_QUERY_PIPELINE_STATISTIC_CLIPPING_INVOCATIONS_BIT |
		VkQueryPipelineStatisticFlagBits::VK_QUERY_PIPELINE_STATISTIC_CLIPPING_PRIMITIVES_BIT |
		VkQueryPipelineStatisticFlagBits::VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT;

	mpQueryPool = GE_ALLOC(VulkanQueryPool)
	(
		mpDevice,
		VkQueryType::VK_QUERY_TYPE_PIPELINE_STATISTICS, 
		static_cast<uint32_t>(mPipelineStats.size()),
		pipelineStatsFlags
	);
	assert(mpQueryPool != nullptr);
#endif
}

// Retrieves the results of the pipeline statistics query submitted to the command buffer
void VulkanRenderer::getQueryResults()
{
#ifdef PIPELINE_STATS
	assert(mpQueryPool != nullptr);

	uint32_t count = static_cast<uint32_t>(mPipelineStats.size());
	VkQueryResultFlags flags = VkQueryResultFlagBits::VK_QUERY_RESULT_64_BIT;// | VkQueryResultFlagBits::VK_QUERY_RESULT_WAIT_BIT;

	mpQueryPool->GetQueryResults(1, mPipelineStats.data(), count * sizeof(uint64_t), sizeof(uint64_t), flags);

	// log query stats
	LOG_INFO("####### Pipeline Stats ########");
	for (uint32_t i = 0; i < mPipelineStats.size(); ++ i)
	{
		LOG_INFO("%s : %lu", mPipelineStatNames[i].c_str(), mPipelineStats[i]);
	}
	LOG_INFO("");
#endif
}

void VulkanRenderer::resetQuery(uint32_t currentBufferIdx)
{
#ifdef PIPELINE_STATS
	assert(currentBufferIdx < mDrawCommandBuffers.size());
	assert(mpQueryPool != nullptr);

	mpQueryPool->ResetQuery(mDrawCommandBuffers[currentBufferIdx]->GetHandle(), static_cast<uint32_t>(mPipelineStats.size()));
#endif
}

void VulkanRenderer::beginQuery(uint32_t currentBufferIdx)
{
#ifdef PIPELINE_STATS
	assert(currentBufferIdx < mDrawCommandBuffers.size());
	assert(mpQueryPool != nullptr);
	auto pCrrDrawCommandBuffer = mDrawCommandBuffers[currentBufferIdx];
	assert(pCrrDrawCommandBuffer != nullptr);

	mpQueryPool->BeginQuery(pCrrDrawCommandBuffer->GetHandle());
#endif
}

void VulkanRenderer::endQuery(uint32_t currentBufferIdx)
{
#ifdef PIPELINE_STATS
	assert(currentBufferIdx < mDrawCommandBuffers.size());
	assert(mpQueryPool != nullptr);

	auto pCrrDrawCommandBuffer = mDrawCommandBuffers[currentBufferIdx];
	assert(pCrrDrawCommandBuffer != nullptr);

	mpQueryPool->EndQuery(pCrrDrawCommandBuffer->GetHandle());
 #endif
}

void VulkanRenderer::RenderFrame(RenderQueue* pRenderQueue, RenderPass* pRenderPass)
{
	if (false == mIsPrepared)
		return;

	assert(pRenderQueue != nullptr);
	assert(pRenderPass != nullptr);

	//	mpRenderQueue = pRenderQueue;
	//	mpRenderPass = pRenderPass;

	BeginFrame();

	DrawSceneToCommandBuffer();

	EndFrame();
}

void VulkanRenderer::UpdateFrame(Camera* pCamera, float32_t deltaTime)
{
	if (false == mIsPrepared)
		return;

	assert(mpRenderQueue != nullptr);
	assert(mpRenderPass != nullptr);

	assert(pCamera != nullptr);
	mpCamera = pCamera;

	mpRenderPass->Update(this, mpRenderQueue, mpCamera, deltaTime);

	/// pipeline stats per frame
	getQueryResults();
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

///////////////////////

void VulkanRenderer::DrawObject(RenderQueue::Renderable* pRenderable, uint32_t currentBufferIdx)
{
	/*if (false == mIsPrepared)
		return;*/

	assert(pRenderable != nullptr);
	assert(currentBufferIdx < mDrawCommandBuffers.size());

	assert(mpRenderPass != nullptr);
	assert(mDescriptorSetDataCollection.empty() == false);
	assert(mPipelineDataCollection.empty() == false);

	auto pGeoNode = pRenderable->pGeometryNode;
	assert(pGeoNode != nullptr);

	auto pCrrDrawCommandBuffer = mDrawCommandBuffers[currentBufferIdx];
	assert(pCrrDrawCommandBuffer != nullptr);

	VisualComponent* pVisComp = pGeoNode->GetComponent<VisualComponent>();
	assert(pVisComp != nullptr);

	// update dynamic states if any
	auto& dynamicState = pVisComp->GetDynamicState();

	if (dynamicState.HasStates())
	{
		UpdateDynamicStates(dynamicState, currentBufferIdx);
	}


	// uniform -> descriptorsets bindings
	// Bind descriptor sets describing shader binding points
	auto descriptorSet = mDescriptorSetDataCollection[mpRenderPass->GetPassType()].pDescriptorSet;
	assert(descriptorSet != nullptr);
	auto pipelineLayout = mPipelineDataCollection[mpRenderPass->GetPassType()].pPipelineLayout;
	assert(pipelineLayout != nullptr);
	vkCmdBindDescriptorSets(pCrrDrawCommandBuffer->GetHandle(), VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout->GetHandle(), 0, 1, &descriptorSet->GetHandle(), 0, nullptr);

	//bind material
	/*if (pRenderable->pMaterial)
	{
		Bind(pRenderable->pMaterial, currentBufferIdx);
	}*/

	// draw geometric primitives
	auto& geoPrimitives = pGeoNode->GetGeometricPrimitives();
	for (auto& primitive : geoPrimitives)
	{
		if (primitive)
		{
			//bind vertex format
			Bind(primitive->GetVertexFormat());
			
			// bind vertex buffer
			auto* pVertexBuffer = primitive->GetVertexBuffer();
			assert(pVertexBuffer != nullptr);

			Bind(pVertexBuffer, currentBufferIdx);

			// bind index buffer
			auto* pIndexBuffer = primitive->GetIndexBuffer();
			assert(pIndexBuffer != nullptr);

			if (primitive->IsIndexed())
			{
				Bind(pIndexBuffer, currentBufferIdx);
			}

			uint32_t count = (primitive->IsIndexed() ? pIndexBuffer->GetIndexCount() : pVertexBuffer->GetVertexCount());

			//TODO - improve instance count logic
			uint32_t instanceCount = (pVertexBuffer->GetVertexInputRate() == VertexBuffer::VertexInputRate::GE_VIR_VERTEX ? 1 : 0);

			DrawDirect(count, instanceCount, currentBufferIdx, primitive->IsIndexed());
		}
	}
}

void VulkanRenderer::ComputeGraphicsResources(RenderQueue* pRenderQueue, RenderPass* pRenderPass)
{
	if (false == mIsPrepared)
		return;

	assert(pRenderQueue != nullptr);
	assert(pRenderPass != nullptr);

	mpRenderQueue = pRenderQueue;
	mpRenderPass = pRenderPass;

	// TODO - compute resources for all renderable types
	auto renderableList = mpRenderQueue->GetRenderables(RenderQueue::RenderableType::GE_RT_OPAQUE);

	mpRenderQueue->ForEach(renderableList,
		[&, this](RenderQueue::Renderable* pRenderable)
		{
			assert(pRenderable != nullptr);

			auto pGeoNode = pRenderable->pGeometryNode;
			assert(pGeoNode != nullptr);

			auto pVisComp = pGeoNode->GetComponent<VisualComponent>();
			assert(pVisComp != nullptr);

			//TODO - shouldn't be done per primitive
			pGeoNode->ForEachPrimitive(
				[this, &pVisComp](GeometricPrimitive* pGeoPrimitive)
				{
					setupPipeline(pGeoPrimitive, pVisComp);
				});
		}
	);
}

void VulkanRenderer::setupPipeline(GeometricPrimitive* pGeoPrimitive, VisualComponent* pVisComp)
{
	assert(pGeoPrimitive != nullptr);
	assert(pVisComp != nullptr);
	assert(mpRenderPass != nullptr);
	assert(mpDevice != nullptr);

	// setup Shader Bindings - uniforms, samplers, etc.

	// setup Shaders
	const auto& shaders = pVisComp->GetShaders();

	// Shaders

	// pipeline shader stages data
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
	shaderStages.resize(shaders.size());

	////////// descriptor/uniform map data
	struct DescriptorSetBindingData
	{
		VkShaderStageFlagBits shaderStage;
		VkDescriptorSetLayoutBinding layoutBinding;
		VkWriteDescriptorSet writeSet;
		VkCopyDescriptorSet copySet;
	};
	std::unordered_map<VkDescriptorType, std::vector<DescriptorSetBindingData>> descriptorSetBindingMap;
	/////////

	size_t i = 0;
	for (auto iter = shaders.begin(); iter != shaders.end(); ++ iter)
	{
		auto shader = iter->second;
		if (shader)
		{
			auto parser = shader->GetGLSLParser();
			assert(parser != nullptr);

			// COMPUTE DESCRIPTOR SET INFO

			VkShaderStageFlagBits shaderStage = VulkanUtils::ShaderStageToVulkanShaderStage(shader->GetShaderStage());

			// uniform buffer info
			if (parser->GetUniformBlock().IsValid())
			{
				// uniform data
				GADRUniformBuffer* pGadrUniformBuffer = nullptr;
				if (pVisComp->HasUniformBuffers()) //get available one
				{
					pGadrUniformBuffer = Get(pVisComp->GetUniformBuffer(shader->GetShaderStage()));
				}
				else
				{
					//otherwise make one of our own
					//pVisComp->AddUniformBuffer();

				}
				assert(pGadrUniformBuffer != nullptr);

				// descriptor metadata
				VkDescriptorType descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

				DescriptorSetBindingData descriptorSetBindingData{};

				int32_t uboBinding = parser->GetUniformBlock().binding;

				// So every shader binding should map to one descriptor set layout binding
				VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{};
				descriptorSetLayoutBinding.binding = uboBinding; // taken from the shader
				descriptorSetLayoutBinding.descriptorType = descriptorType;
				descriptorSetLayoutBinding.descriptorCount = 1;
				descriptorSetLayoutBinding.stageFlags = shaderStage;
				descriptorSetBindingData.layoutBinding = descriptorSetLayoutBinding;

				VkWriteDescriptorSet writeDescriptorSet = VulkanInitializers::WriteDescriptorSet
				(
					VK_NULL_HANDLE,
					uboBinding,
					0, 1,
					descriptorType,
					nullptr, &(pGadrUniformBuffer->GetVKBuffer()->GetDescriptorInfo())
				);

				descriptorSetBindingData.writeSet = writeDescriptorSet;

				descriptorSetBindingMap[descriptorType] = { descriptorSetBindingData };
			}

			// pipeline shader state setup
			auto GADRShader = Get(shader);
			assert(GADRShader != nullptr);

			auto refVkShader = GADRShader->GetVkShaderModule();
			assert(refVkShader != nullptr);

			shaderStages[i++] = VulkanInitializers::PipelineShaderStageCreateInfo
			(
				shaderStage,
				refVkShader->GetHandle(),
				SHADER_ENTRY_POINT
			);
		}
	}

	// setup Textures - TODO


	////////////////////////////////////

	//// setup Descriptor pool
	{
		std::vector<VkDescriptorPoolSize> poolSizes;

		for (auto iter = descriptorSetBindingMap.begin(); iter != descriptorSetBindingMap.end(); ++ iter)
		{
			auto descriptorType = iter->first;
			uint32_t descriptorCount = static_cast<uint32_t>(iter->second.size());

			VkDescriptorPoolSize descriptorPoolSize{};
			descriptorPoolSize.type = descriptorType;
			descriptorPoolSize.descriptorCount = descriptorCount;

			poolSizes.push_back(descriptorPoolSize);
		}

		//////////////

		// TODO - properly compute the number of allowed descriptor sets from this descriptor pool
		uint32_t descriptorSetMaxCount = poolSizes.size();

		// Create the global descriptor pool
		// All descriptors used in this example are allocated from this pool
		if (mpDescriptorPool == nullptr)
		{
			mpDescriptorPool = GE_ALLOC(VulkanDescriptorPool)(mpDevice, descriptorSetMaxCount, poolSizes);
			assert(mpDescriptorPool != nullptr);
		}
		///////////
	}

	// setup Descriptor sets
	{
		// Basically connects the different shader stages to descriptors for binding uniform buffers, image samplers, etc.
		std::vector<VkDescriptorSetLayoutBinding> layoutBindings;

		for (auto iter = descriptorSetBindingMap.begin(); iter != descriptorSetBindingMap.end(); ++iter)
		{
			auto descriptorType = iter->first;
			auto& descritptorSetBidingDataCollection = iter->second;

			for (auto& data : descritptorSetBidingDataCollection)
			{
				layoutBindings.push_back(data.layoutBinding);
			}
		}

		DescriptorSetData descriptorData{};

		descriptorData.pDescriptorSetLayout = GE_ALLOC(VulkanDescriptorSetLayout)(mpDevice, layoutBindings);
		assert(descriptorData.pDescriptorSetLayout != nullptr);

		// Allocate a new descriptor set from the global descriptor pool
		descriptorData.pDescriptorSet = GE_ALLOC(VulkanDescriptorSet)(mpDevice, mpDescriptorPool, { descriptorData.pDescriptorSetLayout });
		assert(descriptorData.pDescriptorSet != nullptr);

		// Update the descriptor set determining the shader binding points
		// For every binding point used in a shader there needs to be one
		// descriptor set matching that binding point
		std::vector<VkWriteDescriptorSet> writeDescriptorSets;

		for (auto iter = descriptorSetBindingMap.begin(); iter != descriptorSetBindingMap.end(); ++iter)
		{
			auto descriptorType = iter->first;
			auto& descritptorSetBidingDataCollection = iter->second;

			for (auto& data : descritptorSetBidingDataCollection)
			{
				// set descriptorset handle
				data.writeSet.dstSet = descriptorData.pDescriptorSet->GetHandle();

				writeDescriptorSets.push_back(data.writeSet);
			}
		}

		descriptorData.pDescriptorSet->Update(writeDescriptorSets, {});

		// TODO - improve render pass management
		mDescriptorSetDataCollection[mpRenderPass->GetPassType()] = descriptorData;
	}	
	////////////

	// setup vertex format
	GADRVertexFormat* gadrVertexFormat = Get(pGeoPrimitive->GetVertexFormat());
	assert(gadrVertexFormat != nullptr);

	Shader* pVertexShader = pVisComp->GetShader(Shader::ShaderStage::GE_SS_VERTEX);
	assert(pVertexShader != nullptr);

	GLSLShaderParser* pGlslParser = pVertexShader->GetGLSLParser();
	assert(pGlslParser != nullptr);

	auto shaderAttribs = pGlslParser->GetVertexAttributes();

	// update attribute locations based on the current used vertex shader
	auto inputAttribs = gadrVertexFormat->GetVkInputAttributes();

	// we must have the same number of attributes (shader data vs input data)
	assert(shaderAttribs.size() == inputAttribs.size());

	std::vector<VkVertexInputAttributeDescription> updatedInputAttribs(inputAttribs.size());

	size_t index = 0;
	auto inputAttIter = inputAttribs.begin();
	auto shaderAttIter = shaderAttribs.begin();
	for (; inputAttIter != inputAttribs.end(); ++inputAttIter, ++shaderAttIter)
	{
		auto& refAtt = updatedInputAttribs[index];
		refAtt = inputAttribs[inputAttIter->first];
		auto refInput = shaderAttIter->second.pInput;
		if (refInput)
			refAtt.location = refInput->location;

		index++;
	}

	// setup vertex input
	GADRVertexBuffer* gadrVertexBuffer = Get(pGeoPrimitive->GetVertexBuffer());
	assert(gadrVertexBuffer != nullptr);

	const auto& inputBinding = gadrVertexBuffer->GetVkInputBinding();

	VkPrimitiveTopology vulkanTopology = VulkanUtils::PrimitiveTopologyToVulkanTopolgy(pGeoPrimitive->GetTopology());

	// TODO - improve resource creation - get it first time to create the Vulkan Index Buffer
	Get(pGeoPrimitive->GetIndexBuffer());

	/////////////////////////////////
	VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo =
		VulkanInitializers::PipelineVertexInputStateCreateInfo(1, &inputBinding, static_cast<uint32_t>(updatedInputAttribs.size()), updatedInputAttribs.data());

	// Input assembly state describes how primitives are assembled
	// This pipeline will assemble vertex data as a triangle lists (though we only use one triangle)
	auto restartPrimitve = pGeoPrimitive->GetIndexBuffer()->GetIsRestartPrimitive();

	VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo =
		VulkanInitializers::PipelineInputAssemblyStateCreateInfo(vulkanTopology, restartPrimitve);


	////////////////////////////////////

	auto& dynamicState = pVisComp->GetDynamicState();

	// Viewport state sets the number of viewports and scissor used in this pipeline
	// Note: This is actually overriden by the dynamic states (see below)
	VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{};

	// if viewport and scisosrs are part of dynamic state, we don't handlle them here!
	if (dynamicState.HasStates())
	{
		pipelineViewportStateCreateInfo = VulkanInitializers::PipelineViewportStateCreateInfo(MIN_NUM_VIEWPORTS, nullptr, MIN_NUM_SCISSORS, nullptr);
	}
	else
	{
		VkViewport viewport{};
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = static_cast<float32_t>(GetWindowWidth());
		viewport.height = static_cast<float32_t>(GetWindowHeight());
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = VkOffset2D { 0, 0 };
		scissor.extent = VkExtent2D{ GetWindowWidth(), GetWindowHeight() };

		pipelineViewportStateCreateInfo = VulkanInitializers::PipelineViewportStateCreateInfo(1, &viewport, 1, &scissor);
	}

	// Rasterization state
	VkPolygonMode vulkanPolygonMode = VulkanUtils::PrimitivePolygonModeToVulkanPolygonMode(pGeoPrimitive->GetPolygonMode());
	VkFrontFace vulkanFaceWinding = VulkanUtils::PrimitiveFaceWindingToVulkanFaceWinding(pGeoPrimitive->GetFaceWinding());
	VkCullModeFlagBits vulkanCullMode = VulkanUtils::FaceCullModeToVulkanFaceCullMode(pVisComp->GetCullFaceState().GetCullMode());


	VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo =
		VulkanInitializers::PipelineRasterizationStateCreateInfo(VK_FALSE, VK_FALSE, vulkanPolygonMode, vulkanCullMode,
			vulkanFaceWinding, VK_FALSE, 0, 0, 0, RASTER_MIN_LINE_WIDTH);

	// Multi sampling state
	// This example does not make use fo multi sampling (for anti-aliasing), the state must still be set and passed to the pipeline
	VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo =
		VulkanInitializers::PipelineMultisampleStateCreateInfo(MIN_NUM_SAMPLES, VK_FALSE, 0.0f, nullptr, VK_FALSE, VK_FALSE);

	// Depth and stencil state containing depth and stencil compare and test operations
	// We only use depth tests and want depth tests and writes to be enabled and compare with less or equal
	auto& depthStencilState = pVisComp->GetDepthStencilState();

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
	VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo =
		VulkanInitializers::PipelineDepthStencilStateCreateInfo(depthEnabled, depthEnabled && depthWritable, depthCompareOp, VK_FALSE, stencilEnabled, stencilOpState, stencilOpState, 0, 0);

	// Color blend state describes how blend factors are calculated (if used)
	// We need one blend attachment state per color attachment (even if blending is not used)
	auto& colorBlendState = pVisComp->GetColorBlendState();

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

	VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo =
		VulkanInitializers::PipelineColorBlendStateCreateInfo(VK_FALSE, VkLogicOp::VK_LOGIC_OP_NO_OP, 1, &pipelineColorBlendAttachmentState, &constantColor[0]);

	// Enable dynamic states
	// Most states are baked into the pipeline, but there are still a few dynamic states that can be changed within a command buffer
	// To be able to change these we need do specify which dynamic states will be changed using this pipeline. Their actual states are set later on in the command buffer.
	// For this example we will set the viewport and scissor using dynamic states
	VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
	std::vector<VkDynamicState> vulkanDynamicStates;

	if (dynamicState.HasStates())
	{
		auto dynamicStates = dynamicState.GetStates();
		vulkanDynamicStates.resize(dynamicStates.size());

		for (size_t i = 0; i < vulkanDynamicStates.size(); ++i)
		{
			vulkanDynamicStates[i] = VulkanUtils::DynamicStateToVulkanDynamicState(dynamicStates[i]);
		}

		pipelineDynamicStateCreateInfo = VulkanInitializers::PipelineDynamicStateCreateInfo(static_cast<uint32_t>(vulkanDynamicStates.size()), vulkanDynamicStates.data());
	}
	else
	{
		pipelineDynamicStateCreateInfo = VulkanInitializers::PipelineDynamicStateCreateInfo(0, nullptr);
	}

	///////////////////
	{
		PipelineData pipelineData{};

		pipelineData.pPipelineLayout = GE_ALLOC(VulkanPipelineLayout)
			(
				mpDevice,
				{ mDescriptorSetDataCollection[mpRenderPass->GetPassType()].pDescriptorSetLayout }, {}
		);
		assert(pipelineData.pPipelineLayout != nullptr);

		//// Assign the pipeline states to the pipeline creation info structure
		pipelineData.pGraphicsPipeline = GE_ALLOC(VulkanGraphicsPipeline)
			(
				mpDevice,
				GetPipelineCache(),
				shaderStages,
				pipelineVertexInputStateCreateInfo, pipelineInputAssemblyStateCreateInfo, {},
				pipelineViewportStateCreateInfo, pipelineRasterizationStateCreateInfo,
				pipelineMultisampleStateCreateInfo, pipelineDepthStencilStateCreateInfo,
				pipelineColorBlendStateCreateInfo, pipelineDynamicStateCreateInfo,
				pipelineData.pPipelineLayout, GetDefaultRenderPass()
				);
		assert(pipelineData.pGraphicsPipeline != nullptr);

		mPipelineDataCollection[mpRenderPass->GetPassType()] = pipelineData;
	}
}

void VulkanRenderer::UpdateUniformBuffers(RenderQueue::Renderable* pRenderable, Camera* pCamera)
{
	assert(pRenderable != nullptr);
	assert(pRenderable->pGeometryNode != nullptr);
	assert(pCamera != nullptr);

	VisualComponent* pVisComp = pRenderable->pGeometryNode->GetComponent<VisualComponent>();
	assert(pVisComp != nullptr);

	auto shaders = pVisComp->GetShaders();
	for (auto iter = shaders.begin(); iter != shaders.end(); ++iter)
	{
		auto shaderStage = iter->first;
		auto uniformBuffer = pVisComp->GetUniformBuffer(shaderStage);

		if (uniformBuffer)
		{
			if (uniformBuffer->HasUniform(GLSLShaderTypes::UniformType::GE_UT_PVM_MATRIX4))
			{
				uniformBuffer->SetUniform(GLSLShaderTypes::UniformType::GE_UT_PVM_MATRIX4, pCamera->GetProjectionViewMatrix());	

				Bind(uniformBuffer);
			}

		}
	}
}

void VulkanRenderer::UpdateDynamicStates(const DynamicState& dynamicState, uint32_t currentBufferIdx)
{
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
			viewport.width = static_cast<float32_t>(GetWindowWidth());
			viewport.height = static_cast<float32_t>(GetWindowHeight());
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			vkCmdSetViewport(pCrrDrawCommandBuffer->GetHandle(), 0, 1, &viewport);
		}
		break;
		case VkDynamicState::VK_DYNAMIC_STATE_SCISSOR:
		{
			VkRect2D scissor{};
			scissor.offset = VkOffset2D{ 0, 0 };
			scissor.extent = VkExtent2D{ GetWindowWidth(), GetWindowHeight() };

			vkCmdSetScissor(pCrrDrawCommandBuffer->GetHandle(), 0, 1, &scissor);
		}
		break;

		//TODO other cases

		default:
			LOG_ERROR("Invalid dynamic state!");
		}
	}
}

void VulkanRenderer::BindPipeline(uint32_t currentBufferIdx)
{
	assert(currentBufferIdx < mDrawCommandBuffers.size());
	assert(mpRenderPass != nullptr);

	auto pCrrDrawCommandBuffer = mDrawCommandBuffers[currentBufferIdx];
	assert(pCrrDrawCommandBuffer != nullptr);

	auto pipeline = mPipelineDataCollection[mpRenderPass->GetPassType()].pGraphicsPipeline;
	assert(pipeline != nullptr);
	// Bind the rendering pipeline
	// The pipeline (state object) contains all states of the rendering pipeline, binding it will set all the states specified at pipeline creation time
	vkCmdBindPipeline(pCrrDrawCommandBuffer->GetHandle(), VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetHandle());
}

void VulkanRenderer::DrawSceneToCommandBuffer()
{
	assert(mpRenderPass != nullptr);
	assert(mpRenderQueue != nullptr);

	// THE ACTUAL RENDERING IS DONE HERE !!!!

	// Set clear values for all framebuffer attachments with loadOp set to clear
	// We use two attachments (color and depth) that are cleared at the start of the subpass and as such we need to set clear values for both
	std::vector<VkClearValue> clearValues(2);
	clearValues[0].color = { { 0.0f, 0.0f, 0.2f, 1.0f } };
	clearValues[1].depthStencil = { 1.0f, 0 };

	VkRect2D renderArea{};
	renderArea.offset = VkOffset2D { 0, 0 };
	renderArea.extent = VkExtent2D { mWindowWidth, mWindowHeight };

	for (int32_t i = 0; i < mDrawCommandBuffers.size(); ++i)
	{
		assert(mDrawCommandBuffers[i] != nullptr);

		// Begin command buffer recording
		VK_CHECK_RESULT(mDrawCommandBuffers[i]->Begin());

		resetQuery(i);

		// RenderPass Begin
		mpDefaultRenderPass->Begin(mDrawCommandBuffers[i]->GetHandle(), mDefaultFrameBuffers[i]->GetHandle(), renderArea, clearValues);

		beginQuery(i);

		mpRenderPass->Render(this, mpRenderQueue, i);

		endQuery(i);

		// RenderPass End
		// Ending the render pass will add an implicit barrier transitioning the frame buffer color attachment to 
		// VK_IMAGE_LAYOUT_PRESENT_SRC_KHR for presenting it to the windowing system
		mpDefaultRenderPass->End(mDrawCommandBuffers[i]->GetHandle());

		// End command buffer recording
		VK_CHECK_RESULT(mDrawCommandBuffers[i]->End());
	}
}

void VulkanRenderer::DrawDirect(uint32_t vertexIndexCount, uint32_t instanceCount, uint32_t currentBufferIdx, bool_t isIndexedDrawing)
{
	assert(currentBufferIdx < mDrawCommandBuffers.size());

	auto pCrrDrawCommandBuffer = mDrawCommandBuffers[currentBufferIdx];
	assert(pCrrDrawCommandBuffer != nullptr);

	if (isIndexedDrawing)
	{
		// TODO - for now firstVertex/Index, firstInstance, vertexOffset are all 0
		vkCmdDrawIndexed(pCrrDrawCommandBuffer->GetHandle(), vertexIndexCount, instanceCount, 0, 0, 0);
	}
	else
	{
		// TODO - for now firstVertex/Index, firstInstance are all 0
		vkCmdDraw(pCrrDrawCommandBuffer->GetHandle(), vertexIndexCount, instanceCount, 0, 0);
	}
}

void VulkanRenderer::DrawIndirect(uint32_t vertexIndexCount, uint32_t instanceCount, uint32_t currentBufferIdx, bool_t isIndexedDrawing)
{
	// TODO - indirect draw
}


/////////////////////////////////////

VulkanDevice* VulkanRenderer::GetDevice() const
{
	return mpDevice;
}

VulkanPipelineCache* VulkanRenderer::GetPipelineCache() const
{
	return mpPipelineCache;
}

VulkanRenderPass* VulkanRenderer::GetDefaultRenderPass() const
{
	return mpDefaultRenderPass;
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