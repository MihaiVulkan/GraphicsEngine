#include "Graphics/Rendering/Backends/Vulkan/VulkanRenderer.hpp"

#include "Foundation/Platform/Platform.hpp"

#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"

#include "Graphics/Rendering/RenderQueue.hpp"

#include "Graphics/GeometricPrimitives/GeometricPrimitive.hpp"
#include "Graphics/SceneGraph/GeometryNode.hpp"
#include "Graphics/SceneGraph/LightNode.hpp"
#include "Graphics/Lights/DirectionalLight.hpp"
#include "Graphics/Cameras/Camera.hpp"

#include "Graphics/Components/VisualComponent.hpp"
#include "Graphics/Components/MaterialComponent.hpp"

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
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanCommandPool.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanCommandBuffer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanFrameBuffer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanSwapChainBuffer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanRenderPass.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanSemaphore.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanFence.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanShaderModule.hpp"
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

#include <unordered_map>

//#define PIPELINE_STATS

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanRenderer::VulkanRenderer()
	: Renderer()
	, mpDevice(nullptr)
	, mpScenePass(nullptr)
	, mpRenderCompleteSemaphore(nullptr)
	, mpPresentCompleteSemaphore(nullptr)
	, mpCommandPool(nullptr)
	, mCurrentBufferIdx(0) 
	, mSubmitInfo{}
	, mpPipelineCache(nullptr)
	, mpQueryPool(nullptr)
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
	, mpQueryPool(nullptr)
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

	//////////
	// pipeline objects
	for (auto iter = mPipelineDataCollection.begin(); iter != mPipelineDataCollection.end(); ++iter)
	{
		auto& pipelineDataVector = iter->second;
		for (auto& ref : pipelineDataVector)
		{
			GE_FREE(ref.pPipelineLayout);
			GE_FREE(ref.pGraphicsPipeline);
			ref.pVisualComponent = nullptr;
		}
	}
	mPipelineDataCollection.clear();

	// descriptor set objects
	for (auto iter = mDescriptorSetDataCollection.begin(); iter != mDescriptorSetDataCollection.end(); ++iter)
	{
		auto& refVector = iter->second;
		for (auto& ref : refVector)
		{
			GE_FREE(ref.pDescriptorSetLayout);
			GE_FREE(ref.pDescriptorSet);
			ref.pVisualComponent = nullptr;
		}
		refVector.clear();
	}
	mDescriptorSetDataCollection.clear();

	// descriptor pools
	for (auto iter = mpDescriptorPoolMap.begin(); iter != mpDescriptorPoolMap.end(); ++iter)
	{
		auto& refVector = iter->second;
		for (auto& ref : refVector)
		{
			GE_FREE(ref.pDescriptorPool);
			ref.pVisualComponent = nullptr;
		}
		refVector.clear();
	}
	mpDescriptorPoolMap.clear();

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

	GE_FREE(mpScenePass);

	GE_FREE(mpDevice);
}


