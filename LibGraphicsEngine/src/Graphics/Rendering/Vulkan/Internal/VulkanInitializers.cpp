#include "VulkanInitializers.hpp"
#include <cstring>

namespace GraphicsEngine
{
	namespace Graphics
	{
		namespace VulkanInitializers
		{
			// Debug
			VkDebugReportCallbackCreateInfoEXT DebugReportCallbackCreateInfo(PFN_vkDebugReportCallbackEXT pfnCallback, VkDebugReportFlagsEXT flags)
			{
				VkDebugReportCallbackCreateInfoEXT debugReportCallbackCreateInfo {};
				debugReportCallbackCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
				debugReportCallbackCreateInfo.pNext = nullptr;
				debugReportCallbackCreateInfo.flags = flags;
				debugReportCallbackCreateInfo.pfnCallback = pfnCallback;
				debugReportCallbackCreateInfo.pUserData = nullptr;

				return debugReportCallbackCreateInfo;
			}

			VkDebugMarkerObjectNameInfoEXT DebugMarkerObjectNameInfo(VkDebugReportObjectTypeEXT objectType, uint64_t object, const char_t* pObjectName)
			{
				VkDebugMarkerObjectNameInfoEXT debugMarkerObjectNameInfo = {};
				debugMarkerObjectNameInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT;
				debugMarkerObjectNameInfo.pNext = nullptr;
				debugMarkerObjectNameInfo.objectType = objectType;
				debugMarkerObjectNameInfo.object = object;
				debugMarkerObjectNameInfo.pObjectName = pObjectName;

				return debugMarkerObjectNameInfo;
			}

			VkDebugMarkerObjectTagInfoEXT DebugMarkerObjectTagInfo(VkDebugReportObjectTypeEXT objectType, uint64_t object, uint64_t tagName, size_t tagSize, const void* pTag)
			{
				VkDebugMarkerObjectTagInfoEXT debugMarkerObjectTagInfo {};
				debugMarkerObjectTagInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_TAG_INFO_EXT;
				debugMarkerObjectTagInfo.pNext = nullptr;
				debugMarkerObjectTagInfo.objectType = objectType;
				debugMarkerObjectTagInfo.object = object;
				debugMarkerObjectTagInfo.tagName = tagName;
				debugMarkerObjectTagInfo.tagSize = tagSize;
				debugMarkerObjectTagInfo.pTag = pTag;

				return debugMarkerObjectTagInfo;
			}

			VkDebugMarkerMarkerInfoEXT DebugMarkerMarkerInfo(const char_t* pMarkerName, float32_t color[4])
			{
				VkDebugMarkerMarkerInfoEXT debugMarkerMarkerInfo {};
				debugMarkerMarkerInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
				debugMarkerMarkerInfo.pNext = nullptr;
				debugMarkerMarkerInfo.pMarkerName = pMarkerName;
				::memcpy(debugMarkerMarkerInfo.color, &color[0], sizeof(float32_t) * 4);

				return debugMarkerMarkerInfo;
			}

			// Instance
			VkApplicationInfo ApplicationInfo(const char_t* pApplicationName, uint32_t applicationVersion, const char_t* pEngineName, uint32_t engineVersion, uint32_t apiVersion)
			{
				VkApplicationInfo applicationInfo {};
				applicationInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_APPLICATION_INFO;
				applicationInfo.pNext = nullptr;
				applicationInfo.pApplicationName = pApplicationName;
				applicationInfo.applicationVersion = applicationVersion;
				applicationInfo.pEngineName = pEngineName;
				applicationInfo.engineVersion = engineVersion;
				applicationInfo.apiVersion = apiVersion;

				return applicationInfo;
			}

			VkInstanceCreateInfo InstanceCreateInfo(const VkApplicationInfo* pApplicationInfo, uint32_t enabledExtensionCount, const char_t* const* ppEnabledExtensionNames,
													uint32_t enabledLayerCount, const char_t* const* ppEnabledLayerNames, VkInstanceCreateFlags flags)
			{
				VkInstanceCreateInfo instanceCreateInfo {};
				instanceCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
				instanceCreateInfo.pNext = nullptr;
				instanceCreateInfo.flags = flags;
				instanceCreateInfo.pApplicationInfo = pApplicationInfo;
				if (enabledLayerCount > 0)
				{
					instanceCreateInfo.enabledLayerCount = enabledLayerCount;
					instanceCreateInfo.ppEnabledLayerNames = ppEnabledLayerNames;
				}
				if (enabledExtensionCount > 0)
				{
					instanceCreateInfo.enabledExtensionCount = enabledExtensionCount;
					instanceCreateInfo.ppEnabledExtensionNames = ppEnabledExtensionNames;
				}

				return instanceCreateInfo;
			}

			// Device
#if defined(VK_VERSION_1_1)
			VkPhysicalDeviceGroupProperties PhysicalDeviceGroupProperties()
			{
				VkPhysicalDeviceGroupProperties physicalDeviceGroupProperties {};
				physicalDeviceGroupProperties.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GROUP_PROPERTIES;

				return physicalDeviceGroupProperties;
			}

			VkPhysicalDeviceProperties2 PhysicalDeviceProperties2()
			{
				VkPhysicalDeviceProperties2 physicalDeviceProperties2 {};
				physicalDeviceProperties2.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

				return physicalDeviceProperties2;
			}
#endif // defined(VK_VERSION_1_1)

#if defined(VK_VERSION_1_2)
			VkPhysicalDeviceDriverProperties PhysicalDeviceDriverProperties()
			{
				VkPhysicalDeviceDriverProperties physicalDeviceDriverProperties {};
				physicalDeviceDriverProperties.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES;

				return physicalDeviceDriverProperties;
			}
#else //extension
			VkPhysicalDeviceDriverPropertiesKHR PhysicalDeviceDriverPropertiesKHR()
			{
				VkPhysicalDeviceDriverPropertiesKHR physicalDeviceDriverPropertiesKHR {};
				physicalDeviceDriverPropertiesKHR.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES_KHR;

				return physicalDeviceDriverPropertiesKHR;
			}
#endif // defined(VK_VERSION_1_2)

			VkDeviceCreateInfo DeviceCreateInfo(uint32_t queueCreateInfoCount, const VkDeviceQueueCreateInfo* pQueueCreateInfos, const VkPhysicalDeviceFeatures* pEnabledFeatures,
												uint32_t enabledExtensionCount, const char_t* const* ppEnabledExtensionNames,
												uint32_t enabledLayerCount, const char_t* const* ppEnabledLayerNames, VkDeviceCreateFlags flags)
			{
				VkDeviceCreateInfo deviceCreateInfo {};
				deviceCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
				deviceCreateInfo.pNext = nullptr;
				deviceCreateInfo.flags = flags;
				deviceCreateInfo.queueCreateInfoCount = queueCreateInfoCount;
				deviceCreateInfo.pQueueCreateInfos = pQueueCreateInfos;
				if (enabledLayerCount > 0)
				{
					deviceCreateInfo.enabledLayerCount = enabledLayerCount;
					deviceCreateInfo.ppEnabledLayerNames = ppEnabledLayerNames;
				}
				if (enabledExtensionCount > 0)
				{
					deviceCreateInfo.enabledExtensionCount = enabledExtensionCount;
					deviceCreateInfo.ppEnabledExtensionNames = ppEnabledExtensionNames;
				}
				deviceCreateInfo.pEnabledFeatures = pEnabledFeatures;

				return deviceCreateInfo;
			}

			// Queue
			VkDeviceQueueCreateInfo DeviceQueueCreateInfo(uint32_t queueFamilyIndex, uint32_t queueCount, const float32_t* pQueuePriorities, VkDeviceQueueCreateFlags flags)
			{
				VkDeviceQueueCreateInfo deviceQueueCreateInfo {};
				deviceQueueCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				deviceQueueCreateInfo.pNext = nullptr;
				deviceQueueCreateInfo.flags = flags;
				deviceQueueCreateInfo.queueFamilyIndex = queueFamilyIndex;
				deviceQueueCreateInfo.queueCount = queueCount;
				deviceQueueCreateInfo.pQueuePriorities = pQueuePriorities;

				return deviceQueueCreateInfo;
			}

