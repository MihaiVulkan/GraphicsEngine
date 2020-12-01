#ifndef GRAPHICS_RENDERING_VULKAN_INITIALIZERS_HPP
#define GRAPHICS_RENDERING_VULKAN_INITIALIZERS_HPP

#include "Foundation/TypeDefs.hpp"
#include "AppConfig.hpp"
#include "vulkan/vulkan.h"

// Disable mecro to solve name clash
#ifdef MemoryBarrier
#undef MemoryBarrier
#endif

namespace GraphicsEngine
{
	namespace Graphics
	{
		/*
			Wrapper to ease Vulkan initialization when creating objects/structs/properties
		*/
		namespace VulkanInitializers
		{
			// Debug
			VkDebugReportCallbackCreateInfoEXT DebugReportCallbackCreateInfo(PFN_vkDebugReportCallbackEXT pfnCallback, VkDebugReportFlagsEXT flags);

			VkDebugMarkerObjectNameInfoEXT DebugMarkerObjectNameInfo(VkDebugReportObjectTypeEXT objectType, uint64_t object, const char_t* pObjectName);

			VkDebugMarkerObjectTagInfoEXT DebugMarkerObjectTagInfo(VkDebugReportObjectTypeEXT objectType, uint64_t object, uint64_t tagName, size_t tagSize, const void* pTag);

			VkDebugMarkerMarkerInfoEXT DebugMarkerMarkerInfo(const char_t* pMarkerName, bfloat32_t color[4]);

			// Instance
			VkApplicationInfo ApplicationInfo(const char_t* pApplicationName, uint32_t applicationVersion, const char_t* pEngineName, uint32_t engineVersion, uint32_t apiVersion = VK_API_VERSION_1_0);

			VkInstanceCreateInfo InstanceCreateInfo(const VkApplicationInfo* pApplicationInfo, uint32_t enabledExtensionCount = 0, const char_t * const* ppEnabledExtensionNames = nullptr,
				uint32_t enabledLayerCount = 0, const char_t* const* ppEnabledLayerNames = nullptr, VkInstanceCreateFlags flags = 0);

			// Device
#if defined(VK_VERSION_1_1)
			VkPhysicalDeviceGroupProperties PhysicalDeviceGroupProperties();
			VkPhysicalDeviceProperties2 PhysicalDeviceProperties2();
#endif // defined(VK_VERSION_1_1)

#if defined(VK_VERSION_1_2)
			VkPhysicalDeviceDriverProperties PhysicalDeviceDriverProperties();
#else //extension
			VkPhysicalDeviceDriverPropertiesKHR PhysicalDeviceDriverPropertiesKHR();
#endif // defined(VK_VERSION_1_2)

			VkDeviceCreateInfo DeviceCreateInfo(uint32_t queueCreateInfoCount, const VkDeviceQueueCreateInfo* pQueueCreateInfos, const VkPhysicalDeviceFeatures* pEnabledFeatures,
				uint32_t enabledExtensionCount = 0, const char_t* const* ppEnabledExtensionNames = nullptr,
				uint32_t enabledLayerCount = 0, const char_t* const* ppEnabledLayerNames = nullptr, VkDeviceCreateFlags flags = 0);

			// Queue
			VkDeviceQueueCreateInfo DeviceQueueCreateInfo(uint32_t queueFamilyIndex, uint32_t queueCount, const bfloat32_t* pQueuePriorities, VkDeviceQueueCreateFlags flags = 0);

			// Surface
			// No Initializer for surface - platform dependent - dealt with in VulkanDevice::CreateSurface()