void VulkanRenderer::Prepare()
{
	////////// GENERAL RESOURCES //////////

	// All these general resources should be managed by the renderer or the Device itself!

	mpScenePass = GE_ALLOC(VulkanScenePass)(this);
	assert(mpScenePass != nullptr);

	SetupDrawCommandPool();

	SetupDrawCommandBuffers();

	SetupSynchronizationPrimitives();

	SetupPipelineCache();

	SetupSubmitInfo();

	SetupPipelineStats();

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
	assert(mpScenePass != nullptr);

	// Ensure all operations on the device have been finished before destroying resources
	mpDevice->WaitIdle();

	// Recreate swap chain
	mpDevice->ResetSwapChain();

	mpScenePass->Reset();

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
void VulkanRenderer::GetQueryResults()
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

void VulkanRenderer::ResetQuery(uint32_t currentBufferIdx)
{
#ifdef PIPELINE_STATS
	assert(currentBufferIdx < mDrawCommandBuffers.size());
	assert(mpQueryPool != nullptr);

	mpQueryPool->ResetQuery(mDrawCommandBuffers[currentBufferIdx]->GetHandle(), static_cast<uint32_t>(mPipelineStats.size()));
#endif
}

void VulkanRenderer::BeginQuery(uint32_t currentBufferIdx)
{
#ifdef PIPELINE_STATS
	assert(currentBufferIdx < mDrawCommandBuffers.size());
	assert(mpQueryPool != nullptr);
	auto pCrrDrawCommandBuffer = mDrawCommandBuffers[currentBufferIdx];
	assert(pCrrDrawCommandBuffer != nullptr);

	mpQueryPool->BeginQuery(pCrrDrawCommandBuffer->GetHandle());
#endif
}

void VulkanRenderer::EndQuery(uint32_t currentBufferIdx)
{
#ifdef PIPELINE_STATS
	assert(currentBufferIdx < mDrawCommandBuffers.size());
	assert(mpQueryPool != nullptr);

	auto pCrrDrawCommandBuffer = mDrawCommandBuffers[currentBufferIdx];
	assert(pCrrDrawCommandBuffer != nullptr);

	mpQueryPool->EndQuery(pCrrDrawCommandBuffer->GetHandle());
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
	assert(mpScenePass != nullptr);

	assert(pCamera != nullptr);
	mpCamera = pCamera;

	mpScenePass->Update(this, mpRenderQueue, mpCamera, crrTime);

	/// pipeline stats per frame
	GetQueryResults();
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

void VulkanRenderer::DrawObject(ScenePass* pScenePass, const RenderQueue::Renderable* pRenderable, uint32_t currentBufferIdx)
{
	/*if (false == mIsPrepared)
		return;*/

	assert(pScenePass != nullptr);
	assert(pRenderable != nullptr);
	assert(currentBufferIdx < mDrawCommandBuffers.size());

	assert(mDescriptorSetDataCollection.empty() == false);
	assert(mPipelineDataCollection.empty() == false);

	auto pGeoNode = pRenderable->pGeometryNode;
	assert(pGeoNode != nullptr);

	if (false == pGeoNode->IsPassAllowed(pScenePass->GetPassType()))
	{
		LOG_DEBUG("Skip processing node: %s for pass: %s", pGeoNode->GetName().c_str(), ScenePass::PassTypeToStr(pScenePass->GetPassType()).c_str());
		return;
	}

	auto pCrrDrawCommandBuffer = mDrawCommandBuffers[currentBufferIdx];
	assert(pCrrDrawCommandBuffer != nullptr);

	auto* pVisComp = pGeoNode->GetComponent<VisualComponent>();
	assert(pVisComp != nullptr);

	auto it2 = mPipelineDataCollection.find(pScenePass->GetPassType());
	assert(it2 != mPipelineDataCollection.end());
	const auto& pipelineDataVector = it2->second;

	std::vector<PipelineData>::const_iterator cIt;
	for (cIt = pipelineDataVector.begin(); cIt != pipelineDataVector.end(); ++ cIt)
	{
		if (cIt->pVisualComponent == pVisComp)
			break;
	}
	const auto& pipelineData = *cIt;

	assert(pipelineData.pGraphicsPipeline != nullptr);
	assert(pipelineData.pPipelineLayout != nullptr);

	auto it3 = mDescriptorSetDataCollection.find(pScenePass->GetPassType());
	assert(it3 != mDescriptorSetDataCollection.end());
	const auto& descriptorSetDataVector = it3->second;

	std::vector<DescriptorSetData>::const_iterator cIt2;
	for (cIt2 = descriptorSetDataVector.begin(); cIt2 != descriptorSetDataVector.end(); ++ cIt2)
	{
		if (cIt2->pVisualComponent == pVisComp)
			break;
	}
	const auto& descriptorSetData = *cIt2;

	assert(descriptorSetData.pDescriptorSet != nullptr);

	// update dynamic states if any
	auto& dynamicState = pVisComp->GetDynamicState();

	if (dynamicState.HasStates())
	{
		UpdateDynamicStates(pScenePass, dynamicState, currentBufferIdx);
	}

	// bind textures if any
	if (pVisComp->HasTextures())
	{
		auto& textureMap = pVisComp->GetTextures();
		for (auto& iter : textureMap) // per shader stage
		{
			auto& textureArray = iter.second;
			for (auto* pTexture : textureArray) // per texture
			{
				Bind(pTexture);
			}
		}

	}

	//bind material
/*if (pRenderable->pMaterial)
{
	Bind(pRenderable->pMaterial, currentBufferIdx);
}*/

	//bind pipeline 
	// Bind the rendering pipeline
	// The pipeline (state object) contains all states of the rendering pipeline, binding it will set all the states specified at pipeline creation time
	vkCmdBindPipeline(pCrrDrawCommandBuffer->GetHandle(), VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineData.pGraphicsPipeline->GetHandle());

	// uniform -> descriptorsets bindings
	// Bind descriptor sets describing shader binding points
	vkCmdBindDescriptorSets(pCrrDrawCommandBuffer->GetHandle(), VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineData.pPipelineLayout->GetHandle(), descriptorSetData.pDescriptorSet->GetId(), 1, &descriptorSetData.pDescriptorSet->GetHandle(), 0, nullptr);

	// draw geometric primitives
	auto* pGeometry = pGeoNode->GetGeometry();
	assert(pGeometry != nullptr);
	//bind vertex format
	Bind(pGeometry->GetVertexFormat());
			
	// bind vertex buffer
	auto* pVertexBuffer = pGeometry->GetVertexBuffer();
	assert(pVertexBuffer != nullptr);

	Bind(pVertexBuffer, currentBufferIdx);

	// bind index buffer
	auto* pIndexBuffer = pGeometry->GetIndexBuffer();
	if (pGeometry->IsIndexed())
	{
		Bind(pIndexBuffer, currentBufferIdx);
	}

	uint32_t count = 0;
	if (pGeometry->IsIndexed())
	{
		if (pIndexBuffer)
			count = pIndexBuffer->GetIndexCount();
	}
	else
	{
		count = pVertexBuffer->GetVertexCount();
	}

	//TODO - improve instance count logic
	uint32_t instanceCount = (pVertexBuffer->GetVertexInputRate() == VertexBuffer::VertexInputRate::GE_VIR_VERTEX ? 1 : 0);
	bool isIndexedDrawing = pGeometry->IsIndexed();

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

void VulkanRenderer::BindLight(ScenePass* pScenePass, const LightNode* pLightNode, GeometryNode* pGeoNode)
{
	assert(pScenePass != nullptr);
	assert(pLightNode != nullptr);
	assert(pGeoNode != nullptr);

	if (false == pGeoNode->IsLit())
		return;

	auto* pVisComp = pGeoNode->GetComponent<VisualComponent>();
	assert(pVisComp != nullptr);

	auto* pLight = pLightNode->GetLight();
	assert(pLight != nullptr);

	switch (pScenePass->GetPassType())
	{
	case ScenePass::PassType::GE_PT_STANDARD:
	case ScenePass::PassType::GE_PT_OFFSCREEN:
	{
		if (pGeoNode->IsPassAllowed(pScenePass->GetPassType()))
		{
			auto* pUBO = pVisComp->GetUniformBuffer(pScenePass->GetPassType(), Shader::ShaderStage::GE_SS_FRAGMENT);
			assert(pUBO != nullptr);

			switch (pLight->GetLightType())
			{
			case Light::LightType::GE_LT_DIRECTIONAL:
			{
				pUBO->AddUniform(GLSLShaderTypes::UniformType::GE_UT_LIGHT_DIR, pLight->GetDirection());
				pUBO->AddUniform(GLSLShaderTypes::UniformType::GE_UT_LIGHT_COLOR, pLight->GetColor());
			}
			break;
			case Light::LightType::GE_LT_POINT:
			case Light::LightType::GE_LT_SPOT:
				//TODO
				break;
			case Light::LightType::GE_PT_COUNT:
			default:
				LOG_ERROR("Invalid light type!");
			}
		}
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
			BindLight(cIt->second, pLightNode, pGeoNode);
		}
	}
	break;
	default:
		LOG_ERROR("Invalid pass type!");
	}
}

//void VulkanRenderer::ComputeGraphicsResources(RenderQueue* pRenderQueue, RenderPass* pRenderPass)
void VulkanRenderer::ComputeGraphicsResources(RenderQueue* pRenderQueue, ScenePass* pScenePass)
{
	if (false == mIsPrepared)
		return;

	assert(pRenderQueue != nullptr);
	assert(mpScenePass != nullptr);
	assert(pScenePass != nullptr);

	mpRenderQueue = pRenderQueue;
	mpScenePass->Init(pScenePass);

	// TODO - compute resources for all renderable types
	auto renderableList = mpRenderQueue->GetRenderables(RenderQueue::RenderableType::GE_RT_OPAQUE);

	mpRenderQueue->ForEach(renderableList,
		[&, this](const RenderQueue::Renderable* pRenderable)
		{
			assert(pRenderable != nullptr);

			auto pGeoNode = pRenderable->pGeometryNode;
			assert(pGeoNode != nullptr);

			if (pGeoNode->IsLit())
			{
				pRenderQueue->ForEach([&, this](const LightNode* pLightNode)
					{
						BindLight(mpScenePass->GetScenePass(), pLightNode, pGeoNode);
					});
			}

			SetupPipelines(mpScenePass->GetScenePass(), pGeoNode);
		}
	);

	//TODO - cleanup
	mDescriptorSetBindingMapCollection.clear();
}

void VulkanRenderer::SetupPipelines(ScenePass* pScenePass, GeometryNode* pGeoNode)
{
	assert(pScenePass != nullptr);
	assert(pGeoNode != nullptr);

	switch (pScenePass->GetPassType())
	{
		case ScenePass::PassType::GE_PT_STANDARD:
		{
			SetupStandardPipeline(pScenePass, pGeoNode);
		}
		break;
		case ScenePass::PassType::GE_PT_OFFSCREEN:
		{
			SetupOffscreenPipeline(pScenePass, pGeoNode);
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
				SetupPipelines(cIt->second, pGeoNode);
			}
		}
			break;
		default:
			LOG_ERROR("Invalid pass type!");
	}
}

void VulkanRenderer::SetupStandardPipeline(ScenePass* pScenePass, GeometryNode* pGeoNode)
{
	assert(pScenePass != nullptr);
	assert(pGeoNode != nullptr);

	if (false == pGeoNode->IsPassAllowed(pScenePass->GetPassType()))
	{
		LOG_DEBUG("Skip processing node: %s for pass: %s", pGeoNode->GetName().c_str(), ScenePass::PassTypeToStr(pScenePass->GetPassType()).c_str());
		return;
	}

	assert(mpDevice != nullptr);
	assert(mpScenePass != nullptr);

	auto* pVisComp = pGeoNode->GetComponent<VisualComponent>();
	assert(pVisComp != nullptr);

	auto* pGeometry = pGeoNode->GetGeometry();
	assert(pGeometry != nullptr);

	ScenePass::PassType passType = pScenePass->GetPassType();


	// in case we have a model to load
	if (pGeometry->IsModel())
	{
		Model* pModel = dynamic_cast<Model*>(pGeometry);
		if (pModel)
		{
			Get(pModel);
		}
	}


	////  Shaders state
	std::vector<VkPipelineShaderStageCreateInfo> pipelineShaderStages;
	SetupShaderStage(pScenePass, pVisComp, pipelineShaderStages);

	//// Descriptor sets 
	SetupDescriptorSets(passType, pVisComp);

	//// Vertex input state
	VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo {};
	SetupVertexInputState(pVisComp, pGeometry, pipelineVertexInputStateCreateInfo);

	//// Input assembly state describes how primitives are assembled
	//// This pipeline will assemble vertex data as a triangle lists
	VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo {};
	SetupPrimitiveAssemblyState(pGeometry, pipelineInputAssemblyStateCreateInfo);

	//// Viewport state sets the number of viewports and scissor used in this pipeline
	//// Note: This is actually overriden by the dynamic states (see below)
	VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo {};
	SetupViewportScissorState(pVisComp, pipelineViewportStateCreateInfo);

	//// Rasterization state
	VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo {};
	SetupRasterizationState(pScenePass, pVisComp, pGeometry, pipelineRasterizationStateCreateInfo);

	// Multi sampling state
	// This example does not make use fo multi sampling (for anti-aliasing), the state must still be set and passed to the pipeline
	VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo {};
	SetupMultisampleState(pipelineMultisampleStateCreateInfo);

	//// Depth and stencil state containing depth and stencil compare and test operations
	//// We only use depth tests and want depth tests and writes to be enabled and compare with less or equal
	VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo {};
	SetupDepthStencilState(pVisComp, pipelineDepthStencilStateCreateInfo);

	// Color blend state describes how blend factors are calculated (if used)
	// We need one blend attachment state per color attachment (even if blending is not used)
	VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo {};
	SetupColorBlendState(pVisComp, pipelineColorBlendStateCreateInfo);

	// Enable dynamic states
	// Most states are baked into the pipeline, but there are still a few dynamic states that can be changed within a command buffer
	// To be able to change these we need do specify which dynamic states will be changed using this pipeline. Their actual states are set later on in the command buffer.
	// For this example we will set the viewport and scissor using dynamic states
	VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo {};
	SetupDynamicState(pVisComp, pipelineDynamicStateCreateInfo);

	///////////////////
	{
		PipelineData pipelineData{};
		pipelineData.pVisualComponent = pVisComp;

		auto it = mDescriptorSetDataCollection.find(pScenePass->GetPassType());
		const auto& descriptorSetDataVector = it->second;

		std::vector<DescriptorSetData>::const_iterator cIt;
		for (cIt = descriptorSetDataVector.begin(); cIt != descriptorSetDataVector.end(); ++cIt)
		{
			if (cIt->pVisualComponent == pVisComp)
				break;
		}
		const auto& descriptorSetData = *cIt;

		pipelineData.pPipelineLayout = GE_ALLOC(VulkanPipelineLayout)
			(
				mpDevice,
				{ descriptorSetData.pDescriptorSetLayout }, {}
		);
		assert(pipelineData.pPipelineLayout != nullptr);

		//// Assign the pipeline states to the pipeline creation info structure
		pipelineData.pGraphicsPipeline = GE_ALLOC(VulkanGraphicsPipeline)
			(
				mpDevice,
				mpPipelineCache,
				pipelineShaderStages,
				pipelineVertexInputStateCreateInfo, pipelineInputAssemblyStateCreateInfo, {},
				pipelineViewportStateCreateInfo, pipelineRasterizationStateCreateInfo,
				pipelineMultisampleStateCreateInfo, pipelineDepthStencilStateCreateInfo,
				pipelineColorBlendStateCreateInfo, pipelineDynamicStateCreateInfo,
				pipelineData.pPipelineLayout, mpScenePass->GetRenderPass(passType)
				);
		assert(pipelineData.pGraphicsPipeline != nullptr);

		mPipelineDataCollection[passType].push_back(pipelineData);
	}
}

void VulkanRenderer::SetupOffscreenPipeline(ScenePass* pScenePass, GeometryNode* pGeoNode)
{
	assert(pScenePass != nullptr);
	assert(pGeoNode != nullptr);

	if (false == pGeoNode->IsPassAllowed(pScenePass->GetPassType()))
	{
		LOG_DEBUG("Skip processing node: %s for pass: %s", pGeoNode->GetName().c_str(), ScenePass::PassTypeToStr(pScenePass->GetPassType()).c_str());
		return;
	}

	assert(mpDevice != nullptr);
	assert(mpScenePass != nullptr);

	auto* pVisComp = pGeoNode->GetComponent<VisualComponent>();
	assert(pVisComp != nullptr);

	auto* pGeometry = pGeoNode->GetGeometry();
	assert(pGeometry != nullptr);

	ScenePass::PassType passType = pScenePass->GetPassType();

	// in case we have a model to load
	if (pGeometry->IsModel())
	{
		Model* pModel = dynamic_cast<Model*>(pGeometry);
		if (pModel)
		{
			Get(pModel);
		}
	}


	////  Shaders state
	std::vector<VkPipelineShaderStageCreateInfo> pipelineShaderStages;
	SetupShaderStage(pScenePass, pVisComp, pipelineShaderStages);

	//// Descriptor sets 
	SetupDescriptorSets(passType, pVisComp);

	//// Vertex input state
	VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{};
	SetupVertexInputState(pVisComp, pGeometry, pipelineVertexInputStateCreateInfo);

	//// Input assembly state describes how primitives are assembled
	//// This pipeline will assemble vertex data as a triangle lists
	VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};
	SetupPrimitiveAssemblyState(pGeometry, pipelineInputAssemblyStateCreateInfo);

	//// Viewport state sets the number of viewports and scissor used in this pipeline
	//// Note: This is actually overriden by the dynamic states (see below)
	VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{};
	SetupViewportScissorState(pVisComp, pipelineViewportStateCreateInfo);

	//// Rasterization state
	VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{};
	SetupRasterizationState(pScenePass, pVisComp, pGeometry, pipelineRasterizationStateCreateInfo);

	// Multi sampling state
	// This example does not make use fo multi sampling (for anti-aliasing), the state must still be set and passed to the pipeline
	VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo{};
	SetupMultisampleState(pipelineMultisampleStateCreateInfo);

	//// Depth and stencil state containing depth and stencil compare and test operations
	//// We only use depth tests and want depth tests and writes to be enabled and compare with less or equal
	VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo{};
	SetupDepthStencilState(pVisComp, pipelineDepthStencilStateCreateInfo);

	// Color blend state describes how blend factors are calculated (if used)
	// We need one blend attachment state per color attachment (even if blending is not used)
	VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{};
	SetupColorBlendState(pVisComp, pipelineColorBlendStateCreateInfo);

	// Enable dynamic states
	// Most states are baked into the pipeline, but there are still a few dynamic states that can be changed within a command buffer
	// To be able to change these we need do specify which dynamic states will be changed using this pipeline. Their actual states are set later on in the command buffer.
	// For this example we will set the viewport and scissor using dynamic states
	VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
	SetupDynamicState(pVisComp, pipelineDynamicStateCreateInfo);

	///////////////////
	{
		PipelineData pipelineData{};
		pipelineData.pVisualComponent = pVisComp;

		auto it = mDescriptorSetDataCollection.find(pScenePass->GetPassType());
		const auto& descriptorSetDataVector = it->second;

		std::vector<DescriptorSetData>::const_iterator cIt;
		for (cIt = descriptorSetDataVector.begin(); cIt != descriptorSetDataVector.end(); ++cIt)
		{
			if (cIt->pVisualComponent == pVisComp)
				break;
		}
		const auto& descriptorSetData = *cIt;

		pipelineData.pPipelineLayout = GE_ALLOC(VulkanPipelineLayout)
			(
				mpDevice,
				{ descriptorSetData.pDescriptorSetLayout }, {}
		);
		assert(pipelineData.pPipelineLayout != nullptr);

		//// Assign the pipeline states to the pipeline creation info structure
		pipelineData.pGraphicsPipeline = GE_ALLOC(VulkanGraphicsPipeline)
			(
				mpDevice,
				mpPipelineCache,
				pipelineShaderStages,
				pipelineVertexInputStateCreateInfo, pipelineInputAssemblyStateCreateInfo, {},
				pipelineViewportStateCreateInfo, pipelineRasterizationStateCreateInfo,
				pipelineMultisampleStateCreateInfo, pipelineDepthStencilStateCreateInfo,
				pipelineColorBlendStateCreateInfo, pipelineDynamicStateCreateInfo,
				pipelineData.pPipelineLayout, mpScenePass->GetRenderPass(passType)
				);
		assert(pipelineData.pGraphicsPipeline != nullptr);

		mPipelineDataCollection[passType].push_back(pipelineData);
	}
}