			// Surface
			// No Initializer for surface - platform dependent - dealt with in VulkanDevice::CreateSurface()

			// SwapChain
			VkSwapchainCreateInfoKHR SwapchainCreateInfoKHR(VkSurfaceKHR surface, uint32_t minImageCount, VkFormat imageFormat, VkColorSpaceKHR imageColorSpace, const VkExtent2D& imageExtent,
															uint32_t imageArrayLayers, VkImageUsageFlags imageUsage, VkSharingMode imageSharingMode, uint32_t queueFamilyIndexCount,
															const uint32_t* pQueueFamilyIndices, VkSurfaceTransformFlagBitsKHR preTransform, VkCompositeAlphaFlagBitsKHR compositeAlpha,
															VkPresentModeKHR presentMode, VkBool32 clipped, VkSwapchainKHR oldSwapchain, VkSwapchainCreateFlagsKHR flags)
			{
				VkSwapchainCreateInfoKHR swapchainCreateInfo {};
				swapchainCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
				swapchainCreateInfo.pNext = nullptr;
				swapchainCreateInfo.flags = flags;
				swapchainCreateInfo.surface = surface;
				swapchainCreateInfo.minImageCount = minImageCount;
				swapchainCreateInfo.imageFormat = imageFormat;
				swapchainCreateInfo.imageColorSpace = imageColorSpace;
				swapchainCreateInfo.imageExtent = imageExtent;
				swapchainCreateInfo.imageArrayLayers = imageArrayLayers;
				swapchainCreateInfo.imageUsage = imageUsage;
				swapchainCreateInfo.imageSharingMode = imageSharingMode;
				swapchainCreateInfo.queueFamilyIndexCount = queueFamilyIndexCount;
				swapchainCreateInfo.pQueueFamilyIndices = pQueueFamilyIndices;
				swapchainCreateInfo.preTransform = preTransform;
				swapchainCreateInfo.compositeAlpha = compositeAlpha;
				swapchainCreateInfo.presentMode = presentMode;
				swapchainCreateInfo.clipped = clipped;
				swapchainCreateInfo.oldSwapchain = oldSwapchain;

				return swapchainCreateInfo;
			}

			// Memory
			VkMemoryAllocateInfo MemoryAllocateInfo(VkDeviceSize allocationSize, uint32_t memoryTypeIndex)
			{
				VkMemoryAllocateInfo memoryAllocateInfo {};
				memoryAllocateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				memoryAllocateInfo.pNext = nullptr;
				memoryAllocateInfo.allocationSize = allocationSize;
				memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;

				return memoryAllocateInfo;
			}

			VkMappedMemoryRange MappedMemoryRange(VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size)
			{
				VkMappedMemoryRange mappedMemoryRange {};
				mappedMemoryRange.sType = VkStructureType::VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
				mappedMemoryRange.pNext = nullptr;
				mappedMemoryRange.memory = memory;
				mappedMemoryRange.offset = offset;
				mappedMemoryRange.size = size;

				return mappedMemoryRange;
			}

			// Buffer
			VkBufferCreateInfo BufferCreateInfo(VkDeviceSize size, VkBufferUsageFlags usage, VkSharingMode sharingMode, uint32_t queueFamilyIndexCount, const uint32_t* pQueueFamilyIndices,
												VkBufferCreateFlags flags)
			{
				VkBufferCreateInfo bufferCreateInfo {};
				bufferCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
				bufferCreateInfo.pNext = nullptr;
				bufferCreateInfo.flags = flags;
				bufferCreateInfo.size = size;
				bufferCreateInfo.usage = usage;
				bufferCreateInfo.sharingMode = sharingMode;
				bufferCreateInfo.queueFamilyIndexCount = queueFamilyIndexCount;
				bufferCreateInfo.pQueueFamilyIndices = pQueueFamilyIndices;

				return bufferCreateInfo;
			}

			/*VkBufferCopy BufferCopy(VkDeviceSize srcOffset, VkDeviceSize dstOffset, VkDeviceSize size)
			{
				VkBufferCopy bufferCopy {};
				bufferCopy.srcOffset = srcOffset;
				bufferCopy.dstOffset = dstOffset;
				bufferCopy.size = size;

				return bufferCopy;
			}*/

			// BufferView
			VkBufferViewCreateInfo BufferViewCreateInfo(VkBuffer buffer, VkFormat format, VkDeviceSize offset, VkDeviceSize  range, VkBufferViewCreateFlags flags)
			{
				VkBufferViewCreateInfo bufferViewCreateInfo {};
				bufferViewCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
				bufferViewCreateInfo.pNext = nullptr;
				bufferViewCreateInfo.flags = flags;
				bufferViewCreateInfo.buffer = buffer;
				bufferViewCreateInfo.format = format;
				bufferViewCreateInfo.offset = offset;
				bufferViewCreateInfo.range = range;

				return bufferViewCreateInfo;
			}

			// Sampler
			VkSamplerCreateInfo SamplerCreateInfo(VkFilter magFilter, VkFilter  minFilter, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode addressModeU,
													VkSamplerAddressMode addressModeV, VkSamplerAddressMode addressModeW, float32_t mipLodBias, VkBool32 anisotropyEnable,
													float32_t maxAnisotropy, VkBool32 compareEnable, VkCompareOp compareOp, float32_t minLod, float32_t maxLod, VkBorderColor borderColor,
													VkBool32 unnormalizedCoordinates, VkSamplerCreateFlags flags)
			{
				VkSamplerCreateInfo samplerCreateInfo {};
				samplerCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
				samplerCreateInfo.pNext = nullptr;
				samplerCreateInfo.flags = flags;
				samplerCreateInfo.magFilter = magFilter;
				samplerCreateInfo.minFilter = minFilter;
				samplerCreateInfo.mipmapMode = mipmapMode;
				samplerCreateInfo.addressModeU = addressModeU;
				samplerCreateInfo.addressModeV = addressModeV;
				samplerCreateInfo.addressModeW = addressModeW;
				samplerCreateInfo.mipLodBias = mipLodBias;
				samplerCreateInfo.anisotropyEnable = anisotropyEnable;
				samplerCreateInfo.maxAnisotropy = maxAnisotropy;
				samplerCreateInfo.compareEnable = compareEnable;
				samplerCreateInfo.compareOp = compareOp;
				samplerCreateInfo.minLod = minLod;
				samplerCreateInfo.maxLod = maxLod;
				samplerCreateInfo.borderColor = borderColor;
				samplerCreateInfo.unnormalizedCoordinates = unnormalizedCoordinates;

				return samplerCreateInfo;
			}

			// Image
			VkImageCreateInfo ImageCreateInfo(VkImageType imageType, VkFormat format, const VkExtent3D& extent, uint32_t mipLevels, uint32_t arrayLayers, VkSampleCountFlagBits samples,
												VkImageTiling tiling, VkImageUsageFlags usage, VkSharingMode sharingMode, uint32_t queueFamilyIndexCount, const uint32_t* pQueueFamilyIndices,
												VkImageLayout initialLayout, VkImageCreateFlags flags)
			{
				VkImageCreateInfo imageCreateInfo {};

				imageCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
				imageCreateInfo.pNext = nullptr;
				imageCreateInfo.flags = flags;
				imageCreateInfo.imageType = imageType;
				imageCreateInfo.format = format;
				imageCreateInfo.extent = extent;
				imageCreateInfo.mipLevels = mipLevels;
				imageCreateInfo.arrayLayers = arrayLayers;
				imageCreateInfo.samples = samples;
				imageCreateInfo.tiling = tiling;
				imageCreateInfo.usage = usage;
				imageCreateInfo.sharingMode = sharingMode;
				imageCreateInfo.queueFamilyIndexCount = queueFamilyIndexCount;
				imageCreateInfo.pQueueFamilyIndices = pQueueFamilyIndices;
				imageCreateInfo.initialLayout = initialLayout;

				return imageCreateInfo;
			}

