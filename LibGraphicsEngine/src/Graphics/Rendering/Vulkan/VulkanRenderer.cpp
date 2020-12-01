#include "VulkanRenderer.hpp"

#include "Foundation/Platform/Platform.hpp"

#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"
#include "Graphics/Rendering/RenderQueue.hpp"
#include "Graphics/Rendering/RenderPasses/RenderPass.hpp"
#include "Graphics/SceneGraph/GeometryNode.hpp"
#include "Graphics/GeometricPrimitives/GeometricPrimitive.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Rendering/Resources/Material.hpp"
#include "Graphics/Rendering/Resources/UniformBuffer.hpp"

// Resources
#include "Resources/VulkanVertexFormat.hpp"
#include "Resources/VulkanVertexBuffer.hpp"
#include "Resources/VulkanIndexBuffer.hpp"
#include "Resources/VulkanShader.hpp"
#include "Resources/VulkanTexture.hpp"
#include "Resources/VulkanMaterial.hpp"
#include "Resources/VulkanUniformBuffer.hpp"

#include "Internal/VulkanCommon.hpp"
#include "Internal/VulkanPassThroughAllocator.hpp"
#include "Internal/VulkanPoolAllocator.hpp"
#include "Internal/VulkanDevice.hpp"
#include "Internal/VulkanQueue.hpp"
#include "Internal/VulkanBuffer.hpp"
#include "Internal/VulkanCommandBuffer.hpp"
#include "Internal/VulkanFrameBuffer.hpp"
#include "Internal/VulkanFrameBufferAttachment.hpp"
#include "Internal/VulkanRenderPass.hpp"
#include "Internal/VulkanRenderPassAttachment.hpp"
#include "Internal/VulkanSubPass.hpp"
#include "Internal/VulkanSemaphore.hpp"
#include "Internal/VulkanFence.hpp"
#include "Internal/VulkanShaderModule.hpp"
#include "Internal/VulkanDescriptorPool.hpp"
#include "Internal/VulkanDescriptorSetLayout.hpp"
#include "Internal/VulkanDescriptorSet.hpp"
#include "Internal/VulkanPipelineCache.hpp"
#include "Internal/VulkanPipelineLayout.hpp"
#include "Internal/VulkanGraphicsPipeline.hpp"
#include "Internal/VulkanQueryPool.hpp"
#include "Internal/VulkanInitializers.hpp"
#include "Internal/VulkanHelpers.hpp"
#include "Internal/VulkanDebug.hpp"

#ifndef NEW_GRAPHICS
#include <fstream>
#include <iostream>
#include <array>
#include <cassert>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/trigonometric.hpp"


/* Number of viewports and number of scissors have to be the same */
/* at pipeline creation and in any call to set them dynamically   */
/* They also have to be the same as each other                    */
#define NUM_VIEWPORTS 1
#define NUM_SCISSORS NUM_VIEWPORTS

#define VERTEX_STAGE 0
#define FRAGMENT_STAGE 1
#define SHADER_COUNT 2


#define TRIANGLE
//#define CUBE

#if defined(TRIANGLE)
#define USE_INDEX_BUFFER
#undef CUBE
#endif

#if defined(CUBE)
#undef TRIANGLE
#endif

#endif // NEW_GRAPHICS

/* Number of descriptor sets needs to be the same at alloc,       */
/* pipeline layout creation, and descriptor set layout creation   */
#define NUM_DESCRIPTOR_SETS 1

/* Number of samples needs to be the same at image creation,      */
/* renderpass creation and pipeline creation.                     */
#define NUM_SAMPLES VK_SAMPLE_COUNT_1_BIT

#define SUBPASS_ID 0

#define COLOR_ATT 0
#define DEPTH_ATT 1
#define ATT_COUNT 2

#define PIPELINE_STATS


using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanRenderer::VulkanRenderer()
	: Renderer(RendererType::FORWARD)
	, mpDevice(nullptr)
	, mpDepthStencil(nullptr)
	, mpDefaultRenderPass(nullptr)
	, mpRenderCompleteSemaphore(nullptr)
	, mpPresentCompleteSemaphore(nullptr)
	, mCurrentBufferIdx(0) 
	, mSubmitInfo{}
	, mpPipelineCache(nullptr)
	, mpQueryPool(nullptr)
#ifndef NEW_GRAPHICS
	, mpDescriptorPool(nullptr)
	, mpDescriptorSet(nullptr)
	, mpDescriptorSetLayout(nullptr)
	, mpPipelineLayout(nullptr)
	, mpGraphicsPipeline(nullptr)
	, mpVertices(nullptr)
	, mpIndices(nullptr)
	, mIndicesCount(0)
	, mpUniformBufferVS(nullptr)
#endif
{}


VulkanRenderer::~VulkanRenderer()
{
	Terminate();
}

void VulkanRenderer::Init(Platform::GE_Window* pWindow)
{
	Renderer::Init(pWindow);

	mpDevice = GE_ALLOC(VulkanDevice)(pWindow, VULKAN_DEBUG);

	Prepare();
}

void VulkanRenderer::Terminate()
{
	// wait for the device to finish ongoing operations on all owned queues
	mpDevice->WaitIdle();


#ifdef _DEBUG
	LOG_INFO("[VulkanMemoryAllocator] AllocationCount: %u", mpDevice->GetAllocator()->GetAllocationCount());
#endif

	Renderer::Terminate();

#ifdef PIPELINE_STATS
	GE_FREE(mpQueryPool);
#endif

	// draw command buffers
	for (auto& commandBufferRef : mDrawCommandBuffers)
	{
		GE_FREE(commandBufferRef);
	}

	GE_FREE(mpPipelineCache);

#ifndef NEW_GRAPHICS
	// pipeline objects
	GE_FREE(mpPipelineLayout);
	GE_FREE(mpGraphicsPipeline);

	// descriptor set layout
	GE_FREE(mpDescriptorSetLayout);

	// descriptor set is invalidated and destroyed with the descriptor pool
	GE_FREE(mpDescriptorSet);

	// descriptor pool
	GE_FREE(mpDescriptorPool);


	// uniform buffers
	GE_FREE(mpUniformBufferVS);

	// vertex buffers
	GE_FREE(mpVertices);

#if defined(USE_INDEX_BUFFER)
	// indices buffers
	GE_FREE(mpIndices);
#endif // USE_INDEX_BUFFER

#endif /////

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

	GE_FREE(mpDepthStencil);


	GE_FREE(mpDevice);
}