void VulkanRenderer::SetupDescriptorSets(ScenePass::PassType passType, VisualComponent* pVisComp)
{
	assert(pVisComp != nullptr);

	auto iter = mDescriptorSetBindingMapCollection.find(passType);
	if (iter == mDescriptorSetBindingMapCollection.end()) //not found
	{
		LOG_ERROR("pVisComp not found in descriptor set binding map collection !");
		return;
	}
	auto& descriptorSetBindingMap = iter->second;

	//// setup Descriptor pool
	{
		std::vector<VkDescriptorPoolSize> poolSizes;
		for (auto iter = descriptorSetBindingMap.begin(); iter != descriptorSetBindingMap.end(); ++iter)
		{
			auto descriptorSetId = iter->first;
			uint32_t descriptorCount = static_cast<uint32_t>(iter->second.size());

			auto& setBindingDataVector = iter->second;

			for (auto& bindingData : setBindingDataVector)
			{
				// interested only in our vis comp
				if (bindingData.pVisualComponent != pVisComp)
				{
					continue;
				}

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

		auto& descPoolVector = mpDescriptorPoolMap[passType];

		DescriptorPoolData descriptorPoolData{};
		descriptorPoolData.pVisualComponent = pVisComp;
		descriptorPoolData.pDescriptorPool = GE_ALLOC(VulkanDescriptorPool)(mpDevice, descriptorSetMaxCount, poolSizes);
		assert(descriptorPoolData.pDescriptorPool != nullptr);

		descPoolVector.push_back(descriptorPoolData);
	}

	// setup Descriptor sets
	{
		for (auto iter = descriptorSetBindingMap.begin(); iter != descriptorSetBindingMap.end(); ++iter)
		{
			DescriptorSetData descriptorSetData{};
			descriptorSetData.pVisualComponent = pVisComp;

			auto& setId = iter->first;
			auto& setBindingDataVector = iter->second;

			// Basically connects the different shader stages to descriptors for binding uniform buffers, image samplers, etc.
			std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
			for (auto& bindingData : setBindingDataVector)
			{
				// interested only in our vis comp
				if (bindingData.pVisualComponent != pVisComp)
				{
					continue;
				}

				layoutBindings.push_back(bindingData.layoutBinding);
			}

			//TODO - for now we reuse the same descriptor set layout for all descriptor sets!
			descriptorSetData.pDescriptorSetLayout = GE_ALLOC(VulkanDescriptorSetLayout)(mpDevice, layoutBindings);
			assert(descriptorSetData.pDescriptorSetLayout != nullptr);

			// Allocate a new descriptor set from the selected descriptor pool
			std::vector<DescriptorPoolData>::const_iterator cIt;
			auto it = mpDescriptorPoolMap.find(passType);
			assert(it != mpDescriptorPoolMap.end());

			const auto& poolDataVector = it->second;
			for (cIt = poolDataVector.begin(); cIt != poolDataVector.end(); ++ cIt)
			{
				if (cIt->pVisualComponent == pVisComp)
					break;
			}
			auto* pPool = cIt->pDescriptorPool;
			assert(pPool != nullptr);

			descriptorSetData.pDescriptorSet = GE_ALLOC(VulkanDescriptorSet)(mpDevice, pPool, setId, { descriptorSetData.pDescriptorSetLayout });
			assert(descriptorSetData.pDescriptorSet != nullptr);

			std::vector<VkWriteDescriptorSet> writeDescriptorSets;
			for (auto& bindingData : setBindingDataVector)
			{
				// interested only in our vis comp
				if (bindingData.pVisualComponent != pVisComp)
				{
					continue;
				}

				bindingData.writeSet.dstSet = descriptorSetData.pDescriptorSet->GetHandle();

				writeDescriptorSets.push_back(bindingData.writeSet);
			}

			//TODO - for now only write descriptor sets
			descriptorSetData.pDescriptorSet->Update(writeDescriptorSets, {});

			auto& descriptorSetDataVector = mDescriptorSetDataCollection[passType];
			descriptorSetDataVector.push_back(descriptorSetData);
		}
	}
}

void VulkanRenderer::SetupShaderStage(ScenePass* pScenePass, VisualComponent* pVisComp, std::vector<VkPipelineShaderStageCreateInfo>& shaderStagesOut)
{
	assert(pScenePass != nullptr);
	assert(pVisComp != nullptr);
	assert(mpScenePass != nullptr);

	shaderStagesOut.clear();

	// setup Shaders
	const auto& shaders = pVisComp->GetShaders();

	// pipeline shader stages data
	shaderStagesOut.resize(shaders.size());

	size_t i = 0;
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
				if (pVisComp->HasUniformBuffers(pScenePass->GetPassType())) //get available one
				{
					pGadrUniformBuffer = Get(pVisComp->GetUniformBuffer(pScenePass->GetPassType(), pShader->GetShaderStage()));
				}
				else
				{
					//otherwise make one of our own
					//pVisComp->AddUniformBuffer();

				}
				assert(pGadrUniformBuffer != nullptr);

				// descriptor metadata
				VkDescriptorType descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

				AddWriteDescriptorSet(pScenePass->GetPassType(), pVisComp, shaderStage, pParser->GetUniformBlock().setId, pParser->GetUniformBlock().binding,
					descriptorType, nullptr, &(pGadrUniformBuffer->GetVKBuffer()->GetDescriptorInfo()));
			}

			// texture sampler(s) are present
			if (pParser->GetUniforms().empty() == false)
			{
				//TODO - for now we know its only samplers
				VkDescriptorType descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

				auto& uniforms = pParser->GetUniforms();

				if (pVisComp->HasTextures())
				{
					auto& textureMap = pVisComp->GetTextures();
					auto& textureArray = textureMap.at(pShader->GetShaderStage());

					assert(textureArray.size() == uniforms.size()); //TODO - improve this

					auto texIter = textureArray.begin();
					auto unifIter = uniforms.begin();
					for (; texIter != textureArray.end(); ++texIter, ++unifIter)
					{
						auto* pTexture = *texIter;

						if (pTexture->GetUsageType() == Texture::UsageType::GE_UT_RENDER)
						{
							auto* gadrTexture = Get(pTexture);
							assert(gadrTexture != nullptr);

							AddWriteDescriptorSet(pScenePass->GetPassType(), pVisComp, shaderStage, unifIter->second.setId, unifIter->second.binding,
								descriptorType, &(gadrTexture->GetDescriptorInfo()), nullptr);
						}
						else if (pTexture->GetUsageType() == Texture::UsageType::GE_UT_RENDER_TARGET)
						{
							const auto& passes = mpScenePass->GetScenePass()->GetPasses();

							bool_t foundRT = false;
							ScenePass::ScenePassMap::const_iterator cIt;
							ScenePass::RenderTargetMap::const_iterator cIt2;
							for (cIt = passes.begin(); cIt != passes.end(); ++ cIt)
							{
								const auto& RTs = cIt->second->GetRenderTargets();
								for (cIt2 = RTs.begin(); cIt2 != RTs.end(); ++ cIt2)
								{
									if (cIt2->second->GetTexture() == pTexture)
									{
										foundRT = true;
										break;
									}
								}

								if (foundRT)
									break;
							}

							if (foundRT)
							{
								auto* pPass = cIt->second;
								auto* pRT = cIt2->second;

								auto* gadrTexture = Get(pRT);
								assert(gadrTexture != nullptr);

								AddWriteDescriptorSet(pScenePass->GetPassType(), pVisComp, shaderStage, unifIter->second.setId, unifIter->second.binding,
									descriptorType, &(gadrTexture->GetDescriptorInfo()), nullptr);
							}
						}
					}
				}
			}

			// pipeline shader state setup
			auto GADRShader = Get(pShader);
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
}

void VulkanRenderer::AddWriteDescriptorSet(ScenePass::PassType passType, VisualComponent* pVisComp, VkShaderStageFlagBits shaderStage, uint32_t setId, uint32_t binding, VkDescriptorType descriptorType,
	const VkDescriptorImageInfo* pDescriptorImageInfo, const VkDescriptorBufferInfo* pDescriptorBufferInfo)
{
	assert(pVisComp != nullptr);

	DescriptorSetBindingData descriptorSetBindingData{};
	descriptorSetBindingData.pVisualComponent = pVisComp;

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


	auto& descriptorSetBindingMap = mDescriptorSetBindingMapCollection[passType];
	descriptorSetBindingMap[setId].push_back(descriptorSetBindingData);
}

void VulkanRenderer::SetupVertexInputState(VisualComponent* pVisComp, GeometricPrimitive* pGeometry, VkPipelineVertexInputStateCreateInfo& pipelineVertexInputStateCreateInfoOut)
{
	assert(pVisComp != nullptr);
	assert(pGeometry != nullptr);

	// setup vertex input
	GADRVertexFormat* gadrVertexFormat = Get(pGeometry->GetVertexFormat());
	assert(gadrVertexFormat != nullptr);

	Shader* pVertexShader = pVisComp->GetShader(Shader::ShaderStage::GE_SS_VERTEX);
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
	auto shaderAttIter = shaderAttribs.begin();
	for (auto shaderAttIter = shaderAttribs.begin(); shaderAttIter != shaderAttribs.end(); ++shaderAttIter)
	{
		auto& refAtt = updatedShaderAttribs[index];
		refAtt = inputAttribs[shaderAttIter->first];
		auto refInput = shaderAttIter->second.pInput;
		if (refInput)
			refAtt.location = refInput->location;

		index++;
	}


	GADRVertexBuffer* gadrVertexBuffer = Get(pGeometry->GetVertexBuffer());
	assert(gadrVertexBuffer != nullptr);

	const auto& inputBinding = gadrVertexBuffer->GetVkInputBinding();

	// TODO - improve resource creation - get it first time to create the Vulkan Index Buffer
	if (pGeometry->IsIndexed())
		Get(pGeometry->GetIndexBuffer());

	/////////////////////////////////
	pipelineVertexInputStateCreateInfoOut =
		VulkanInitializers::PipelineVertexInputStateCreateInfo(1, &inputBinding, static_cast<uint32_t>(updatedShaderAttribs.size()), updatedShaderAttribs.data());
}

void VulkanRenderer::SetupPrimitiveAssemblyState(GeometricPrimitive* pGeometry, VkPipelineInputAssemblyStateCreateInfo& pipelineInputAssemblyStateCreateInfoOut)
{
	assert(pGeometry != nullptr);

	// Input assembly state describes how primitives are assembled
	// This pipeline will assemble vertex data as a triangle lists
	VkPrimitiveTopology vulkanTopology = VulkanUtils::PrimitiveTopologyToVulkanTopolgy(pGeometry->GetTopology());

	auto restartPrimitve = (pGeometry->IsIndexed() ? pGeometry->GetIndexBuffer()->GetIsRestartPrimitive() : false);

	pipelineInputAssemblyStateCreateInfoOut  =
		VulkanInitializers::PipelineInputAssemblyStateCreateInfo(vulkanTopology, restartPrimitve);
}

void VulkanRenderer::SetupViewportScissorState(VisualComponent* pVisComp, VkPipelineViewportStateCreateInfo& pipelineViewportStateCreateInfoOut)
{
	assert(pVisComp != nullptr);

	// viewport & scicors state
	auto& dynamicState = pVisComp->GetDynamicState();

	// Viewport state sets the number of viewports and scissor used in this pipeline
	// Note: This is actually overriden by the dynamic states (see below)

	// if viewport and scisosrs are part of dynamic state, we don't handlle them here!
	if (dynamicState.HasStates())
	{
		pipelineViewportStateCreateInfoOut = VulkanInitializers::PipelineViewportStateCreateInfo(MIN_NUM_VIEWPORTS, nullptr, MIN_NUM_SCISSORS, nullptr);
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
		scissor.offset = VkOffset2D{ 0, 0 };
		scissor.extent = VkExtent2D{ GetWindowWidth(), GetWindowHeight() };

		pipelineViewportStateCreateInfoOut = VulkanInitializers::PipelineViewportStateCreateInfo(1, &viewport, 1, &scissor);
	}
}

void VulkanRenderer::SetupRasterizationState(ScenePass* pScenePass, VisualComponent* pVisComp, GeometricPrimitive* pGeometry, VkPipelineRasterizationStateCreateInfo& pipelineRasterizationStateCreateInfoOut)
{
	assert(pScenePass != nullptr);
	assert(pVisComp != nullptr);
	assert(pGeometry != nullptr);

	CullFaceState cfs;
	const auto& effect = pScenePass->GetVisualEffect();
	if (effect.isEnabled)
	{
		cfs = effect.cullFaceState;
	}
	else
	{
		cfs = pVisComp->GetCullFaceState();
	}

	// Rasterization state
	VkPolygonMode vulkanPolygonMode = VulkanUtils::PrimitivePolygonModeToVulkanPolygonMode(pGeometry->GetPolygonMode());
	VkFrontFace vulkanFaceWinding = VulkanUtils::PrimitiveFaceWindingToVulkanFaceWinding(pGeometry->GetFaceWinding());
	VkCullModeFlagBits vulkanCullMode = VulkanUtils::FaceCullModeToVulkanFaceCullMode(cfs.GetCullMode());

	pipelineRasterizationStateCreateInfoOut =
		VulkanInitializers::PipelineRasterizationStateCreateInfo(VK_FALSE, VK_FALSE, vulkanPolygonMode, vulkanCullMode,
			vulkanFaceWinding, VK_FALSE, 0, 0, 0, RASTER_MIN_LINE_WIDTH);

}

void VulkanRenderer::SetupMultisampleState(VkPipelineMultisampleStateCreateInfo& pipelineMultisampleStateCreateInfoOut)
{
	// Multi sampling state
	// This example does not make use fo multi sampling (for anti-aliasing), the state must still be set and passed to the pipeline
	pipelineMultisampleStateCreateInfoOut =
		VulkanInitializers::PipelineMultisampleStateCreateInfo(MIN_NUM_SAMPLES, VK_FALSE, 0.0f, nullptr, VK_FALSE, VK_FALSE);
}

void VulkanRenderer::SetupDepthStencilState(VisualComponent* pVisComp, VkPipelineDepthStencilStateCreateInfo& pipelineDepthStencilStateCreateInfoOut)
{
	assert(pVisComp != nullptr);

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
	pipelineDepthStencilStateCreateInfoOut =
		VulkanInitializers::PipelineDepthStencilStateCreateInfo(depthEnabled, depthEnabled && depthWritable, depthCompareOp, VK_FALSE, stencilEnabled, stencilOpState, stencilOpState, 0, 0);
}

void VulkanRenderer::SetupColorBlendState(VisualComponent* pVisComp, VkPipelineColorBlendStateCreateInfo& pipelineColorBlendStateCreateInfoOut)
{
	assert(pVisComp != nullptr);

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

	pipelineColorBlendStateCreateInfoOut =
		VulkanInitializers::PipelineColorBlendStateCreateInfo(VK_FALSE, VkLogicOp::VK_LOGIC_OP_NO_OP, 1, &pipelineColorBlendAttachmentState, &constantColor[0]);
}

void VulkanRenderer::SetupDynamicState(VisualComponent* pVisComp, VkPipelineDynamicStateCreateInfo& pipelineDynamicStateCreateInfoOut)
{
	assert(pVisComp != nullptr);

	std::vector<VkDynamicState> vulkanDynamicStates;

	auto& dynamicState = pVisComp->GetDynamicState();

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

void VulkanRenderer::UpdateUniformBuffers(ScenePass* pScenePass, const RenderQueue::Renderable* pRenderable, Camera* pCamera, float32_t crrTime)
{
	assert(pScenePass != nullptr);
	assert(pRenderable != nullptr);

	auto* pGeoNode = pRenderable->pGeometryNode;
	assert(pGeoNode != nullptr);
	assert(pCamera != nullptr);

	if (false == pGeoNode->IsPassAllowed(pScenePass->GetPassType()))
	{
		LOG_DEBUG("Skip processing node: %s for pass: %s", pGeoNode->GetName().c_str(), ScenePass::PassTypeToStr(pScenePass->GetPassType()).c_str());
		return;
	}

	auto* pVisComp = pGeoNode->GetComponent<VisualComponent>();
	assert(pVisComp != nullptr);

	const auto& effect = pScenePass->GetVisualEffect();

	auto shaders = pVisComp->GetShaders();
	for (auto iter = shaders.begin(); iter != shaders.end(); ++iter)
	{
		auto shaderStage = iter->first;
		auto uniformBuffer = pVisComp->GetUniformBuffer(pScenePass->GetPassType(), shaderStage);

		if (uniformBuffer)
		{
			// spacial case for cubemaps - environment mapping
			if (uniformBuffer->HasUniform(GLSLShaderTypes::UniformType::GE_UT_PV_CUBEMAP_MATRIX4))
			{
				glm::mat4 PV = pCamera->GetProjectionViewMatrix();

				// we remove the translation transform to allow the cubemap to never change position in world space
				PV[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

				uniformBuffer->SetUniform(GLSLShaderTypes::UniformType::GE_UT_PVM_MATRIX4, PV * pGeoNode->GetModelMatrix());
			}
			else if (uniformBuffer->HasUniform(GLSLShaderTypes::UniformType::GE_UT_PVM_MATRIX4))
			{
				glm::mat4 PVM = pCamera->GetProjectionViewMatrix();
				//TODO - improve effect support
				if (effect.isEnabled)
				{
					PVM *= effect.transform * pGeoNode->GetModelMatrix();
				}
				else
				{
					PVM *= pGeoNode->GetModelMatrix();
				}
				uniformBuffer->SetUniform(GLSLShaderTypes::UniformType::GE_UT_PVM_MATRIX4, PVM);
			}

			if (uniformBuffer->HasUniform(GLSLShaderTypes::UniformType::GE_UT_MODEL_MATRIX4))
			{
				uniformBuffer->SetUniform(GLSLShaderTypes::UniformType::GE_UT_MODEL_MATRIX4, pGeoNode->GetModelMatrix());
			}

			if (uniformBuffer->HasUniform(GLSLShaderTypes::UniformType::GE_UT_NORMAL_MATRIX4))
			{
				uniformBuffer->SetUniform(GLSLShaderTypes::UniformType::GE_UT_NORMAL_MATRIX4, pGeoNode->GetNormalMatrix());
			}

			if (uniformBuffer->HasUniform(GLSLShaderTypes::UniformType::GE_UT_NORMAL_MATRIX3))
			{
				uniformBuffer->SetUniform(GLSLShaderTypes::UniformType::GE_UT_NORMAL_MATRIX3, glm::mat3(pGeoNode->GetNormalMatrix()));
			}

			if (uniformBuffer->HasUniform(GLSLShaderTypes::UniformType::GE_UT_CAMERA_POS))
			{
				uniformBuffer->SetUniform(GLSLShaderTypes::UniformType::GE_UT_CAMERA_POS, pCamera->GetPosition());
			}

			if (uniformBuffer->HasUniform(GLSLShaderTypes::UniformType::GE_UT_CRR_TIME))
			{
				uniformBuffer->SetUniform(GLSLShaderTypes::UniformType::GE_UT_CRR_TIME, crrTime);
			}

			Bind(uniformBuffer);
		}
	}
}

void VulkanRenderer::UpdateDynamicStates(ScenePass* pPass, const DynamicState& dynamicState, uint32_t currentBufferIdx)
{
	assert(pPass != nullptr);
	assert(currentBufferIdx < mDrawCommandBuffers.size());
	auto pCrrDrawCommandBuffer = mDrawCommandBuffers[currentBufferIdx];
	assert(pCrrDrawCommandBuffer != nullptr);

	VkExtent2D extent;
	switch (pPass->GetPassType())
	{
		case ScenePass::PassType::GE_PT_STANDARD:
		{
			extent.width = GetWindowWidth();
			extent.height = GetWindowHeight();
		}
		break;
		case ScenePass::PassType::GE_PT_OFFSCREEN:
		{
			auto* pColorRT = pPass->GetRenderTarget(RenderTarget::TargetType::GE_TT_COLOR);
			extent.width = pColorRT->GetWidth();
			extent.height = pColorRT->GetHeight();
		}
		break;
		case ScenePass::PassType::GE_PT_SHADOWS:
			//TODO
			break;
	}

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
			viewport.width = static_cast<float32_t>(extent.width);
			viewport.height = static_cast<float32_t>(extent.height);
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			vkCmdSetViewport(pCrrDrawCommandBuffer->GetHandle(), 0, 1, &viewport);
		}
		break;
		case VkDynamicState::VK_DYNAMIC_STATE_SCISSOR:
		{
			VkRect2D scissor{};
			scissor.offset = VkOffset2D{ 0, 0 };
			scissor.extent = extent;

			vkCmdSetScissor(pCrrDrawCommandBuffer->GetHandle(), 0, 1, &scissor);
		}
		break;

		//TODO other cases

		default:
			LOG_ERROR("Invalid dynamic state!");
		}
	}
}