			/*VkImageCopy ImageCopy(const VkImageSubresourceLayers& srcSubresource, const VkOffset3D& srcOffset, const VkImageSubresourceLayers& dstSubresource, 
									const VkOffset3D& dstOffset, const VkExtent3D& extent)
			{
				VkImageCopy imageCopy {};
				imageCopy.srcSubresource = srcSubresource;
				imageCopy.srcOffset = srcOffset;
				imageCopy.dstSubresource = dstSubresource;
				imageCopy.dstOffset = dstOffset;
				imageCopy.extent = extent;

				return imageCopy;
			}

			VkImageBlit ImageBlit(const VkImageSubresourceLayers& srcSubresource, VkOffset3D srcOffsets[2], const VkImageSubresourceLayers& dstSubresource, VkOffset3D dstOffsets[2])
			{
				VkImageBlit imageBlit {};
				imageBlit.srcSubresource = srcSubresource;
				::memcpy(imageBlit.srcOffsets, srcOffsets, sizeof(VkOffset3D) * 2);
				imageBlit.dstSubresource = dstSubresource;
				::memcpy(imageBlit.dstOffsets, dstOffsets, sizeof(VkOffset3D) * 2);

				return imageBlit;
			}

			VkBufferImageCopy BufferImageCopy(VkDeviceSize bufferOffset, uint32_t bufferRowLength, uint32_t bufferImageHeight, const VkImageSubresourceLayers& imageSubresource,
												const VkOffset3D& imageOffset, const VkExtent3D& imageExtent)
			{
				VkBufferImageCopy bufferImageCopy {};
				bufferImageCopy.bufferOffset = bufferOffset;
				bufferImageCopy.bufferRowLength = bufferRowLength;
				bufferImageCopy.bufferImageHeight = bufferImageHeight;
				bufferImageCopy.imageSubresource = imageSubresource;
				bufferImageCopy.imageOffset = imageOffset;
				bufferImageCopy.imageExtent = imageExtent;

				return bufferImageCopy;
			}

			VkComponentMapping ComponentMapping(VkComponentSwizzle r, VkComponentSwizzle g, VkComponentSwizzle b, VkComponentSwizzle a)
			{
				VkComponentMapping componentMapping {};
				componentMapping.r = r;
				componentMapping.g = g;
				componentMapping.b = b;
				componentMapping.a = a;

				return componentMapping;
			}

			VkImageSubresourceLayers ImageSubresourceLayers(VkImageAspectFlags aspectMask, uint32_t mipLevel, uint32_t baseArrayLayer, uint32_t layerCount)
			{
				VkImageSubresourceLayers imageSubresourceLayers {};
				imageSubresourceLayers.aspectMask = aspectMask;
				imageSubresourceLayers.mipLevel = mipLevel;
				imageSubresourceLayers.baseArrayLayer = baseArrayLayer;
				imageSubresourceLayers.layerCount = layerCount;

				return imageSubresourceLayers;
			}

			VkSubresourceLayout SubresourceLayout(VkDeviceSize offset, VkDeviceSize size, VkDeviceSize rowPitch, VkDeviceSize arrayPitch, VkDeviceSize depthPitch)
			{
				VkSubresourceLayout subresourceLayout {};
				subresourceLayout.offset = offset;
				subresourceLayout.size = size;
				subresourceLayout.rowPitch = rowPitch;
				subresourceLayout.arrayPitch = arrayPitch;
				subresourceLayout.depthPitch = depthPitch;

				return subresourceLayout;
			}

			VkImageSubresourceRange ImageSubresourceRange(VkImageAspectFlags aspectMask, uint32_t baseMipLevel, uint32_t levelCount, uint32_t baseArrayLayer, uint32_t layerCount)
			{
				VkImageSubresourceRange imageSubresourceRange {};
				imageSubresourceRange.aspectMask = aspectMask;
				imageSubresourceRange.baseMipLevel = baseMipLevel;
				imageSubresourceRange.levelCount = levelCount;
				imageSubresourceRange.baseArrayLayer = baseArrayLayer;
				imageSubresourceRange.layerCount = layerCount;

				return imageSubresourceRange;
			}

			VkImageResolve ImageResolve(const VkImageSubresourceLayers& srcSubresource, const VkOffset3D& srcOffset, const VkImageSubresourceLayers& dstSubresource,
										const VkOffset3D& dstOffset, const VkExtent3D& extent)
			{
				VkImageResolve imageResolve {};
				imageResolve.srcSubresource = srcSubresource;
				imageResolve.srcOffset = srcOffset;
				imageResolve.dstSubresource = dstSubresource;
				imageResolve.dstOffset = dstOffset;
				imageResolve.extent = extent;

				return imageResolve;
			}*/

			// ImageView
			VkImageViewCreateInfo ImageViewCreateInfo(VkImage image, VkImageViewType viewType, VkFormat format, const VkComponentMapping& components, const VkImageSubresourceRange& subresourceRange,
														VkImageViewCreateFlags flags)
			{
				VkImageViewCreateInfo imageViewCreateInfo {};
				imageViewCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				imageViewCreateInfo.pNext = nullptr;
				imageViewCreateInfo.flags = flags;
				imageViewCreateInfo.image = image;
				imageViewCreateInfo.viewType = viewType;
				imageViewCreateInfo.format = format;
				imageViewCreateInfo.components = components;
				imageViewCreateInfo.subresourceRange = subresourceRange;

				return imageViewCreateInfo;
			}

			// ShaderModule
			VkShaderModuleCreateInfo ShaderModuleCreateInfo(size_t codeSize, const uint32_t* pCode, VkShaderModuleCreateFlags flags)
			{
				VkShaderModuleCreateInfo shaderModuleCreateInfo {};
				shaderModuleCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				shaderModuleCreateInfo.pNext = nullptr;
				shaderModuleCreateInfo.flags = flags;
				shaderModuleCreateInfo.codeSize = codeSize;
				shaderModuleCreateInfo.pCode = pCode;

				return shaderModuleCreateInfo;
			}

			//// Attachment
			//VkAttachmentDescription AttachmentDescription(VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp,
			//												VkAttachmentLoadOp stencilLoadOp, VkAttachmentStoreOp stencilStoreOp, VkImageLayout initialLayout,
			//												VkImageLayout finalLayout, VkAttachmentDescriptionFlags flags)
			//{
			//	VkAttachmentDescription attachmentDescription {};
			//	attachmentDescription.flags = flags;
			//	attachmentDescription.format = format;
			//	attachmentDescription.samples = samples;
			//	attachmentDescription.loadOp = loadOp;
			//	attachmentDescription.storeOp = storeOp;
			//	attachmentDescription.stencilLoadOp = stencilLoadOp;
			//	attachmentDescription.stencilStoreOp = stencilStoreOp;
			//	attachmentDescription.initialLayout = initialLayout;
			//	attachmentDescription.finalLayout = finalLayout;

			//	return attachmentDescription;
			//}

			//VkAttachmentReference AttachmentReference(uint32_t attachment, VkImageLayout layout)
			//{
			//	VkAttachmentReference attachmentReference {};
			//	attachmentReference.attachment = attachment;
			//	attachmentReference.layout = layout;

			//	return attachmentReference;
			//}

			//// Subpass
			//VkSubpassDescription SubpassDescription(VkPipelineBindPoint pipelineBindPoint, uint32_t colorAttachmentCount, const VkAttachmentReference* pColorAttachments,
			//										const VkAttachmentReference* pDepthStencilAttachment, uint32_t inputAttachmentCount, const VkAttachmentReference* pInputAttachments,
			//										const VkAttachmentReference* pResolveAttachments, uint32_t preserveAttachmentCount, const uint32_t* pPreserveAttachments,
			//										VkSubpassDescriptionFlags flags)
			//{
			//	VkSubpassDescription subpassDescription {};
			//	subpassDescription.flags = flags;
			//	subpassDescription.pipelineBindPoint = pipelineBindPoint;
			//	subpassDescription.inputAttachmentCount = inputAttachmentCount;
			//	subpassDescription.pInputAttachments = pInputAttachments;
			//	subpassDescription.colorAttachmentCount = colorAttachmentCount;
			//	subpassDescription.pColorAttachments = pColorAttachments;
			//	subpassDescription.pResolveAttachments = pResolveAttachments;
			//	subpassDescription.pDepthStencilAttachment = pDepthStencilAttachment;
			//	subpassDescription.preserveAttachmentCount = preserveAttachmentCount;
			//	subpassDescription.pPreserveAttachments = pPreserveAttachments;