			// SwapChain - extension
			VkSwapchainCreateInfoKHR SwapchainCreateInfoKHR(VkSurfaceKHR surface, uint32_t minImageCount, VkFormat imageFormat, VkColorSpaceKHR imageColorSpace, const VkExtent2D& imageExtent,
				uint32_t imageArrayLayers, VkImageUsageFlags imageUsage, VkSharingMode imageSharingMode, uint32_t queueFamilyIndexCount,
				const uint32_t* pQueueFamilyIndices, VkSurfaceTransformFlagBitsKHR preTransform, VkCompositeAlphaFlagBitsKHR compositeAlpha,
				VkPresentModeKHR presentMode, VkBool32 clipped, VkSwapchainKHR oldSwapchain, VkSwapchainCreateFlagsKHR flags = 0);

			// Memory
			VkMemoryAllocateInfo MemoryAllocateInfo(VkDeviceSize allocationSize, uint32_t memoryTypeIndex = 0);

			VkMappedMemoryRange MappedMemoryRange(VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size);

			// Buffer
			VkBufferCreateInfo BufferCreateInfo(VkDeviceSize size, VkBufferUsageFlags usage, VkSharingMode sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE, uint32_t queueFamilyIndexCount = 0,
				const uint32_t * pQueueFamilyIndices = nullptr, VkBufferCreateFlags flags = 0);

			VkBufferCopy BufferCopy(VkDeviceSize srcOffset, VkDeviceSize dstOffset, VkDeviceSize size);

			// BufferView
			VkBufferViewCreateInfo BufferViewCreateInfo(VkBuffer buffer, VkFormat format, VkDeviceSize offset, VkDeviceSize  range, VkBufferViewCreateFlags flags = 0);

			// Sampler
			VkSamplerCreateInfo SamplerCreateInfo(VkFilter magFilter, VkFilter  minFilter, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode addressModeU,
				VkSamplerAddressMode addressModeV, VkSamplerAddressMode addressModeW, bfloat32_t mipLodBias, VkBool32 anisotropyEnable,
				bfloat32_t maxAnisotropy, VkBool32 compareEnable, VkCompareOp compareOp, bfloat32_t minLod, bfloat32_t maxLod, VkBorderColor borderColor,
				VkBool32 unnormalizedCoordinates, VkSamplerCreateFlags flags = 0);

			// Image
			VkImageCreateInfo ImageCreateInfo(VkImageType imageType, VkFormat format, const VkExtent3D& extent, uint32_t mipLevels, uint32_t arrayLayers, VkSampleCountFlagBits samples,
				VkImageTiling tiling, VkImageUsageFlags usage, VkSharingMode sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE, uint32_t queueFamilyIndexCount = 0,
				const uint32_t * pQueueFamilyIndices = nullptr, VkImageLayout initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED, VkImageCreateFlags flags = 0);

			VkImageCopy ImageCopy(const VkImageSubresourceLayers& srcSubresource, const VkOffset3D& srcOffset, const VkImageSubresourceLayers& dstSubresource,
				const VkOffset3D& dstOffset, const VkExtent3D& extent);

			VkImageBlit ImageBlit(const VkImageSubresourceLayers& srcSubresource, VkOffset3D srcOffsets[2], const VkImageSubresourceLayers& dstSubresource, VkOffset3D dstOffsets[2]);

			VkBufferImageCopy BufferImageCopy(VkDeviceSize bufferOffset, uint32_t bufferRowLength, uint32_t bufferImageHeight, const VkImageSubresourceLayers& imageSubresource,
				const VkOffset3D& imageOffset, const VkExtent3D& imageExtent);

			VkComponentMapping ComponentMapping(VkComponentSwizzle r, VkComponentSwizzle g, VkComponentSwizzle b, VkComponentSwizzle a);

			VkImageSubresourceLayers ImageSubresourceLayers(VkImageAspectFlags aspectMask, uint32_t mipLevel, uint32_t baseArrayLayer, uint32_t layerCount);

			VkSubresourceLayout SubresourceLayout(VkDeviceSize offset, VkDeviceSize size, VkDeviceSize rowPitch, VkDeviceSize arrayPitch, VkDeviceSize depthPitch);