void VulkanRenderer::Prepare()
{
	////////// GENERAL RESOURCES //////////

	setupDefaultDepthStencil();

	setupDefaultRenderPass();

	setupDefaultFrameBuffer();

	setupDrawCommandBuffers();

	setupSynchronizationPrimitives();

	setupPipelineCache();

	setupSubmitInfo();

	setupPipelineStats();

	//////// SCENE DEPENDENT ///////////
#ifndef NEW_GRAPHICS
	setupScene();
#endif

	mIsPrepared = true;
}

void VulkanRenderer::WindowResize(uint32_t width, uint32_t height)
{
	if (false == mIsPrepared)
		return;

	mIsPrepared = false;

	if (width > 0)
		mWindowWidth = width;

	if (height > 0)
		mWindowHeight = height;

	// Ensure all operations on the device have been finished before destroying resources
	mpDevice->WaitIdle();

	// Recreate swap chain
	mpDevice->ResetSwapChain();


	GE_FREE(mpDepthStencil);

	setupDefaultDepthStencil();

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

#ifdef NEW_GRAPHICS
	DrawSceneToCommandBuffer();
#else
	buildCommandBuffers();
#endif

	mpDevice->WaitIdle();

#ifndef NEW_GRAPHICS
	// notify derived class
	viewChanged(); //TODO - to remove
#endif 

	mIsPrepared = true;
}


void VulkanRenderer::setupDefaultDepthStencil()
{
	if (mpDepthStencil)
	{
		GE_FREE(mpDepthStencil);
	}

	mpDepthStencil = GE_ALLOC(VulkanFrameBufferAttachment)(mpDevice, mpDevice->GetDepthFormat(), 
									VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
															mWindowWidth, mWindowHeight, VulkanFrameBufferAttachment::Type::DEPTH_STENCIL);
}