			//	return subpassDescription;
			//}

			//// Subpass
			//VkSubpassDependency SubpassDependency(uint32_t srcSubpass, uint32_t dstSubpass, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
			//										VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkDependencyFlags dependencyFlags)
			//{
			//	VkSubpassDependency subpassDependency {};
			//	subpassDependency.srcSubpass = srcSubpass;
			//	subpassDependency.dstSubpass = dstSubpass;
			//	subpassDependency.srcStageMask = srcStageMask;
			//	subpassDependency.dstStageMask = dstStageMask;
			//	subpassDependency.srcAccessMask = srcAccessMask;
			//	subpassDependency.dstAccessMask = dstAccessMask;
			//	subpassDependency.dependencyFlags = dependencyFlags;

			//	return subpassDependency;
			//}

			// RenderPass
			VkRenderPassCreateInfo RenderPassCreateInfo(uint32_t attachmentCount, const VkAttachmentDescription* pAttachments, uint32_t subpassCount, 
														const VkSubpassDescription* pSubpasses, uint32_t dependencyCount, const VkSubpassDependency* pDependencies,
														VkRenderPassCreateFlags flags)
			{
				VkRenderPassCreateInfo renderPassCreateInfo {};
				renderPassCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
				renderPassCreateInfo.flags = flags;
				renderPassCreateInfo.pNext = nullptr;
				renderPassCreateInfo.attachmentCount = attachmentCount;
				renderPassCreateInfo.pAttachments = pAttachments;
				renderPassCreateInfo.subpassCount = subpassCount;
				renderPassCreateInfo.pSubpasses = pSubpasses;
				renderPassCreateInfo.dependencyCount = dependencyCount;
				renderPassCreateInfo.pDependencies = pDependencies;

				return renderPassCreateInfo;
			}

			VkRenderPassBeginInfo RenderPassBeginInfo(VkRenderPass renderPass, VkFramebuffer framebuffer, const VkRect2D& renderArea, uint32_t clearValueCount,
														const VkClearValue* pClearValues)
			{
				VkRenderPassBeginInfo renderPassBeginInfo {};
				renderPassBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				renderPassBeginInfo.pNext = nullptr;
				renderPassBeginInfo.renderPass = renderPass;
				renderPassBeginInfo.framebuffer = framebuffer;
				renderPassBeginInfo.renderArea = renderArea;
				renderPassBeginInfo.clearValueCount = clearValueCount;
				renderPassBeginInfo.pClearValues = pClearValues;

				return renderPassBeginInfo;
			}

			// Framebuffer
			VkFramebufferCreateInfo FramebufferCreateInfo(VkRenderPass renderPass, uint32_t attachmentCount, const VkImageView* pAttachments, uint32_t width,
															uint32_t height, uint32_t layers, VkFramebufferCreateFlags flags)
			{
				VkFramebufferCreateInfo framebufferCreateInfo {};
				framebufferCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				framebufferCreateInfo.pNext = nullptr;
				framebufferCreateInfo.flags = flags;
				framebufferCreateInfo.renderPass = renderPass;
				framebufferCreateInfo.attachmentCount = attachmentCount;
				framebufferCreateInfo.pAttachments = pAttachments;
				framebufferCreateInfo.width = width;
				framebufferCreateInfo.height = height;
				framebufferCreateInfo.layers = layers;

				return framebufferCreateInfo;
			}


			// CommandPool
			VkCommandPoolCreateInfo CommandPoolCreateInfo(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags)
			{
				VkCommandPoolCreateInfo commandPoolCreateInfo {};
				commandPoolCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
				commandPoolCreateInfo.pNext = nullptr;
				commandPoolCreateInfo.flags = flags;
				commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;

				return commandPoolCreateInfo;
			}

			// CommandBuffer
			VkCommandBufferAllocateInfo CommandBufferAllocateInfo(VkCommandPool commandPool, VkCommandBufferLevel level, uint32_t commandBufferCount)
			{
				VkCommandBufferAllocateInfo commandBufferAllocateInfo {};
				commandBufferAllocateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				commandBufferAllocateInfo.pNext = nullptr;
				commandBufferAllocateInfo.commandPool = commandPool;
				commandBufferAllocateInfo.level = level;
				commandBufferAllocateInfo.commandBufferCount = commandBufferCount;

				return commandBufferAllocateInfo;
			}

			VkCommandBufferInheritanceInfo CommandBufferInheritanceInfo(VkRenderPass renderPass, uint32_t subpass, VkFramebuffer framebuffer, VkBool32 occlusionQueryEnable,
																		VkQueryControlFlags queryFlags, VkQueryPipelineStatisticFlags pipelineStatistics)
			{
				VkCommandBufferInheritanceInfo commandBufferInheritanceInfo{};
				commandBufferInheritanceInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
				commandBufferInheritanceInfo.pNext = nullptr;
				commandBufferInheritanceInfo.renderPass = renderPass;
				commandBufferInheritanceInfo.subpass = subpass;
				commandBufferInheritanceInfo.framebuffer = framebuffer;
				commandBufferInheritanceInfo.occlusionQueryEnable = occlusionQueryEnable;
				commandBufferInheritanceInfo.queryFlags = queryFlags;
				commandBufferInheritanceInfo.pipelineStatistics = pipelineStatistics;

				return commandBufferInheritanceInfo;
			}

			VkCommandBufferBeginInfo CommandBufferBeginInfo(const VkCommandBufferInheritanceInfo* pInheritanceInfo, VkCommandBufferUsageFlags flags)
			{
				VkCommandBufferBeginInfo commandBufferBeginInfo {};
				commandBufferBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				commandBufferBeginInfo.pNext = nullptr;
				commandBufferBeginInfo.flags = flags;
				commandBufferBeginInfo.pInheritanceInfo = pInheritanceInfo;

				return commandBufferBeginInfo;
			}

			// Semaphore
			VkSemaphoreCreateInfo SemaphoreCreateInfo(VkSemaphoreCreateFlags flags)
			{
				VkSemaphoreCreateInfo semaphoreCreateInfo {};
				semaphoreCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
				semaphoreCreateInfo.pNext = nullptr;
				semaphoreCreateInfo.flags = flags;

				return semaphoreCreateInfo;
			}

			// Fence
			VkFenceCreateInfo FenceCreateInfo(VkFenceCreateFlags flags)
			{
				VkFenceCreateInfo fenceCreateInfo {};
				fenceCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
				fenceCreateInfo.pNext = nullptr;
				fenceCreateInfo.flags = flags;

				return fenceCreateInfo;
			}

			// Event
			VkEventCreateInfo EventCreateInfo(VkEventCreateFlags flags)
			{
				VkEventCreateInfo eventCreateInfo {};
				eventCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_EVENT_CREATE_INFO;
				eventCreateInfo.pNext = nullptr;
				eventCreateInfo.flags = flags;

				return eventCreateInfo;
			}

			// MemoryBarrier
			VkMemoryBarrier MemoryBarrier(VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask)
			{
				VkMemoryBarrier memoryBarrier {};
				memoryBarrier.sType = VkStructureType::VK_STRUCTURE_TYPE_MEMORY_BARRIER;
				memoryBarrier.pNext = nullptr;
				memoryBarrier.srcAccessMask = srcAccessMask;
				memoryBarrier.dstAccessMask = dstAccessMask;

				return memoryBarrier;
			}