			VkImageSubresourceRange ImageSubresourceRange(VkImageAspectFlags aspectMask, uint32_t baseMipLevel, uint32_t levelCount, uint32_t baseArrayLayer, uint32_t layerCount);

			VkImageResolve ImageResolve(const VkImageSubresourceLayers& srcSubresource, const VkOffset3D& srcOffset, const VkImageSubresourceLayers& dstSubresource,
				const VkOffset3D& dstOffset, const VkExtent3D& extent);

			// ImageView
			VkImageViewCreateInfo ImageViewCreateInfo(VkImage image, VkImageViewType viewType, VkFormat format, const VkComponentMapping& components, const VkImageSubresourceRange& subresourceRange,
				VkImageViewCreateFlags flags = 0);

			// ShaderModule
			VkShaderModuleCreateInfo ShaderModuleCreateInfo(size_t codeSize, const uint32_t* pCode, VkShaderModuleCreateFlags flags = 0);

			// Attachment
			VkAttachmentDescription AttachmentDescription(VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp,
				VkAttachmentLoadOp stencilLoadOp, VkAttachmentStoreOp stencilStoreOp, VkImageLayout initialLayout,
				VkImageLayout finalLayout, VkAttachmentDescriptionFlags flags = 0);

			VkAttachmentReference AttachmentReference(uint32_t attachment, VkImageLayout layout);

			// Subpass
			VkSubpassDescription SubpassDescription(VkPipelineBindPoint pipelineBindPoint, uint32_t colorAttachmentCount, const VkAttachmentReference* pColorAttachments,
				const VkAttachmentReference* pDepthStencilAttachment, uint32_t inputAttachmentCount = 0, const VkAttachmentReference * pInputAttachments = nullptr,
				const VkAttachmentReference * pResolveAttachments = nullptr, uint32_t preserveAttachmentCount = 0, const uint32_t * pPreserveAttachments = nullptr,
				VkSubpassDescriptionFlags flags = 0);

			VkSubpassDependency SubpassDependency(uint32_t srcSubpass, uint32_t dstSubpass, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
				VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkDependencyFlags dependencyFlags);

			// RenderPass
			VkRenderPassCreateInfo RenderPassCreateInfo(uint32_t attachmentCount, const VkAttachmentDescription* pAttachments, uint32_t subpassCount,
				const VkSubpassDescription* pSubpasses, uint32_t dependencyCount, const VkSubpassDependency* pDependencies,
				VkRenderPassCreateFlags flags = 0);

			VkRenderPassBeginInfo RenderPassBeginInfo(VkRenderPass renderPass, VkFramebuffer framebuffer, const VkRect2D& renderArea, uint32_t clearValueCount,
				const VkClearValue* pClearValues);

			// Framebuffer
			VkFramebufferCreateInfo FramebufferCreateInfo(VkRenderPass renderPass, uint32_t attachmentCount, const VkImageView* pAttachments, uint32_t width,
				uint32_t height, uint32_t layers = 1, VkFramebufferCreateFlags flags = 0);