void VulkanRenderer::setupDefaultRenderPass()
{
	// attachments
	std::vector<VulkanRenderPassAttachment*> renderPassAttachments;
	renderPassAttachments.resize(ATT_COUNT);

	// For this color attachemnt we use VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL as final layout, because this attachment is used as present image to the graphics queue!
	renderPassAttachments[COLOR_ATT] = GE_ALLOC(VulkanRenderPassAttachment)
											(mpDevice->GetSurfaceFormat().format, NUM_SAMPLES, VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR, 
											VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE, VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE,
											VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE, VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED, 
											VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VulkanRenderPassAttachment::Type::COLOR);

	renderPassAttachments[DEPTH_ATT] = GE_ALLOC(VulkanRenderPassAttachment)
											(mpDevice->GetDepthFormat(), NUM_SAMPLES, VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR,
												VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE, VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR, 
												VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE, VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
												VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
												VulkanRenderPassAttachment::Type::DEPTH_STENCIL);

	std::vector<VkAttachmentReference> colorReferences;
	colorReferences.push_back(VulkanInitializers::AttachmentReference(COLOR_ATT, VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL));
	VkAttachmentReference depthStencilReference = VulkanInitializers::AttachmentReference(DEPTH_ATT, VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	// subPasses
	std::vector<VulkanSubPass*> subPasses;
	subPasses.push_back(GE_ALLOC(VulkanSubPass)(colorReferences, depthStencilReference, {}, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, SUBPASS_ID));

	// SubPass dependencies for layout transitions
	std::vector<VkSubpassDependency> subPassDependencies;
	subPassDependencies.push_back(VulkanInitializers::SubpassDependency(
									VK_SUBPASS_EXTERNAL, SUBPASS_ID, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
									VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
									VkAccessFlagBits::VK_ACCESS_MEMORY_READ_BIT, 
									VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
									VkDependencyFlagBits::VK_DEPENDENCY_BY_REGION_BIT));

	subPassDependencies.push_back(VulkanInitializers::SubpassDependency(
									SUBPASS_ID, VK_SUBPASS_EXTERNAL, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
									VkPipelineStageFlagBits::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
									VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, 
									VkAccessFlagBits::VK_ACCESS_MEMORY_READ_BIT, VkDependencyFlagBits::VK_DEPENDENCY_BY_REGION_BIT));


	mpDefaultRenderPass = GE_ALLOC(VulkanRenderPass)(mpDevice, renderPassAttachments, subPasses, subPassDependencies);

	// cleanup
	for (auto& att : renderPassAttachments)
	{
		GE_FREE(att);
	}

	for (auto& subpass : subPasses)
	{
		GE_FREE(subpass);
	}
}

void VulkanRenderer::setupDefaultFrameBuffer()
{
	if (mDefaultFrameBuffers.size() > 0)
	{
		for (auto& fb : mDefaultFrameBuffers)
		{
			GE_FREE(fb);
		}
	}

	std::vector<VulkanFrameBufferAttachment*> frameBufferAttachments;
	frameBufferAttachments.resize(ATT_COUNT);

	frameBufferAttachments[DEPTH_ATT] = mpDepthStencil;

	mDefaultFrameBuffers.resize(mpDevice->GetSwapChainImageCount());
	for (uint32_t i = 0; i < mDefaultFrameBuffers.size(); ++i)
	{
		frameBufferAttachments[COLOR_ATT] = mpDevice->GetSwapChainBuffers()[i];

		mDefaultFrameBuffers[i] = GE_ALLOC(VulkanFrameBuffer)(mpDevice, mpDefaultRenderPass, frameBufferAttachments, mWindowWidth, mWindowHeight);
	}
}

void VulkanRenderer::setupDrawCommandBuffers()
{
	if (mDrawCommandBuffers.size() > 0)
	{
		for (auto& commandBufferRef : mDrawCommandBuffers)
		{
			GE_FREE(commandBufferRef);
		}
	}

	// Create one command buffer for each swap chain image and reuse for rendering
	mDrawCommandBuffers.resize(mpDevice->GetSwapChainImageCount());

	for (auto& commandBufferRef : mDrawCommandBuffers)
	{
		commandBufferRef = GE_ALLOC(VulkanCommandBuffer)(mpDevice, VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		assert(commandBufferRef != nullptr);
	}
}

void VulkanRenderer::setupSynchronizationPrimitives()
{
	// Semaphores (Used for correct command ordering)
	//// Semaphore used to ensures that all commands submitted have been finished before submitting the image to the queue
	mpRenderCompleteSemaphore = GE_ALLOC(VulkanSemaphore)(mpDevice);
	//// Semaphore used to ensures that image presentation is complete before starting to submit again
	mpPresentCompleteSemaphore = GE_ALLOC(VulkanSemaphore)(mpDevice);


	// Fences (Used to check draw command buffer completion)
	// Create in signaled state so we don't wait on first render of each command buffer
	mWaitFences.resize(mDrawCommandBuffers.size());
	for (auto& fence : mWaitFences)
	{
		fence = GE_ALLOC(VulkanFence)(mpDevice, VkFenceCreateFlagBits::VK_FENCE_CREATE_SIGNALED_BIT);
	}
}

void VulkanRenderer::setupPipelineCache()
{
	mpPipelineCache = GE_ALLOC(VulkanPipelineCache)(mpDevice, {});
}

#ifndef NEW_GRAPHICS
void VulkanRenderer::setupScene()
{
	setupVertexBuffers();

	setupUniformBuffers();

	setupShaders();

	setupTextures();


	setupDescriptorPool();

	setupDescriptorSet();

	setupPipeline();

	buildCommandBuffers();

}


void VulkanRenderer::setupVertexBuffers()
{
	// Prepare vertex and index buffers for an indexed triangle
	// Also uploads them to device local memory using staging and initializes vertex input and attribute binding to match the vertex shader

	// A note on memory management in Vulkan in general:
	//	This is a very complex topic and while it's fine for an example application to do small individual memory allocations that is not
	//	what should be done a real-world application, where you should allocate large chunkgs of memory at once instead.

#if defined(TRIANGLE)
	// Setup vertices
	std::vector<Vertex> vertexBuffer =
	{
		// the clip space coordinates are defined as expected by Vulkan viewport - Y points down
		// coordinates defined in clock-wise winding

		// clip space position, color
		{ {  0.0f,  1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
		{ {  1.0f, -1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
		{ { -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } }
	};

	uint32_t vertexBufferSize = static_cast<uint32_t>(vertexBuffer.size()) * sizeof(Vertex);

	// Setup indices
	std::vector<uint32_t> indexBuffer = { 0, 1, 2 };

	//mIndices.count = static_cast<uint32_t>(indexBuffer.size());
	mIndicesCount = static_cast<uint32_t>(indexBuffer.size());
	uint32_t indexBufferSize = mIndicesCount * sizeof(uint32_t);

#elif defined(CUBE)
#define XYZ1(_x_, _y_, _z_) (_x_), (_y_), (_z_)//, 1.f
#define UV(_u_, _v_) (_u_), (_v_)

	// Setup vertices
	std::vector<Vertex> vertexBuffer =
	{
		// clip coordinates defined in clockwise winding

		// red - front face
		{XYZ1(-1, -1, 1), XYZ1(1.f, 0.f, 0.f)},
		{XYZ1(-1, 1, 1), XYZ1(1.f, 0.f, 0.f)},
		{XYZ1(1, -1, 1), XYZ1(1.f, 0.f, 0.f)},
		{XYZ1(1, -1, 1), XYZ1(1.f, 0.f, 0.f)},
		{XYZ1(-1, 1, 1), XYZ1(1.f, 0.f, 0.f)},
		{XYZ1(1, 1, 1), XYZ1(1.f, 0.f, 0.f)},
		// green - back face
		{XYZ1(-1, -1, -1), XYZ1(0.f, 1.f, 0.f)},
		{XYZ1(1, -1, -1), XYZ1(0.f, 1.f, 0.f)},
		{XYZ1(-1, 1, -1), XYZ1(0.f, 1.f, 0.f)},
		{XYZ1(-1, 1, -1), XYZ1(0.f, 1.f, 0.f)},
		{XYZ1(1, -1, -1), XYZ1(0.f, 1.f, 0.f)},
		{XYZ1(1, 1, -1), XYZ1(0.f, 1.f, 0.f)},
		// blue - left face
		{XYZ1(-1, 1, 1), XYZ1(0.f, 0.f, 1.f)},
		{XYZ1(-1, -1, 1), XYZ1(0.f, 0.f, 1.f)},
		{XYZ1(-1, 1, -1), XYZ1(0.f, 0.f, 1.f)},
		{XYZ1(-1, 1, -1), XYZ1(0.f, 0.f, 1.f)},
		{XYZ1(-1, -1, 1), XYZ1(0.f, 0.f, 1.f)},
		{XYZ1(-1, -1, -1), XYZ1(0.f, 0.f, 1.f)},
		// yellow - right face
		{XYZ1(1, 1, 1), XYZ1(1.f, 1.f, 0.f)},
		{XYZ1(1, 1, -1), XYZ1(1.f, 1.f, 0.f)},
		{XYZ1(1, -1, 1), XYZ1(1.f, 1.f, 0.f)},
		{XYZ1(1, -1, 1), XYZ1(1.f, 1.f, 0.f)},
		{XYZ1(1, 1, -1), XYZ1(1.f, 1.f, 0.f)},
		{XYZ1(1, -1, -1), XYZ1(1.f, 1.f, 0.f)},
		// magenta - top face
		{XYZ1(1, 1, 1), XYZ1(1.f, 0.f, 1.f)},
		{XYZ1(-1, 1, 1), XYZ1(1.f, 0.f, 1.f)},
		{XYZ1(1, 1, -1), XYZ1(1.f, 0.f, 1.f)},
		{XYZ1(1, 1, -1), XYZ1(1.f, 0.f, 1.f)},
		{XYZ1(-1, 1, 1), XYZ1(1.f, 0.f, 1.f)},
		{XYZ1(-1, 1, -1), XYZ1(1.f, 0.f, 1.f)},
		// cyan - bottom face
		{XYZ1(1, -1, 1), XYZ1(0.f, 1.f, 1.f)},
		{XYZ1(1, -1, -1), XYZ1(0.f, 1.f, 1.f)},
		{XYZ1(-1, -1, 1), XYZ1(0.f, 1.f, 1.f)},
		{XYZ1(-1, -1, 1), XYZ1(0.f, 1.f, 1.f)},
		{XYZ1(1, -1, -1), XYZ1(0.f, 1.f, 1.f)},
		{XYZ1(-1, -1, -1), XYZ1(0.f, 1.f, 1.f)},
	};

	uint32_t vertexBufferSize = static_cast<uint32_t>(vertexBuffer.size()) * sizeof(Vertex);

	// no index data
	std::vector<uint32_t> indexBuffer;
	uint32_t indexBufferSize = 0;
#endif //

	// Vertex buffer
	// Create a host-visible buffer to copy the vertex data to (staging buffer)
	VulkanBuffer* pStagingVertices = GE_ALLOC(VulkanBuffer)(mpDevice, 
								VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
								VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT, vertexBufferSize, vertexBuffer.data());
	assert(pStagingVertices != nullptr);

	// Create a device local buffer to which the (host local) vertex data will be copied and which will be used for rendering
	mpVertices = GE_ALLOC(VulkanBuffer)(mpDevice, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
										VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT, vertexBufferSize);
	assert(mpVertices != nullptr);

	// Buffer copies have to be submitted to a queue, so we need a command buffer for them
	// Note: Some devices offer a dedicated transfer queue (with only the transfer bit set) that may be faster when doing lots of copies

	if (mpDevice->IsPresentQueueSupported()) // separate present queue
	{
		pStagingVertices->CopyTo(mpVertices, mpDevice->GetPresentQueue());
	}
	else // graphics and present queue are the same
	{
		pStagingVertices->CopyTo(mpVertices, mpDevice->GetMainGraphicsQueue());
	}
	GE_FREE(pStagingVertices);

#if defined(USE_INDEX_BUFFER)
	// Index buffer
	// Create a host-visible buffer to copy the vertex data to (staging buffer)
	VulkanBuffer* pStagingIndices = GE_ALLOC(VulkanBuffer)(mpDevice, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
															VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT, indexBufferSize, indexBuffer.data());
	assert(pStagingIndices != nullptr);

	// Create a device local buffer to which the (host local) vertex data will be copied and which will be used for rendering
	mpIndices = GE_ALLOC(VulkanBuffer)(mpDevice, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
										VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT, indexBufferSize);
	assert(mpIndices != nullptr);

	// Buffer copies have to be submitted to a queue, so we need a command buffer for them
	// Note: Some devices offer a dedicated transfer queue (with only the transfer bit set) that may be faster when doing lots of copies

	if (mpDevice->IsPresentQueueSupported()) // separate present queue
	{
		pStagingIndices->CopyTo(mpIndices, mpDevice->GetPresentQueue());
	}
	else // graphics and present queue are the same
	{
		pStagingIndices->CopyTo(mpIndices, mpDevice->GetMainGraphicsQueue());
	}

	GE_FREE(pStagingIndices);
#endif // USE_INDEX_BUFFER
}

void VulkanRenderer::setupUniformBuffers()
{
	// This buffer will be used as a uniform buffer
	mpUniformBufferVS = GE_ALLOC(VulkanBuffer)(mpDevice, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
												VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, static_cast<VkDeviceSize>(sizeof(mUboVS)));
	assert(mpUniformBufferVS != nullptr);

	// Store information in the uniform's descriptor that is used by the descriptor set
	mpUniformBufferVS->SetDecriptorInfo(static_cast<VkDeviceSize>(sizeof(mUboVS)));


	updateUBO();
}

void VulkanRenderer::setupShaders()
{
	mShaderModules.resize(SHADER_COUNT);

	mShaderModules[VERTEX_STAGE] = GE_ALLOC(VulkanShaderModule)(mpDevice, VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT, "../../LibGraphicsEngine/res/shaders/triangle.vert");
	mShaderModules[FRAGMENT_STAGE] = GE_ALLOC(VulkanShaderModule)(mpDevice, VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT, "../../LibGraphicsEngine/res/shaders/triangle.frag");
}

void VulkanRenderer::setupTextures()
{
	//TODO
}


void VulkanRenderer::updateUBO()
{
	// Update matrices
	assert(mpCamera != nullptr);

	mUboVS.projectionMatrix = mpCamera->GetProjectionMatrix();

	mUboVS.viewMatrix = mpCamera->GetViewMatrix();

	mUboVS.modelMatrix = glm::mat4(1.0f);
	//mUboVS.modelMatrix = glm::rotate(mUboVS.modelMatrix, glm::radians(mRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	//mUboVS.modelMatrix = glm::rotate(mUboVS.modelMatrix, glm::radians(mRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	//mUboVS.modelMatrix = glm::rotate(mUboVS.modelMatrix, glm::radians(mRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

	// Map uniform buffer and update it
	assert(mpUniformBufferVS != nullptr);

	mpUniformBufferVS->Map(sizeof(mUboVS));
	mpUniformBufferVS->SetData(&mUboVS, (VkDeviceSize)sizeof(mUboVS));
	mpUniformBufferVS->UnMap();
}

void VulkanRenderer::viewChanged()
{
	updateUBO();
}

///////////////////////////////
void VulkanRenderer::setupDescriptorPool()
{
	// We need to tell the API the number of max. requested descriptors per type

	// This example only uses one descriptor type (uniform buffer) and only requests one descriptor of this type
	std::vector<VkDescriptorPoolSize> descriptorPoolSizes;
	descriptorPoolSizes.push_back(VulkanInitializers::DescriptorPoolSize(VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, NUM_DESCRIPTOR_SETS));

	// For additional types you need to add new entries in the type count list
	// E.g. for two combined image samplers :
	// typeCounts[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	// typeCounts[1].descriptorCount = 2;

	// Create the global descriptor pool
	// All descriptors used in this example are allocated from this pool
	mpDescriptorPool = GE_ALLOC(VulkanDescriptorPool)(mpDevice, NUM_DESCRIPTOR_SETS, descriptorPoolSizes);
}

void VulkanRenderer::setupDescriptorSet()
{
	// Setup layout of descriptors used in this example
	// Basically connects the different shader stages to descriptors for binding uniform buffers, image samplers, etc.
	// So every shader binding should map to one descriptor set layout binding

	// Binding 0: Uniform buffer (Vertex shader)
	std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
	descriptorSetLayoutBindings.push_back(VulkanInitializers::DescriptorSetLayoutBinding(0, VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT));

	mpDescriptorSetLayout = GE_ALLOC(VulkanDescriptorSetLayout)(mpDevice, descriptorSetLayoutBindings);

	// Allocate a new descriptor set from the global descriptor pool
	mpDescriptorSet = GE_ALLOC(VulkanDescriptorSet)(mpDevice, mpDescriptorPool, { mpDescriptorSetLayout });

	// Update the descriptor set determining the shader binding points
	// For every binding point used in a shader there needs to be one
	// descriptor set matching that binding point

	// Binding 0 : Uniform buffer
	//// Binds this uniform buffer to binding point 0
	std::vector<VkWriteDescriptorSet> writeDescriptorSets;
	writeDescriptorSets.push_back(VulkanInitializers::WriteDescriptorSet(mpDescriptorSet->GetHandle(), 0, 0, 1, VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, nullptr,
												&mpUniformBufferVS->GetDescriptorInfo()));

	//vkUpdateDescriptorSets(mpDevice->GetDeviceHandle(), 1, &writeDescriptorSet, 0, nullptr);
	mpDescriptorSet->Update(writeDescriptorSets, {});
}

void VulkanRenderer::setupPipeline()
{
	// Create the graphics pipeline used in this example
	// Vulkan uses the concept of rendering pipelines to encapsulate fixed states, replacing OpenGL's complex state machine
	// A pipeline is then stored and hashed on the GPU making pipeline changes very fast
	// Note: There are still a few dynamic states that are not directly part of the pipeline (but the info that they are used is)

	// Construct the differnent states making up the pipeline

	// Shaders
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
	shaderStages.resize(SHADER_COUNT);

	// Vertex shader
	shaderStages[VERTEX_STAGE] = VulkanInitializers::PipelineShaderStageCreateInfo(VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT, mShaderModules[VERTEX_STAGE]->GetHandle(), "main");

	// Fragment shader
	shaderStages[FRAGMENT_STAGE] = VulkanInitializers::PipelineShaderStageCreateInfo(VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT, mShaderModules[FRAGMENT_STAGE]->GetHandle(), "main");

	// Vertex input descriptions 
	// Specifies the vertex input parameters for a pipeline

	// Vertex input binding
	// This example uses a single vertex input binding at binding point 0 (see vkCmdBindVertexBuffers)
	VkVertexInputBindingDescription vertexInputBindingDescription =
		VulkanInitializers::VertexInputBindingDescription(0, sizeof(Vertex), VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX);

	// Inpute attribute bindings describe shader attribute locations and memory layouts
	std::array<VkVertexInputAttributeDescription, 2> vertexInputAttributs;
	// These match the following shader layout (see triangle.vert):
	//	layout (location = 0) in vec3 inPos;
	//	layout (location = 1) in vec3 inColor;

	vertexInputAttributs[0] = VulkanInitializers::VertexInputAttributeDescription(0, 0, VkFormat::VK_FORMAT_R32G32B32_SFLOAT, OFFSETOF(Vertex, position));
	vertexInputAttributs[1] = VulkanInitializers::VertexInputAttributeDescription(1, 0, VkFormat::VK_FORMAT_R32G32B32_SFLOAT, OFFSETOF(Vertex, color));

	// Vertex input state used for pipeline creation
	VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = 
		VulkanInitializers::PipelineVertexInputStateCreateInfo(1, &vertexInputBindingDescription, 2, vertexInputAttributs.data());

	// Input assembly state describes how primitives are assembled
	// This pipeline will assemble vertex data as a triangle lists (though we only use one triangle)
	VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = 
		VulkanInitializers::PipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE);


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
#if defined(TRIANGLE) || defined(CUBE)
	pipelineRasterizationStateCreateInfo.frontFace = VkFrontFace::VK_FRONT_FACE_CLOCKWISE;
#endif

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

	mpPipelineLayout = GE_ALLOC(VulkanPipelineLayout)(mpDevice, { mpDescriptorSetLayout }, {});

	//// Assign the pipeline states to the pipeline creation info structure
	mpGraphicsPipeline = GE_ALLOC(VulkanGraphicsPipeline)(mpDevice, mpPipelineCache, shaderStages, pipelineVertexInputStateCreateInfo, pipelineInputAssemblyStateCreateInfo,
									{}, pipelineViewportStateCreateInfo, pipelineRasterizationStateCreateInfo, pipelineMultisampleStateCreateInfo, 
									pipelineDepthStencilStateCreateInfo, pipelineColorBlendStateCreateInfo, pipelineDynamicStateCreateInfo,
									mpPipelineLayout, mpDefaultRenderPass);

	// Shader modules are no longer needed once the graphics pipeline has been created
	GE_FREE(mShaderModules[VERTEX_STAGE]);
	GE_FREE(mShaderModules[FRAGMENT_STAGE]);
}

// Build separate command buffers for every framebuffer image
// Unlike in OpenGL all rendering commands are recorded once into command buffers that are then resubmitted to the queue
// This allows to generate work upfront and from multiple threads, one of the biggest advantages of Vulkan
void VulkanRenderer::buildCommandBuffers()
{
	// THE ACTUAL RENDERING IS DONE HERE !!!!

	// Set clear values for all framebuffer attachments with loadOp set to clear
	// We use two attachments (color and depth) that are cleared at the start of the subpass and as such we need to set clear values for both
	std::vector<VkClearValue> clearValues;
	clearValues.resize(2);
	clearValues[0].color = { { 0.0f, 0.0f, 0.2f, 1.0f } };
	clearValues[1].depthStencil = { 1.0f, 0 };

	VkRect2D renderArea = VulkanInitializers::Rect2D(VulkanInitializers::Offset2D(0, 0), VulkanInitializers::Extent2D(mWindowWidth, mWindowHeight));

	for (int32_t i = 0; i < mDrawCommandBuffers.size(); ++i)
	{

		// Begin command buffer recording
		VK_CHECK_RESULT(mDrawCommandBuffers[i]->Begin());

		resetQuery(i);

		// RenderPass Begin
		mpDefaultRenderPass->Begin(mDrawCommandBuffers[i]->GetHandle(), mDefaultFrameBuffers[i]->GetHandle(), renderArea, clearValues);

		// Update dynamic viewport state
		VkViewport viewport = VulkanInitializers::Viewport(0, 0, static_cast<bfloat32_t>(mWindowWidth), static_cast<bfloat32_t>(mWindowHeight), 0.0f, 1.0f);
		vkCmdSetViewport(mDrawCommandBuffers[i]->GetHandle(), 0, 1, &viewport);

		// Update dynamic scissor state
		VkRect2D scissor = VulkanInitializers::Rect2D(VulkanInitializers::Offset2D(0, 0), VulkanInitializers::Extent2D(mWindowWidth, mWindowHeight));
		vkCmdSetScissor(mDrawCommandBuffers[i]->GetHandle(), 0, 1, &scissor);

		beginQuery(i);

		// First Subpass (the only one in this case) - is bound by default
		// The next subpasses if present must be bound explicitely using: vkCmdNextSubpass(...)
		{
			// Bind descriptor sets describing shader binding points
			vkCmdBindDescriptorSets(mDrawCommandBuffers[i]->GetHandle(), VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, mpPipelineLayout->GetHandle(), 0, 1, &mpDescriptorSet->GetHandle(), 0, nullptr);

			// Bind the rendering pipeline
			// The pipeline (state object) contains all states of the rendering pipeline, binding it will set all the states specified at pipeline creation time
			vkCmdBindPipeline(mDrawCommandBuffers[i]->GetHandle(), VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, mpGraphicsPipeline->GetHandle());

			// Bind triangle vertex buffer (contains position and colors)
			VkDeviceSize offsets[1] = { 0 };
			vkCmdBindVertexBuffers(mDrawCommandBuffers[i]->GetHandle(), VERTEX_BUFFER_BIND_ID, 1, &mpVertices->GetHandle(), offsets);

//#if defined(USE_INDEX_BUFFER) // Draw indexed
			// Bind triangle index buffer
	//		vkCmdBindIndexBuffer(mDrawCommandBuffers[i]->GetHandle(), mpIndices->GetHandle(), 0, VK_INDEX_TYPE_UINT32);

			// Draw indexed triangle
	//		vkCmdDrawIndexed(mDrawCommandBuffers[i]->GetHandle(), mIndicesCount, 1, 0, 0, 1);
//#else // Draw
			vkCmdDraw(mDrawCommandBuffers[i]->GetHandle(), 3, 1, 0, 0); // DRAW TRIANGLE
		//	vkCmdDraw(mDrawCommandBuffers[i]->GetHandle(), 12 * 3, 1, 0, 0); // DRAW CUBE
//#endif // USE_INDEX_BUFFER
		}

		endQuery(i);

		// RenderPass End
		mpDefaultRenderPass->End(mDrawCommandBuffers[i]->GetHandle());

		// End command buffer recording
		// Ending the render pass will add an implicit barrier transitioning the frame buffer color attachment to 
		// VK_IMAGE_LAYOUT_PRESENT_SRC_KHR for presenting it to the windowing system
		VK_CHECK_RESULT(mDrawCommandBuffers[i]->End());
	}
}
#endif

void VulkanRenderer::setupSubmitInfo()
{
	/** @brief Pipeline stages used to wait at for graphics queue submissions */
	mSubmitPipelineStages = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	// The submit info structure specifices a command buffer queue submission batch
	mSubmitInfo = VulkanInitializers::SubmitInfo(1, &mDrawCommandBuffers[mCurrentBufferIdx]->GetHandle(), 1, &mpPresentCompleteSemaphore->GetHandle(), &mSubmitPipelineStages, 1, &mpRenderCompleteSemaphore->GetHandle());
}

void VulkanRenderer::setupPipelineStats()
{
#ifdef PIPELINE_STATS
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

	mpQueryPool = GE_ALLOC(VulkanQueryPool)(mpDevice, VkQueryType::VK_QUERY_TYPE_PIPELINE_STATISTICS, 
		static_cast<uint32_t>(mPipelineStats.size()), pipelineStatsFlags);
	assert(mpQueryPool != nullptr);
#endif
}

// Retrieves the results of the pipeline statistics query submitted to the command buffer
void VulkanRenderer::getQueryResults()
{
#ifdef PIPELINE_STATS
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

void VulkanRenderer::resetQuery(uint32_t currentBufferId)
{
#ifdef PIPELINE_STATS
	mpQueryPool->ResetQuery(mDrawCommandBuffers[currentBufferId]->GetHandle(), static_cast<uint32_t>(mPipelineStats.size()));
#endif
}

void VulkanRenderer::beginQuery(uint32_t currentBufferId)
{
#ifdef PIPELINE_STATS
	mpQueryPool->BeginQuery(mDrawCommandBuffers[currentBufferId]->GetHandle());
#endif
}

void VulkanRenderer::endQuery(uint32_t currentBufferId)
{
#ifdef PIPELINE_STATS
	mpQueryPool->EndQuery(mDrawCommandBuffers[currentBufferId]->GetHandle());
 #endif
}

void VulkanRenderer::RenderFrame()
{
#ifndef NEW_GRAPHICS
	viewChanged();

	Draw();

	getQueryResults();
#endif
}

void VulkanRenderer::UpdateFrame(bfloat32_t deltaTime)
{
	//TODO
}

void VulkanRenderer::BeginFrame()
{
	//TODO
}

void VulkanRenderer::EndFrame()
{
	// TODO
}

#ifndef NEW_GRAPHICS
void VulkanRenderer::Draw()
{
	// HERE THE FRAME IS JUST PRESENTED TO THE GRAPHICS QUEUE
	// THE ACTUAL RENDERING IS DONE WHEN THE COMMAND BUFFER IS BUILT/ALL COMMANDS RECORDED

	if (false == mIsPrepared)
		return;

	// Get next image in the swap chain (back/front buffer)
	// updates mCurrentBuffer
	PrepareFrame();

	// Use a fence to wait until the command buffer has finished execution before using it again
	VK_CHECK_RESULT(mWaitFences[mCurrentBufferIdx]->WaitIdle(VK_TRUE, UINT64_MAX));
	VK_CHECK_RESULT(mWaitFences[mCurrentBufferIdx]->Reset());

	// account for the new value of mCurrentBufferIdx
	mSubmitInfo.pCommandBuffers = &mDrawCommandBuffers[mCurrentBufferIdx]->GetHandle();
	
	// Submit to the graphics queue
	VK_CHECK_RESULT(mpDevice->GetMainGraphicsQueue()->Submit(1, &mSubmitInfo, mWaitFences[mCurrentBufferIdx]->GetHandle()));

	// Present the current buffer to the swap chain
	// Pass the semaphore signaled by the command buffer submission from the submit info as the wait semaphore for swap chain presentation
	// This ensures that the image is not presented to the windowing system until all commands have been submitted
	PresentFrame();
}
#endif

void VulkanRenderer::SubmitFrame()
{
	// HERE THE FRAME IS JUST SUBMITED TO THE GRAPHICS QUEUE
	// THE IMAGE IS PRESENTED TO THE SWAPCHAIN
	// THE ACTUAL RENDERING IS DONE WHEN THE COMMAND BUFFER IS BUILT/ALL COMMANDS RECORDED

	if (false == mIsPrepared)
		return;

	// Get next image in the swap chain (back/front buffer)
	// updates mCurrentBuffer
	PrepareFrame();

	// Use a fence to wait until the command buffer has finished execution before using it again
	VK_CHECK_RESULT(mWaitFences[mCurrentBufferIdx]->WaitIdle(VK_TRUE, UINT64_MAX));
	VK_CHECK_RESULT(mWaitFences[mCurrentBufferIdx]->Reset());

	// account for the new value of mCurrentBufferIdx
	mSubmitInfo.pCommandBuffers = &mDrawCommandBuffers[mCurrentBufferIdx]->GetHandle();

	// Submit to the graphics queue
	VK_CHECK_RESULT(mpDevice->GetMainGraphicsQueue()->Submit(1, &mSubmitInfo, mWaitFences[mCurrentBufferIdx]->GetHandle()));

	// Present the current buffer to the swap chain
	// Pass the semaphore signaled by the command buffer submission from the submit info as the wait semaphore for swap chain presentation
	// This ensures that the image is not presented to the windowing system until all commands have been submitted
	PresentFrame();
}

void VulkanRenderer::PrepareFrame()
{
	// Acquire the next image from the swap chain - we get the next image index
	VkResult res = mpDevice->AcquireNextImage(&mCurrentBufferIdx, mpPresentCompleteSemaphore->GetHandle());

	// Recreate the swapchain if it's no longer compatible with the surface (OUT_OF_DATE) or no longer optimal for presentation (SUBOPTIMAL)
	if ((res == VkResult::VK_ERROR_OUT_OF_DATE_KHR) || (res == VkResult::VK_SUBOPTIMAL_KHR))
	{
		WindowResize();
	}
	else
	{
		VK_CHECK_RESULT(res);
	}
}

void VulkanRenderer::PresentFrame()
{
	// Return the image to the swap chain for presentation
	VkPresentInfoKHR presentInfo = 
		VulkanInitializers::PresentInfo(1, &mpDevice->GetSwapChainHandle(), 1, &mpRenderCompleteSemaphore->GetHandle(), &mCurrentBufferIdx);

	VkResult res = VkResult::VK_SUCCESS;
	if (mpDevice->IsPresentQueueSupported()) // separate present queue
	{
		mpDevice->GetPresentQueue()->Present(presentInfo);
	}
	else // graphics and present queue are the same
	{
		res = mpDevice->GetMainGraphicsQueue()->Present(presentInfo);
	}
	if (false == ((res == VkResult::VK_SUCCESS) || (res == VkResult::VK_SUBOPTIMAL_KHR)))
	{
		if (res == VkResult::VK_ERROR_OUT_OF_DATE_KHR)
		{
			// Swap chain is no longer compatible with the surface and needs to be recreated
			WindowResize();
			return;
		}
		else
		{
			VK_CHECK_RESULT(res);
		}
	}
	// wait until queue is idle
	VK_CHECK_RESULT(mpDevice->GetMainGraphicsQueue()->WaitIdle());
}

VulkanDevice* VulkanRenderer::GetDevice() const
{
	return mpDevice;
}

///////////////////////

void VulkanRenderer::DrawObject(RenderQueue::Renderable* pRenderable, uint32_t currentBufferIdx)
{
	assert(pRenderable != nullptr);
	assert(pRenderable->pGeometryNode != nullptr);

	auto& geoPrimitives = pRenderable->pGeometryNode->GetGeometricPrimitives();

	// draw geometric primitives
	for (auto& primitive : geoPrimitives)
	{
		if (primitive)
		{
			//bind material
			Bind(pRenderable->pMaterial, primitive, currentBufferIdx);


			//bind vertex format
			GADRVertexFormat* gadrVertexFormat = Bind(primitive->GetVertexFormat());

			// bind vertex buffer
			GADRVertexBuffer* gadrVertexBuffer = Bind(primitive->GetVertexBuffer());


			VkDeviceSize offsets[1] = { 0 };
			vkCmdBindVertexBuffers(mDrawCommandBuffers[currentBufferIdx]->GetHandle(), VERTEX_BUFFER_BIND_ID, 1, &(gadrVertexBuffer->GetVkBuffer()->GetHandle()), offsets);

			if (primitive->IsIndexed())
			{
				// bind index buffer
				GADRIndexBuffer* gadrIndexBuffer = Bind(primitive->GetIndexBuffer());

				VkIndexType vkIndexType = GADRIndexBuffer::IndexTypeToVulkanIndexType(primitive->GetIndexBuffer()->GetIndexType());
				vkCmdBindIndexBuffer(mDrawCommandBuffers[currentBufferIdx]->GetHandle(), gadrIndexBuffer->GetVkBuffer()->GetHandle(), 0, vkIndexType);

				// draw indexed
				// 1 instance as we do not do instance drawing
				vkCmdDrawIndexed(mDrawCommandBuffers[currentBufferIdx]->GetHandle(), primitive->GetIndexBuffer()->GetIndexCount(), 1, 0, 0, 1);
			}
			else
			{
				// draw
				// 1 instance as we do not do instance drawing
				vkCmdDraw(mDrawCommandBuffers[currentBufferIdx]->GetHandle(), primitive->GetVertexBuffer()->GetVertexCount(), 1, 0, 0);
			}
		}
	}
}

void VulkanRenderer::DrawGeometry(GeometryNode* pGeometryNode, uint32_t currentBufferIdx)
{
	assert(pGeometryNode != nullptr);

	auto& geoPrimitives = pGeometryNode->GetGeometricPrimitives();

	// draw geometric primitives
	for (auto& primitive : geoPrimitives)
	{
		if (primitive)
		{
			//bind vertex format
			GADRVertexFormat* gadrVertexFormat = Bind(primitive->GetVertexFormat());

			// bind vertex buffer
			GADRVertexBuffer* gadrVertexBuffer = Bind(primitive->GetVertexBuffer());


			VkDeviceSize offsets[1] = { 0 };
			vkCmdBindVertexBuffers(mDrawCommandBuffers[currentBufferIdx]->GetHandle(), VERTEX_BUFFER_BIND_ID, 1, &(gadrVertexBuffer->GetVkBuffer()->GetHandle()), offsets);

			if (primitive->IsIndexed())
			{
				// bind index buffer
				GADRIndexBuffer* gadrIndexBuffer = Bind(primitive->GetIndexBuffer());
				
				vkCmdBindIndexBuffer(mDrawCommandBuffers[currentBufferIdx]->GetHandle(), gadrIndexBuffer->GetVkBuffer()->GetHandle(), 0, VK_INDEX_TYPE_UINT32);

				// draw indexed
				// 1 instance as we do not do instance drawing
				vkCmdDrawIndexed(mDrawCommandBuffers[currentBufferIdx]->GetHandle(), primitive->GetIndexBuffer()->GetIndexCount(), 1, 0, 0, 1);
			}
			else
			{
				// draw
				// 1 instance as we do not do instance drawing
				vkCmdDraw(mDrawCommandBuffers[currentBufferIdx]->GetHandle(), primitive->GetVertexBuffer()->GetVertexCount(), 1, 0, 0);
			}
		}
	}
}

void VulkanRenderer::Render(RenderQueue* pRenderQueue, RenderPass* pRenderPass)
{
	assert(pRenderQueue != nullptr);
	assert(pRenderPass != nullptr);

	mpRenderQueue = pRenderQueue;
	mpRenderPass = pRenderPass;

	BeginFrame();

	DrawSceneToCommandBuffer();

	EndFrame();
}

void VulkanRenderer::Update(Camera* pCamera, bfloat32_t deltaTime)
{
	assert(mpRenderQueue != nullptr);
	assert(mpRenderPass != nullptr);

	assert(pCamera != nullptr);
	mpCamera = pCamera;

	mpRenderPass->Update(this, mpRenderQueue, mpCamera, deltaTime);

	/// pipeline stats per frame
	getQueryResults();
}

void VulkanRenderer::UpdateUniformBuffers(RenderQueue::Renderable* pRenderable, Camera* pCamera)
{
	assert(pRenderable != nullptr);
	assert(pRenderable->pMaterial != nullptr);
	assert(pCamera != nullptr);

	auto uniformBuffer = pRenderable->pMaterial->GetUniformBuffer();

	switch (uniformBuffer->GetUniformUsage())
	{
		case UniformBuffer::UniformUsage::UU_PVM:
		{
			// matrix setup
		//	uniformBuffer->Uniforms.projectionMatrix = pCamera->GetProjectionMatrix();
		//	uniformBuffer->Uniforms.viewMatrix = pCamera->GetViewMatrix();
		//	uniformBuffer->Uniforms.modelMatrix = glm::mat4(1.0f);

			uniformBuffer->Uniforms.PVM = pCamera->GetProjectionViewMatrix(); // NOTE! Model Matrix is identity for now
		}
		break;
	}
	
	//bind uniform buffer
	GADRUniformBuffer* gadrUniformBuffer = Bind(uniformBuffer);
	assert(gadrUniformBuffer != nullptr);

	gadrUniformBuffer->UpdateData(uniformBuffer);

}

void VulkanRenderer::BindShaderBindings(uint32_t currentBufferIdx, VulkanDescriptorSet* pDescriptorSet,
					VulkanPipelineLayout* pPipelineLayout, VulkanGraphicsPipeline* pGraphicsPipeline)
{
	assert(pDescriptorSet != nullptr);
	assert(pPipelineLayout != nullptr);
	assert(pGraphicsPipeline != nullptr);

	// Bind descriptor sets describing shader binding points
	vkCmdBindDescriptorSets(mDrawCommandBuffers[currentBufferIdx]->GetHandle(), VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pPipelineLayout->GetHandle(), 0, 1, &pDescriptorSet->GetHandle(), 0, nullptr);

	// Bind the rendering pipeline
	// The pipeline (state object) contains all states of the rendering pipeline, binding it will set all the states specified at pipeline creation time
	vkCmdBindPipeline(mDrawCommandBuffers[currentBufferIdx]->GetHandle(), VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pGraphicsPipeline->GetHandle());
}

void VulkanRenderer::DrawSceneToCommandBuffer()
{
	assert(mpRenderPass != nullptr);
	assert(mpRenderQueue != nullptr);

	// THE ACTUAL RENDERING IS DONE HERE !!!!

	// Set clear values for all framebuffer attachments with loadOp set to clear
	// We use two attachments (color and depth) that are cleared at the start of the subpass and as such we need to set clear values for both
	std::vector<VkClearValue> clearValues;
	clearValues.resize(2);
	clearValues[0].color = { { 0.0f, 0.0f, 0.2f, 1.0f } };
	clearValues[1].depthStencil = { 1.0f, 0 };

	VkRect2D renderArea = VulkanInitializers::Rect2D(VulkanInitializers::Offset2D(0, 0), VulkanInitializers::Extent2D(mWindowWidth, mWindowHeight));

	for (int32_t i = 0; i < mDrawCommandBuffers.size(); ++i)
	{

		// Begin command buffer recording
		VK_CHECK_RESULT(mDrawCommandBuffers[i]->Begin());

		resetQuery(i);

		// RenderPass Begin
		mpDefaultRenderPass->Begin(mDrawCommandBuffers[i]->GetHandle(), mDefaultFrameBuffers[i]->GetHandle(), renderArea, clearValues);

		// Update dynamic viewport state
		VkViewport viewport = VulkanInitializers::Viewport(0, 0, static_cast<bfloat32_t>(mWindowWidth), static_cast<bfloat32_t>(mWindowHeight), 0.0f, 1.0f);
		vkCmdSetViewport(mDrawCommandBuffers[i]->GetHandle(), 0, 1, &viewport);

		// Update dynamic scissor state
		VkRect2D scissor = VulkanInitializers::Rect2D(VulkanInitializers::Offset2D(0, 0), VulkanInitializers::Extent2D(mWindowWidth, mWindowHeight));
		vkCmdSetScissor(mDrawCommandBuffers[i]->GetHandle(), 0, 1, &scissor);

		beginQuery(i);

		mpRenderPass->Render(this, mpRenderQueue, i);

		endQuery(i);

		// RenderPass End
		mpDefaultRenderPass->End(mDrawCommandBuffers[i]->GetHandle());

		// End command buffer recording
		// Ending the render pass will add an implicit barrier transitioning the frame buffer color attachment to 
		// VK_IMAGE_LAYOUT_PRESENT_SRC_KHR for presenting it to the windowing system
		VK_CHECK_RESULT(mDrawCommandBuffers[i]->End());
	}
}

/////////////////////////////////////


VulkanPipelineCache* VulkanRenderer::GetVkPipelineCache() const
{
	return mpPipelineCache;
}

VulkanRenderPass* VulkanRenderer::GetVkDefaultRenderPass() const
{
	return mpDefaultRenderPass;
}