			VkBufferMemoryBarrier BufferMemoryBarrier(VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, uint32_t srcQueueFamilyIndex, uint32_t dstQueueFamilyIndex,
														VkBuffer buffer, VkDeviceSize offset, VkDeviceSize size)
			{
				VkBufferMemoryBarrier bufferMemoryBarrier {};
				bufferMemoryBarrier.sType = VkStructureType::VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
				bufferMemoryBarrier.pNext = nullptr;
				bufferMemoryBarrier.srcAccessMask = srcAccessMask;
				bufferMemoryBarrier.dstAccessMask = srcAccessMask;
				bufferMemoryBarrier.srcQueueFamilyIndex = srcAccessMask;
				bufferMemoryBarrier.dstQueueFamilyIndex = srcAccessMask;
				bufferMemoryBarrier.buffer = buffer;
				bufferMemoryBarrier.offset = offset;
				bufferMemoryBarrier.size = size;

				return bufferMemoryBarrier;
			}

			VkImageMemoryBarrier ImageMemoryBarrier(VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldLayout, VkImageLayout newLayout, 
													uint32_t srcQueueFamilyIndex, uint32_t dstQueueFamilyIndex, VkImage image, const VkImageSubresourceRange& subresourceRange)
			{
				VkImageMemoryBarrier imageMemoryBarrier {};
				imageMemoryBarrier.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				imageMemoryBarrier.pNext = nullptr;
				imageMemoryBarrier.srcAccessMask = srcAccessMask;
				imageMemoryBarrier.dstAccessMask = dstAccessMask;
				imageMemoryBarrier.oldLayout = oldLayout;
				imageMemoryBarrier.newLayout = newLayout;
				imageMemoryBarrier.srcQueueFamilyIndex = srcQueueFamilyIndex;
				imageMemoryBarrier.dstQueueFamilyIndex = dstQueueFamilyIndex;
				imageMemoryBarrier.image = image;
				imageMemoryBarrier.subresourceRange = subresourceRange;

				return imageMemoryBarrier;
			}

			// QueryPool
			VkQueryPoolCreateInfo QueryPoolCreateInfo(VkQueryType queryType, uint32_t queryCount, VkQueryPipelineStatisticFlags pipelineStatistics, VkQueryPoolCreateFlags flags)
			{
				VkQueryPoolCreateInfo queryPoolCreateInfo {};
				queryPoolCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
				queryPoolCreateInfo.pNext = nullptr;
				queryPoolCreateInfo.flags = flags;
				queryPoolCreateInfo.queryType = queryType;
				queryPoolCreateInfo.queryCount = queryCount;
				queryPoolCreateInfo.pipelineStatistics = pipelineStatistics;

				return queryPoolCreateInfo;
			}

			//// DescriptorPool
			//VkDescriptorPoolSize DescriptorPoolSize(VkDescriptorType type, uint32_t descriptorCount)
			//{
			//	VkDescriptorPoolSize descriptorPoolSize{};
			//	descriptorPoolSize.type = type;
			//	descriptorPoolSize.descriptorCount = descriptorCount;

			//	return descriptorPoolSize;
			//}

			VkDescriptorPoolCreateInfo DescriptorPoolCreateInfo(uint32_t maxSets, uint32_t poolSizeCount, const VkDescriptorPoolSize* pPoolSizes,
																VkDescriptorPoolCreateFlags flags)
			{
				VkDescriptorPoolCreateInfo descriptorPoolCreateInfo {};
				descriptorPoolCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
				descriptorPoolCreateInfo.pNext = nullptr;
				descriptorPoolCreateInfo.flags = flags;
				descriptorPoolCreateInfo.maxSets = maxSets;
				descriptorPoolCreateInfo.poolSizeCount = poolSizeCount;
				descriptorPoolCreateInfo.pPoolSizes = pPoolSizes;

				return descriptorPoolCreateInfo;
			}

			//// DescriptorSet
			//VkDescriptorSetLayoutBinding DescriptorSetLayoutBinding(uint32_t binding, VkDescriptorType descriptorType, uint32_t descriptorCount,
			//														VkShaderStageFlags stageFlags, const VkSampler* pImmutableSamplers)
			//{
			//	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding {};
			//	descriptorSetLayoutBinding.binding = binding;
			//	descriptorSetLayoutBinding.descriptorType = descriptorType;
			//	descriptorSetLayoutBinding.descriptorCount = descriptorCount;
			//	descriptorSetLayoutBinding.stageFlags = stageFlags;
			//	descriptorSetLayoutBinding.pImmutableSamplers = pImmutableSamplers;

			//	return descriptorSetLayoutBinding;
			//}

			VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo(uint32_t bindingCount, const VkDescriptorSetLayoutBinding* pBindings,
																			VkDescriptorSetLayoutCreateFlags flags)
			{
				VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo {};
				descriptorSetLayoutCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				descriptorSetLayoutCreateInfo.pNext = nullptr;
				descriptorSetLayoutCreateInfo.flags = flags;
				descriptorSetLayoutCreateInfo.bindingCount = bindingCount;
				descriptorSetLayoutCreateInfo.pBindings = pBindings;

				return descriptorSetLayoutCreateInfo;
			}

			VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo(VkDescriptorPool descriptorPool, uint32_t descriptorSetCount, const VkDescriptorSetLayout* pSetLayouts)
			{
				VkDescriptorSetAllocateInfo descriptorSetAllocateInfo {};
				descriptorSetAllocateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
				descriptorSetAllocateInfo.pNext = nullptr;
				descriptorSetAllocateInfo.descriptorPool = descriptorPool;
				descriptorSetAllocateInfo.descriptorSetCount = descriptorSetCount;
				descriptorSetAllocateInfo.pSetLayouts = pSetLayouts;

				return descriptorSetAllocateInfo;
			}

			VkCopyDescriptorSet CopyDescriptorSet(VkDescriptorSet srcSet, uint32_t srcBinding, uint32_t srcArrayElement, VkDescriptorSet dstSet, uint32_t dstBinding,
													uint32_t dstArrayElement, uint32_t descriptorCount)
			{
				VkCopyDescriptorSet copyDescriptorSet {};
				copyDescriptorSet.sType = VkStructureType::VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
				copyDescriptorSet.pNext = nullptr;
				copyDescriptorSet.srcSet = srcSet;
				copyDescriptorSet.srcBinding = srcBinding;
				copyDescriptorSet.srcArrayElement = srcArrayElement;
				copyDescriptorSet.dstSet = dstSet;
				copyDescriptorSet.dstBinding = dstBinding;
				copyDescriptorSet.dstArrayElement = dstArrayElement;
				copyDescriptorSet.descriptorCount = descriptorCount;

				return copyDescriptorSet;
			}

			VkWriteDescriptorSet WriteDescriptorSet(VkDescriptorSet dstSet, uint32_t dstBinding, uint32_t dstArrayElement, uint32_t descriptorCount, VkDescriptorType descriptorType, 
													const VkDescriptorImageInfo* pImageInfo, const VkDescriptorBufferInfo* pBufferInfo, const VkBufferView* pTexelBufferView)
			{
				VkWriteDescriptorSet writeDescriptorSet {};
				writeDescriptorSet.sType = VkStructureType::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				writeDescriptorSet.pNext = nullptr;
				writeDescriptorSet.dstSet = dstSet;
				writeDescriptorSet.dstBinding = dstBinding;
				writeDescriptorSet.dstArrayElement = dstArrayElement;
				writeDescriptorSet.descriptorCount = descriptorCount;
				writeDescriptorSet.descriptorType = descriptorType;
				writeDescriptorSet.pImageInfo = pImageInfo;
				writeDescriptorSet.pBufferInfo = pBufferInfo;
				writeDescriptorSet.pTexelBufferView = pTexelBufferView;

				return writeDescriptorSet;
			}

			/*VkDescriptorImageInfo DescriptorImageInfo(VkSampler sampler, VkImageView imageView, VkImageLayout imageLayout)
			{
				VkDescriptorImageInfo descriptorImageInfo{};
				descriptorImageInfo.sampler = sampler;
				descriptorImageInfo.imageView = imageView;
				descriptorImageInfo.imageLayout = imageLayout;

				return descriptorImageInfo;
			}

			VkDescriptorBufferInfo DescriptorBufferInfo(VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range)
			{
				VkDescriptorBufferInfo descriptorBufferInfo {};
				descriptorBufferInfo.buffer = buffer;
				descriptorBufferInfo.offset = offset;
				descriptorBufferInfo.range = range;

				return descriptorBufferInfo;
			}*/