			// CommandPool
			VkCommandPoolCreateInfo CommandPoolCreateInfo(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

			// CommandBuffer
			VkCommandBufferAllocateInfo CommandBufferAllocateInfo(VkCommandPool commandPool, VkCommandBufferLevel level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY, uint32_t commandBufferCount = 1);

			VkCommandBufferInheritanceInfo CommandBufferInheritanceInfo(VkRenderPass renderPass, uint32_t subpass, VkFramebuffer framebuffer, VkBool32 occlusionQueryEnable,
				VkQueryControlFlags queryFlags, VkQueryPipelineStatisticFlags pipelineStatistics);

			VkCommandBufferBeginInfo CommandBufferBeginInfo(const VkCommandBufferInheritanceInfo* pInheritanceInfo = nullptr, VkCommandBufferUsageFlags flags = 0);

			// Semaphore
			VkSemaphoreCreateInfo SemaphoreCreateInfo(VkSemaphoreCreateFlags flags = 0);

			// Fence
			VkFenceCreateInfo FenceCreateInfo(VkFenceCreateFlags flags = 0);

			// Event
			VkEventCreateInfo EventCreateInfo(VkEventCreateFlags flags = 0);

			// MemoryBarrier
			VkMemoryBarrier MemoryBarrier(VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask);

			VkBufferMemoryBarrier BufferMemoryBarrier(VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, uint32_t srcQueueFamilyIndex, uint32_t dstQueueFamilyIndex,
				VkBuffer buffer, VkDeviceSize offset, VkDeviceSize size);

			VkImageMemoryBarrier ImageMemoryBarrier(VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldLayout, VkImageLayout newLayout,
				uint32_t srcQueueFamilyIndex, uint32_t dstQueueFamilyIndex, VkImage image, const VkImageSubresourceRange& subresourceRange);

			// QueryPool
			VkQueryPoolCreateInfo QueryPoolCreateInfo(VkQueryType queryType, uint32_t queryCount, VkQueryPipelineStatisticFlags pipelineStatistics, VkQueryPoolCreateFlags flags = 0);

			// DescriptorPool
			VkDescriptorPoolSize DescriptorPoolSize(VkDescriptorType type, uint32_t descriptorCount);

			VkDescriptorPoolCreateInfo DescriptorPoolCreateInfo(uint32_t maxSets, uint32_t poolSizeCount, const VkDescriptorPoolSize* pPoolSizes,
				VkDescriptorPoolCreateFlags flags = VkDescriptorPoolCreateFlagBits::VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);

			// DescriptorSet
			VkDescriptorSetLayoutBinding DescriptorSetLayoutBinding(uint32_t binding, VkDescriptorType descriptorType, uint32_t descriptorCount,
				VkShaderStageFlags stageFlags, const VkSampler* pImmutableSamplers = nullptr);

			VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo(uint32_t bindingCount, const VkDescriptorSetLayoutBinding* pBindings,
				VkDescriptorSetLayoutCreateFlags flags = 0);

			VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo(VkDescriptorPool descriptorPool, uint32_t descriptorSetCount, const VkDescriptorSetLayout* pSetLayouts);

			VkCopyDescriptorSet CopyDescriptorSet(VkDescriptorSet srcSet, uint32_t srcBinding, uint32_t srcArrayElement, VkDescriptorSet dstSet, uint32_t dstBinding,
				uint32_t dstArrayElement, uint32_t descriptorCount);

			VkWriteDescriptorSet WriteDescriptorSet(VkDescriptorSet dstSet, uint32_t dstBinding, uint32_t dstArrayElement, uint32_t descriptorCount, VkDescriptorType descriptorType,
				const VkDescriptorImageInfo* pImageInfo, const VkDescriptorBufferInfo* pBufferInfo, const VkBufferView* pTexelBufferView = nullptr);

			VkDescriptorImageInfo DescriptorImageInfo(VkSampler sampler, VkImageView imageView, VkImageLayout imageLayout);

			VkDescriptorBufferInfo DescriptorBufferInfo(VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range);

			// Pipeline
			VkPipelineCacheCreateInfo PipelineCacheCreateInfo(size_t initialDataSize, const void* pInitialData, VkPipelineCacheCreateFlags flags = 0);

			VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo(uint32_t setLayoutCount, const VkDescriptorSetLayout* pSetLayouts, uint32_t pushConstantRangeCount = 0,
				const VkPushConstantRange * pPushConstantRanges = nullptr, VkPipelineLayoutCreateFlags flags = 0);

			VkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo(uint32_t stageCount, const VkPipelineShaderStageCreateInfo* pStages, const VkPipelineVertexInputStateCreateInfo* pVertexInputState,
				const VkPipelineInputAssemblyStateCreateInfo* pInputAssemblyState, const VkPipelineTessellationStateCreateInfo* pTessellationState,
				const VkPipelineViewportStateCreateInfo* pViewportState, const VkPipelineRasterizationStateCreateInfo* pRasterizationState,
				const VkPipelineMultisampleStateCreateInfo* pMultisampleState, const VkPipelineDepthStencilStateCreateInfo* pDepthStencilState,
				const VkPipelineColorBlendStateCreateInfo* pColorBlendState, const VkPipelineDynamicStateCreateInfo* pDynamicState,
				VkPipelineLayout layout, VkRenderPass renderPass, uint32_t subpass, VkPipeline basePipelineHandle = VK_NULL_HANDLE, int32_t basePipelineIndex = 0,
				VkPipelineCreateFlags flags = 0);


			VkComputePipelineCreateInfo ComputePipelineCreateInfo(VkPipelineShaderStageCreateInfo stage, VkPipelineLayout layout, VkPipeline basePipelineHandle, int32_t basePipelineIndex,
				VkPipelineCreateFlags flags = 0);

			// Pipeline Stages and States

			// Shader Stage
			VkPipelineShaderStageCreateInfo PipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule module, const char_t* pName, const VkSpecializationInfo* pSpecializationInfo = nullptr,
				VkPipelineShaderStageCreateFlags flags = 0);