void VulkanRenderer::DrawSceneToCommandBuffer()
{
	assert(mpScenePass != nullptr);
	assert(mpRenderQueue != nullptr);

	// THE ACTUAL RENDERING IS DONE HERE !!!!

	for (uint32_t i = 0; i < mDrawCommandBuffers.size(); ++i)
	{
		assert(mDrawCommandBuffers[i] != nullptr);

		// Begin command buffer recording
		VK_CHECK_RESULT(mDrawCommandBuffers[i]->Begin());

		ResetQuery(i);

		BeginQuery(i);

		mpScenePass->Render(this, mpRenderQueue, i);

		EndQuery(i);

		// End command buffer recording
		VK_CHECK_RESULT(mDrawCommandBuffers[i]->End());
	}
}

void VulkanRenderer::DrawDirect(uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount, uint32_t currentBufferIdx, bool_t isIndexedDrawing)
{
	assert(currentBufferIdx < mDrawCommandBuffers.size());

	auto pCrrDrawCommandBuffer = mDrawCommandBuffers[currentBufferIdx];
	assert(pCrrDrawCommandBuffer != nullptr);

	if (isIndexedDrawing)
	{
		// TODO - for now firstVertex/Index, firstInstance, vertexOffset are all 0
		vkCmdDrawIndexed(pCrrDrawCommandBuffer->GetHandle(), indexCount, instanceCount, firstIndex, 0, 0);
	}
	else
	{
		// TODO - for now firstVertex/Index, firstInstance are all 0
		vkCmdDraw(pCrrDrawCommandBuffer->GetHandle(), indexCount, instanceCount, 0, 0);
	}
}

void VulkanRenderer::DrawIndirect(uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount, uint32_t currentBufferIdx, bool_t isIndexedDrawing)
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

VulkanCommandPool* VulkanRenderer::GetCommandPool() const
{
	return mpCommandPool;
}

VulkanCommandBuffer* VulkanRenderer::GetCommandBuffer(uint32_t currentBufferIdx) const
{
	assert(currentBufferIdx < mDrawCommandBuffers.size());

	return mDrawCommandBuffers[currentBufferIdx];
}