			// Pipeline
			VkPipelineCacheCreateInfo PipelineCacheCreateInfo(size_t initialDataSize, const void* pInitialData, VkPipelineCacheCreateFlags flags)
			{
				VkPipelineCacheCreateInfo pipelineCacheCreateInfo {};
				pipelineCacheCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
				pipelineCacheCreateInfo.pNext = nullptr;
				pipelineCacheCreateInfo.flags = flags;
				pipelineCacheCreateInfo.initialDataSize = initialDataSize;
				pipelineCacheCreateInfo.pInitialData = pInitialData;

				return pipelineCacheCreateInfo;
			}

			VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo(uint32_t setLayoutCount, const VkDescriptorSetLayout* pSetLayouts, uint32_t pushConstantRangeCount,
																const VkPushConstantRange* pPushConstantRanges, VkPipelineLayoutCreateFlags flags)
			{
				VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo {};
				pipelineLayoutCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
				pipelineLayoutCreateInfo.pNext = nullptr;
				pipelineLayoutCreateInfo.flags = flags;
				pipelineLayoutCreateInfo.setLayoutCount = setLayoutCount;
				pipelineLayoutCreateInfo.pSetLayouts = pSetLayouts;
				pipelineLayoutCreateInfo.pushConstantRangeCount = pushConstantRangeCount;
				pipelineLayoutCreateInfo.pPushConstantRanges = pPushConstantRanges;

				return pipelineLayoutCreateInfo;
			}

			VkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo(uint32_t stageCount, const VkPipelineShaderStageCreateInfo* pStages, const VkPipelineVertexInputStateCreateInfo* pVertexInputState,
																	const VkPipelineInputAssemblyStateCreateInfo* pInputAssemblyState, const VkPipelineTessellationStateCreateInfo* pTessellationState, 
																	const VkPipelineViewportStateCreateInfo* pViewportState, const VkPipelineRasterizationStateCreateInfo* pRasterizationState,
																	const VkPipelineMultisampleStateCreateInfo* pMultisampleState, const VkPipelineDepthStencilStateCreateInfo* pDepthStencilState, 
																	const VkPipelineColorBlendStateCreateInfo* pColorBlendState, const VkPipelineDynamicStateCreateInfo* pDynamicState,
																	VkPipelineLayout layout, VkRenderPass renderPass, uint32_t subpass, VkPipeline basePipelineHandle, int32_t basePipelineIndex, 
																	VkPipelineCreateFlags flags)
			{
				VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo {};
				graphicsPipelineCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
				graphicsPipelineCreateInfo.pNext = nullptr;
				graphicsPipelineCreateInfo.flags = flags;
				graphicsPipelineCreateInfo.stageCount = stageCount;
				graphicsPipelineCreateInfo.pStages = pStages;
				graphicsPipelineCreateInfo.pVertexInputState = pVertexInputState;
				graphicsPipelineCreateInfo.pInputAssemblyState = pInputAssemblyState;
				graphicsPipelineCreateInfo.pTessellationState = pTessellationState;
				graphicsPipelineCreateInfo.pViewportState = pViewportState;
				graphicsPipelineCreateInfo.pRasterizationState = pRasterizationState;
				graphicsPipelineCreateInfo.pMultisampleState = pMultisampleState;
				graphicsPipelineCreateInfo.pDepthStencilState = pDepthStencilState;
				graphicsPipelineCreateInfo.pColorBlendState = pColorBlendState;
				graphicsPipelineCreateInfo.pDynamicState = pDynamicState;
				graphicsPipelineCreateInfo.layout = layout;
				graphicsPipelineCreateInfo.renderPass = renderPass;
				graphicsPipelineCreateInfo.subpass = subpass;
				graphicsPipelineCreateInfo.basePipelineHandle = basePipelineHandle;
				graphicsPipelineCreateInfo.basePipelineIndex = basePipelineIndex;

				return graphicsPipelineCreateInfo;
			}

			VkComputePipelineCreateInfo ComputePipelineCreateInfo(VkPipelineShaderStageCreateInfo stage, VkPipelineLayout layout, VkPipeline basePipelineHandle, int32_t basePipelineIndex, 
																	VkPipelineCreateFlags flags)
			{
				VkComputePipelineCreateInfo computePipelineCreateInfo {};
				computePipelineCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
				computePipelineCreateInfo.pNext = nullptr;
				computePipelineCreateInfo.flags = flags;
				computePipelineCreateInfo.stage = stage;
				computePipelineCreateInfo.layout = layout;
				computePipelineCreateInfo.basePipelineHandle = basePipelineHandle;
				computePipelineCreateInfo.basePipelineIndex = basePipelineIndex;

				return computePipelineCreateInfo;
			}

			// Pipeline Stages and States

			// Shader Stage
			VkPipelineShaderStageCreateInfo PipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule module, const char_t* pName, const VkSpecializationInfo* pSpecializationInfo,
																			VkPipelineShaderStageCreateFlags flags)
			{
				VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo {};
				pipelineShaderStageCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				pipelineShaderStageCreateInfo.pNext = nullptr;
				pipelineShaderStageCreateInfo.flags = flags;
				pipelineShaderStageCreateInfo.stage = stage;
				pipelineShaderStageCreateInfo.module = module;
				pipelineShaderStageCreateInfo.pName = pName;
				pipelineShaderStageCreateInfo.pSpecializationInfo = pSpecializationInfo;

				return pipelineShaderStageCreateInfo;
			}

		/*	VkSpecializationInfo SpecializationInfo(uint32_t mapEntryCount, const VkSpecializationMapEntry* pMapEntries, size_t dataSize, const void* pData)
			{
				VkSpecializationInfo specializationInfo {};
				specializationInfo.mapEntryCount = mapEntryCount;
				specializationInfo.pMapEntries = pMapEntries;
				specializationInfo.dataSize = dataSize;
				specializationInfo.pData = pData;

				return specializationInfo;
			}

			VkSpecializationMapEntry SpecializationMapEntry(uint32_t constantID, uint32_t offset, size_t size)
			{
				VkSpecializationMapEntry specializationMapEntry {};
				specializationMapEntry.constantID = constantID;
				specializationMapEntry.offset = offset;
				specializationMapEntry.size = size;

				return specializationMapEntry;
			}*/

			// VertexInput State
			VkPipelineVertexInputStateCreateInfo PipelineVertexInputStateCreateInfo(uint32_t vertexBindingDescriptionCount, const VkVertexInputBindingDescription* pVertexBindingDescriptions, 
																					uint32_t vertexAttributeDescriptionCount, const VkVertexInputAttributeDescription* pVertexAttributeDescriptions, 
																					VkPipelineVertexInputStateCreateFlags flags)
			{
				VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo {};
				pipelineVertexInputStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
				pipelineVertexInputStateCreateInfo.pNext = nullptr;
				pipelineVertexInputStateCreateInfo.flags = flags;
				pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = vertexBindingDescriptionCount;
				pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = pVertexBindingDescriptions;
				pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = vertexAttributeDescriptionCount;
				pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = pVertexAttributeDescriptions;

				return pipelineVertexInputStateCreateInfo;
			}

		/*	VkVertexInputBindingDescription VertexInputBindingDescription(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate)
			{
				VkVertexInputBindingDescription vertexInputBindingDescription {};
				vertexInputBindingDescription.binding = binding;
				vertexInputBindingDescription.stride = stride;
				vertexInputBindingDescription.inputRate = inputRate;

				return vertexInputBindingDescription;
			}

			VkVertexInputAttributeDescription VertexInputAttributeDescription(uint32_t location, uint32_t binding, VkFormat format, uint32_t offset)
			{
				VkVertexInputAttributeDescription vertexInputAttributeDescription {};
				vertexInputAttributeDescription.location = location;
				vertexInputAttributeDescription.binding = binding;
				vertexInputAttributeDescription.format = format;
				vertexInputAttributeDescription.offset = offset;

				return vertexInputAttributeDescription;
			}*/