			VkSpecializationInfo SpecializationInfo(uint32_t mapEntryCount, const VkSpecializationMapEntry* pMapEntries, size_t dataSize, const void* pData);

			VkSpecializationMapEntry SpecializationMapEntry(uint32_t constantID, uint32_t offset, size_t size);

			// VertexInput State
			VkPipelineVertexInputStateCreateInfo PipelineVertexInputStateCreateInfo(uint32_t vertexBindingDescriptionCount, const VkVertexInputBindingDescription* pVertexBindingDescriptions,
				uint32_t vertexAttributeDescriptionCount, const VkVertexInputAttributeDescription* pVertexAttributeDescriptions,
				VkPipelineVertexInputStateCreateFlags flags = 0);


			VkVertexInputBindingDescription VertexInputBindingDescription(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate);

			VkVertexInputAttributeDescription VertexInputAttributeDescription(uint32_t location, uint32_t binding, VkFormat format, uint32_t offset);

			// InputAssembly State
			VkPipelineInputAssemblyStateCreateInfo PipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable, VkPipelineInputAssemblyStateCreateFlags flags = 0);


			// Tesselation State
			VkPipelineTessellationStateCreateInfo PipelineTessellationStateCreateInfo(uint32_t patchControlPoints, VkPipelineTessellationStateCreateFlags flags = 0);


			// Viewport State
			VkPipelineViewportStateCreateInfo PipelineViewportStateCreateInfo(uint32_t viewportCount, const VkViewport* pViewports, uint32_t scissorCount, const VkRect2D* pScissors,
				VkPipelineViewportStateCreateFlags flags = 0);

			// Rasterization State
			VkPipelineRasterizationStateCreateInfo PipelineRasterizationStateCreateInfo(VkBool32 depthClampEnable, VkBool32 rasterizerDiscardEnable, VkPolygonMode polygonMode,
				VkCullModeFlags cullMode, VkFrontFace frontFace, VkBool32 depthBiasEnable,
				bfloat32_t depthBiasConstantFactor, bfloat32_t depthBiasClamp, bfloat32_t depthBiasSlopeFactor,
				bfloat32_t lineWidth, VkPipelineRasterizationStateCreateFlags flags = 0);

			// Multisample State
			VkPipelineMultisampleStateCreateInfo PipelineMultisampleStateCreateInfo(VkSampleCountFlagBits rasterizationSamples, VkBool32 sampleShadingEnable, bfloat32_t minSampleShading,
				const VkSampleMask* pSampleMask, VkBool32 alphaToCoverageEnable, VkBool32 alphaToOneEnable,
				VkPipelineMultisampleStateCreateFlags flags = 0);