			// InputAssembly State
			VkPipelineInputAssemblyStateCreateInfo PipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable, VkPipelineInputAssemblyStateCreateFlags flags)
			{
				VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo {};
				pipelineInputAssemblyStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
				pipelineInputAssemblyStateCreateInfo.pNext = nullptr;
				pipelineInputAssemblyStateCreateInfo.flags = flags;
				pipelineInputAssemblyStateCreateInfo.topology = topology;
				pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = primitiveRestartEnable;

				return pipelineInputAssemblyStateCreateInfo;
			}

			// Tesselation State
			VkPipelineTessellationStateCreateInfo PipelineTessellationStateCreateInfo(uint32_t patchControlPoints, VkPipelineTessellationStateCreateFlags flags)
			{
				VkPipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo {};
				pipelineTessellationStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
				pipelineTessellationStateCreateInfo.pNext = nullptr;
				pipelineTessellationStateCreateInfo.flags = flags;
				pipelineTessellationStateCreateInfo.patchControlPoints = patchControlPoints;

				return pipelineTessellationStateCreateInfo;
			}

			// Viewport State
			VkPipelineViewportStateCreateInfo PipelineViewportStateCreateInfo(uint32_t viewportCount, const VkViewport* pViewports, uint32_t scissorCount, const VkRect2D* pScissors, 
																				VkPipelineViewportStateCreateFlags flags)
			{
				VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo {};
				pipelineViewportStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
				pipelineViewportStateCreateInfo.pNext = nullptr;
				pipelineViewportStateCreateInfo.flags = flags;
				pipelineViewportStateCreateInfo.viewportCount = viewportCount;
				pipelineViewportStateCreateInfo.pViewports = pViewports;
				pipelineViewportStateCreateInfo.scissorCount = scissorCount;
				pipelineViewportStateCreateInfo.pScissors = pScissors;

				return pipelineViewportStateCreateInfo;
			}

			// Rasterization State
			VkPipelineRasterizationStateCreateInfo PipelineRasterizationStateCreateInfo(VkBool32 depthClampEnable, VkBool32 rasterizerDiscardEnable, VkPolygonMode polygonMode, 
																						VkCullModeFlags cullMode, VkFrontFace frontFace, VkBool32 depthBiasEnable, 
																						float32_t depthBiasConstantFactor, float32_t depthBiasClamp, float32_t depthBiasSlopeFactor,
																						float32_t lineWidth, VkPipelineRasterizationStateCreateFlags flags)
			{
				VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo {};
				pipelineRasterizationStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
				pipelineRasterizationStateCreateInfo.pNext = nullptr;
				pipelineRasterizationStateCreateInfo.flags = flags;
				pipelineRasterizationStateCreateInfo.depthClampEnable = depthClampEnable;
				pipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = rasterizerDiscardEnable;
				pipelineRasterizationStateCreateInfo.polygonMode = polygonMode;
				pipelineRasterizationStateCreateInfo.cullMode = cullMode;
				pipelineRasterizationStateCreateInfo.frontFace = frontFace;
				pipelineRasterizationStateCreateInfo.depthBiasEnable = depthBiasEnable;
				pipelineRasterizationStateCreateInfo.depthBiasConstantFactor = depthBiasConstantFactor;
				pipelineRasterizationStateCreateInfo.depthBiasClamp = depthBiasClamp;
				pipelineRasterizationStateCreateInfo.depthBiasSlopeFactor = depthBiasSlopeFactor;
				pipelineRasterizationStateCreateInfo.lineWidth = lineWidth;

				return pipelineRasterizationStateCreateInfo;
			}

			// Multisample State
			VkPipelineMultisampleStateCreateInfo PipelineMultisampleStateCreateInfo(VkSampleCountFlagBits rasterizationSamples, VkBool32 sampleShadingEnable, float32_t minSampleShading,
																					const VkSampleMask* pSampleMask, VkBool32 alphaToCoverageEnable, VkBool32 alphaToOneEnable,
																					VkPipelineMultisampleStateCreateFlags flags)
			{
				VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo {};
				pipelineMultisampleStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
				pipelineMultisampleStateCreateInfo.pNext = nullptr;
				pipelineMultisampleStateCreateInfo.flags = flags;
				pipelineMultisampleStateCreateInfo.rasterizationSamples = rasterizationSamples;
				pipelineMultisampleStateCreateInfo.sampleShadingEnable = sampleShadingEnable;
				pipelineMultisampleStateCreateInfo.minSampleShading = minSampleShading;
				pipelineMultisampleStateCreateInfo.pSampleMask = pSampleMask;
				pipelineMultisampleStateCreateInfo.alphaToCoverageEnable = alphaToCoverageEnable;
				pipelineMultisampleStateCreateInfo.alphaToOneEnable = alphaToOneEnable;

				return pipelineMultisampleStateCreateInfo;
			}

			// DepthStencil State
			VkPipelineDepthStencilStateCreateInfo PipelineDepthStencilStateCreateInfo(VkBool32 depthTestEnable, VkBool32 depthWriteEnable, VkCompareOp depthCompareOp, 
																						VkBool32 depthBoundsTestEnable, VkBool32 stencilTestEnable, const VkStencilOpState& front,
																						const VkStencilOpState& back, float32_t minDepthBounds, float32_t maxDepthBounds,
																						VkPipelineDepthStencilStateCreateFlags flags)
			{
				VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo {};
				pipelineDepthStencilStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
				pipelineDepthStencilStateCreateInfo.pNext = nullptr;
				pipelineDepthStencilStateCreateInfo.flags = flags;
				pipelineDepthStencilStateCreateInfo.depthTestEnable = depthTestEnable;
				pipelineDepthStencilStateCreateInfo.depthWriteEnable = depthWriteEnable;
				pipelineDepthStencilStateCreateInfo.depthCompareOp = depthCompareOp;
				pipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = depthBoundsTestEnable;
				pipelineDepthStencilStateCreateInfo.stencilTestEnable = stencilTestEnable;
				pipelineDepthStencilStateCreateInfo.front = front;
				pipelineDepthStencilStateCreateInfo.back = back;
				pipelineDepthStencilStateCreateInfo.minDepthBounds = minDepthBounds;
				pipelineDepthStencilStateCreateInfo.maxDepthBounds = maxDepthBounds;

				return pipelineDepthStencilStateCreateInfo;
			}

		/*	VkStencilOpState StencilOpState(VkStencilOp failOp, VkStencilOp passOp, VkStencilOp depthFailOp, VkCompareOp compareOp, 
											uint32_t compareMask, uint32_t writeMask, uint32_t reference)
			{
				VkStencilOpState stencilOpState {};
				stencilOpState.failOp = failOp;
				stencilOpState.passOp = passOp;
				stencilOpState.depthFailOp = depthFailOp;
				stencilOpState.compareOp = compareOp;
				stencilOpState.compareMask = compareMask;
				stencilOpState.writeMask = writeMask;
				stencilOpState.reference = reference;

				return stencilOpState;
			}*/

			// ColorBlend State
			VkPipelineColorBlendStateCreateInfo PipelineColorBlendStateCreateInfo(VkBool32 logicOpEnable, VkLogicOp logicOp, uint32_t attachmentCount,
																					const VkPipelineColorBlendAttachmentState* pAttachments, float32_t blendConstants[4],
																					VkPipelineColorBlendStateCreateFlags flags)
			{
				VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo {};
				pipelineColorBlendStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
				pipelineColorBlendStateCreateInfo.pNext = nullptr;
				pipelineColorBlendStateCreateInfo.flags = flags;
				pipelineColorBlendStateCreateInfo.logicOpEnable = logicOpEnable;
				pipelineColorBlendStateCreateInfo.logicOp = logicOp;
				pipelineColorBlendStateCreateInfo.attachmentCount = attachmentCount;
				pipelineColorBlendStateCreateInfo.pAttachments = pAttachments;
				::memcpy(pipelineColorBlendStateCreateInfo.blendConstants, blendConstants, sizeof(float32_t) * 4);

				return pipelineColorBlendStateCreateInfo;
			}

		/*	VkPipelineColorBlendAttachmentState PipelineColorBlendAttachmentState(VkBool32 blendEnable, VkBlendFactor srcColorBlendFactor, VkBlendFactor dstColorBlendFactor,
																					VkBlendOp colorBlendOp, VkBlendFactor srcAlphaBlendFactor, VkBlendFactor dstAlphaBlendFactor,
																					VkBlendOp alphaBlendOp, VkColorComponentFlags colorWriteMask)
			{
				VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState {};
				pipelineColorBlendAttachmentState.blendEnable = blendEnable;
				pipelineColorBlendAttachmentState.srcColorBlendFactor = srcColorBlendFactor;
				pipelineColorBlendAttachmentState.dstColorBlendFactor = dstColorBlendFactor;
				pipelineColorBlendAttachmentState.colorBlendOp = colorBlendOp;
				pipelineColorBlendAttachmentState.srcAlphaBlendFactor = srcAlphaBlendFactor;
				pipelineColorBlendAttachmentState.dstAlphaBlendFactor = dstAlphaBlendFactor;
				pipelineColorBlendAttachmentState.alphaBlendOp = alphaBlendOp;
				pipelineColorBlendAttachmentState.colorWriteMask = colorWriteMask;

				return pipelineColorBlendAttachmentState;
			}*/

			// Dynamic State
			VkPipelineDynamicStateCreateInfo PipelineDynamicStateCreateInfo(uint32_t dynamicStateCount, const VkDynamicState* pDynamicStates, VkPipelineDynamicStateCreateFlags flags)
			{
				VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo {};
				pipelineDynamicStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
				pipelineDynamicStateCreateInfo.pNext = nullptr;
				pipelineDynamicStateCreateInfo.flags = flags;
				pipelineDynamicStateCreateInfo.dynamicStateCount = dynamicStateCount;
				pipelineDynamicStateCreateInfo.pDynamicStates = pDynamicStates;

				return pipelineDynamicStateCreateInfo;
			}


			//// PushConstants
			//VkPushConstantRange PushConstantRange(VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size)
			//{
			//	VkPushConstantRange pushConstantRange {};
			//	pushConstantRange.stageFlags = stageFlags;
			//	pushConstantRange.offset = offset;
			//	pushConstantRange.size = size;

			//	return pushConstantRange;
			//}

			//// Viewport
			//VkViewport Viewport(float32_t x, float32_t y, float32_t width, float32_t height, float32_t minDepth, float32_t maxDepth)
			//{
			//	VkViewport viewport {};
			//	viewport.x = x;
			//	viewport.y = y;
			//	viewport.width = width;
			//	viewport.height = height;
			//	viewport.minDepth = minDepth;
			//	viewport.maxDepth = maxDepth;

			//	return viewport;
			//}

			//// Offset
			//VkOffset2D Offset2D(int32_t x, uint32_t y)
			//{
			//	VkOffset2D offset2D {};
			//	offset2D.x = x;
			//	offset2D.y = y;

			//	return offset2D;
			//}

			//VkOffset3D Offset3D(int32_t x, uint32_t y, uint32_t z)
			//{
			//	VkOffset3D offset3D {};
			//	offset3D.x = x;
			//	offset3D.y = y;
			//	offset3D.z = z;

			//	return offset3D;
			//}

			//// Extent
			//VkExtent2D Extent2D(uint32_t width, uint32_t height)
			//{
			//	VkExtent2D extent2D {};
			//	extent2D.width = width;
			//	extent2D.height = height;

			//	return extent2D;
			//}

			//VkExtent3D Extent3D(uint32_t width, uint32_t height, uint32_t depth)
			//{
			//	VkExtent3D extent3D {};
			//	extent3D.width = width;
			//	extent3D.height = height;
			//	extent3D.depth = depth;

			//	return extent3D;
			//}

			//// Rect
			//VkRect2D Rect2D(const VkOffset2D& offset, const VkExtent2D& extent)
			//{
			//	VkRect2D rect2D {};
			//	rect2D.offset = offset;
			//	rect2D.extent = extent;

			//	return rect2D;
			//}

			// Clear
			//VkClearColorValue ClearColorValue(float float32[4], int32_t int32[4], uint32_t uint32[4])
			//{
			//	VkClearColorValue clearColorValue {};
			//	if (float32)
			//		::memcpy(clearColorValue.float32, float32, sizeof(float) * 4);

			//	if (int32)
			//		::memcpy(clearColorValue.int32, int32, sizeof(int32_t) * 4);

			//	if (uint32)
			//		::memcpy(clearColorValue.uint32, uint32, sizeof(uint32_t) * 4);

			//	return clearColorValue;
			//}

		/*	VkClearDepthStencilValue ClearDepthStencilValue(float32_t depth, uint32_t stencil)
			{
				VkClearDepthStencilValue clearDepthStencilValue {};
				clearDepthStencilValue.depth = depth;
				clearDepthStencilValue.stencil = stencil;

				return clearDepthStencilValue;
			}*/

			//VkClearValue ClearValue(VkClearColorValue color, VkClearDepthStencilValue depthStencil)
			//{
			//	VkClearValue clearValue {};

			//	clearValue.color = color;
			//	clearValue.depthStencil = depthStencil;

			//	return clearValue;
			//}

		/*	VkClearAttachment ClearAttachment(VkImageAspectFlags aspectMask, uint32_t colorAttachment, VkClearValue clearValue)
			{
				VkClearAttachment clearAttachment {};
				clearAttachment.aspectMask = aspectMask;
				clearAttachment.colorAttachment = colorAttachment;
				clearAttachment.clearValue = clearValue;

				return clearAttachment;
			}

			VkClearRect ClearRect(VkRect2D rect, uint32_t baseArrayLayer, uint32_t layerCount)
			{
				VkClearRect clearRect {};
				clearRect.rect = rect;
				clearRect.baseArrayLayer = baseArrayLayer;
				clearRect.layerCount = layerCount;

				return clearRect;
			}*/

			// SubmitInfo 
			VkSubmitInfo SubmitInfo(uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers, uint32_t waitSemaphoreCount, const VkSemaphore* pWaitSemaphores,
									const VkPipelineStageFlags* pWaitDstStageMask, uint32_t signalSemaphoreCount, const VkSemaphore* pSignalSemaphores)
			{
				VkSubmitInfo submitInfo {};
				submitInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO;
				submitInfo.pNext = nullptr;
				submitInfo.waitSemaphoreCount = waitSemaphoreCount;
				submitInfo.pWaitSemaphores = pWaitSemaphores;
				submitInfo.pWaitDstStageMask = pWaitDstStageMask;
				submitInfo.commandBufferCount = commandBufferCount;
				submitInfo.pCommandBuffers = pCommandBuffers;
				submitInfo.signalSemaphoreCount = signalSemaphoreCount;
				submitInfo.pSignalSemaphores = pSignalSemaphores;

				return submitInfo;
			}
	
			// PresentInfo
			VkPresentInfoKHR PresentInfo(uint32_t swapchainCount, const VkSwapchainKHR* pSwapchains, uint32_t waitSemaphoreCount, const VkSemaphore* pWaitSemaphores,
											const uint32_t* pImageIndices, VkResult* pResults)
			{
				VkPresentInfoKHR presentInfo {};
				presentInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
				presentInfo.pNext = nullptr;
				presentInfo.waitSemaphoreCount = waitSemaphoreCount;
				presentInfo.pWaitSemaphores = pWaitSemaphores;
				presentInfo.swapchainCount = swapchainCount;
				presentInfo.pSwapchains = pSwapchains;
				presentInfo.pImageIndices = pImageIndices;
				presentInfo.pResults = pResults;

				return presentInfo;
			}
		}
	}
}