			// DepthStencil State
			VkPipelineDepthStencilStateCreateInfo PipelineDepthStencilStateCreateInfo(VkBool32 depthTestEnable, VkBool32 depthWriteEnable, VkCompareOp depthCompareOp,
				VkBool32 depthBoundsTestEnable, VkBool32 stencilTestEnable, const VkStencilOpState& front,
				const VkStencilOpState& back, bfloat32_t minDepthBounds, bfloat32_t maxDepthBounds,
				VkPipelineDepthStencilStateCreateFlags flags = 0);

			VkStencilOpState StencilOpState(VkStencilOp failOp, VkStencilOp passOp, VkStencilOp depthFailOp, VkCompareOp compareOp,
				uint32_t compareMask, uint32_t writeMask, uint32_t reference);

			// ColorBlend State
			VkPipelineColorBlendStateCreateInfo PipelineColorBlendStateCreateInfo(VkBool32 logicOpEnable, VkLogicOp logicOp, uint32_t attachmentCount,
				const VkPipelineColorBlendAttachmentState* pAttachments, bfloat32_t blendConstants[4],
				VkPipelineColorBlendStateCreateFlags flags = 0);

			VkPipelineColorBlendAttachmentState PipelineColorBlendAttachmentState(VkBool32 blendEnable, VkBlendFactor srcColorBlendFactor, VkBlendFactor dstColorBlendFactor,
				VkBlendOp colorBlendOp, VkBlendFactor srcAlphaBlendFactor, VkBlendFactor dstAlphaBlendFactor,
				VkBlendOp alphaBlendOp, VkColorComponentFlags colorWriteMask);

			// Dynamic State
			VkPipelineDynamicStateCreateInfo PipelineDynamicStateCreateInfo(uint32_t dynamicStateCount, const VkDynamicState* pDynamicStates, VkPipelineDynamicStateCreateFlags flags = 0);



			// PushConstants
			VkPushConstantRange PushConstantRange(VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size);

			// Viewport
			VkViewport Viewport(bfloat32_t x, bfloat32_t y, bfloat32_t width, bfloat32_t height, bfloat32_t minDepth, bfloat32_t maxDepth);

			// Offset
			VkOffset2D Offset2D(int32_t x, uint32_t y);

			VkOffset3D Offset3D(int32_t x, uint32_t y, uint32_t z);

			// Extent
			VkExtent2D Extent2D(uint32_t width, uint32_t height);

			VkExtent3D Extent3D(uint32_t width, uint32_t height, uint32_t depth);

			// Rect
			VkRect2D Rect2D(const VkOffset2D& offset, const VkExtent2D& extent);

			// Clear

			//VkClearColorValue - union
			//VkClearColorValue ClearColorValue(float float32[4] = {}, int32_t int32[4] = {}, uint32_t uint32[4] = {});

			VkClearDepthStencilValue ClearDepthStencilValue(bfloat32_t depth, uint32_t stencil);
			// VkClearValue - union
			//VkClearValue ClearValue(VkClearColorValue color, VkClearDepthStencilValue depthStencil);

			VkClearAttachment ClearAttachment(VkImageAspectFlags aspectMask, uint32_t colorAttachment, VkClearValue clearValue);

			VkClearRect ClearRect(VkRect2D rect, uint32_t baseArrayLayer, uint32_t layerCount);

			// SubmitInfo 
			VkSubmitInfo SubmitInfo(uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers, uint32_t waitSemaphoreCount = 0, const VkSemaphore * pWaitSemaphores = nullptr,
				const VkPipelineStageFlags * pWaitDstStageMask = nullptr, uint32_t signalSemaphoreCount = 0, const VkSemaphore * pSignalSemaphores = nullptr);

			// PresentInfo
			VkPresentInfoKHR PresentInfo(uint32_t swapchainCount, const VkSwapchainKHR* pSwapchains, uint32_t waitSemaphoreCount, const VkSemaphore* pWaitSemaphores,
				const uint32_t* pImageIndices, VkResult* pResults = nullptr);

		}
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_INITIALIZERS_HPP