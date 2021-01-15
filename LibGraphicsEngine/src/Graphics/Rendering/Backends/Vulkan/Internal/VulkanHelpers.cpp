#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanHelpers.hpp"
#include "Foundation/Logger.hpp"
#include <sstream>

namespace GraphicsEngine
{
	namespace Graphics
	{
		namespace VulkanHelpers
		{
			std::string GetVulkanVersionStr(uint32_t version)
				//const char* GetVulkanVersionStr(uint32_t version) //memory leak = no free
			{
				std::stringstream ss;
				ss << VK_VERSION_MAJOR(version) << "." << VK_VERSION_MINOR(version) << "." << VK_VERSION_PATCH(version);

				return ss.str();

				/*size_t size = ss.str().length();
				char* pCStr = new char[size + 1];
				::strcpy_s(pCStr, size + 1, ss.str().c_str());
				return pCStr;*/
			}

			std::string ErrorString(VkResult errorCode)
			{
				switch (errorCode)
				{
#define STR(r) case VkResult::VK_ ##r: return #r
					STR(NOT_READY);
					STR(TIMEOUT);
					STR(EVENT_SET);
					STR(EVENT_RESET);
					STR(INCOMPLETE);
					STR(ERROR_OUT_OF_HOST_MEMORY);
					STR(ERROR_OUT_OF_DEVICE_MEMORY);
					STR(ERROR_INITIALIZATION_FAILED);
					STR(ERROR_DEVICE_LOST);
					STR(ERROR_MEMORY_MAP_FAILED);
					STR(ERROR_LAYER_NOT_PRESENT);
					STR(ERROR_EXTENSION_NOT_PRESENT);
					STR(ERROR_FEATURE_NOT_PRESENT);
					STR(ERROR_INCOMPATIBLE_DRIVER);
					STR(ERROR_TOO_MANY_OBJECTS);
					STR(ERROR_FORMAT_NOT_SUPPORTED);
					STR(ERROR_FRAGMENTED_POOL);
					STR(ERROR_OUT_OF_POOL_MEMORY);
					STR(ERROR_INVALID_EXTERNAL_HANDLE);
					STR(ERROR_FRAGMENTATION);
					STR(ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS);
					STR(ERROR_SURFACE_LOST_KHR);
					STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
					STR(SUBOPTIMAL_KHR);
					STR(ERROR_OUT_OF_DATE_KHR);
					STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
					STR(ERROR_VALIDATION_FAILED_EXT);
					STR(ERROR_INVALID_SHADER_NV);
					STR(ERROR_INCOMPATIBLE_VERSION_KHR);
					STR(ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
					STR(ERROR_NOT_PERMITTED_EXT);
					STR(ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
					STR(THREAD_IDLE_KHR);
					STR(THREAD_DONE_KHR);
					STR(OPERATION_DEFERRED_KHR);
					STR(OPERATION_NOT_DEFERRED_KHR);
					STR(PIPELINE_COMPILE_REQUIRED_EXT);
#undef STR
				default:
					return "UNKNOWN_ERROR";
				}
			}

			void ListInstanceSupportedLayers(const std::vector<VkLayerProperties>& supportedInstanceLayers)
			{
				// List Instance extensions
				LOG_INFO("Vulkan Instance Supported Layers: \n##############################################################\n");
				for (const VkLayerProperties& layer : supportedInstanceLayers)
				{
					LOG_INFO("\t%s v%u v%u %s", layer.layerName, layer.specVersion, layer.implementationVersion, layer.description);
				}
				LOG_INFO("##############################################################\n");
			}

			void ListInstanceSupportedExtensions(const std::vector<VkExtensionProperties>& supportedInstanceExtensions)
			{
				// List Instance extensions
				LOG_INFO("Vulkan Instance Supported Extensions: \n##############################################################\n");
				for (const VkExtensionProperties& extension : supportedInstanceExtensions)
				{
					LOG_INFO("\t%s v%u", extension.extensionName, extension.specVersion);
				}
				LOG_INFO("##############################################################\n");
			}

#if defined(VK_VERSION_1_1)
			void ListPhysicalDeviceGroupProperties(const std::vector<VkPhysicalDeviceGroupProperties>& physicalDeviceGroupPropertiesVector)
			{
				// List Device Properties
				LOG_INFO("Vulkan Physical Device Group Properties: \n##############################################################\n");
				for (size_t i = 0;i < physicalDeviceGroupPropertiesVector.size(); ++ i)
				{
					LOG_INFO("deviceGroupProperties[%llu].subsetAllocation: %u", i, physicalDeviceGroupPropertiesVector[i].subsetAllocation);
					LOG_INFO("deviceGroupProperties[%llu].physicalDeviceCount: %u", i, physicalDeviceGroupPropertiesVector[i].physicalDeviceCount);
				}
				LOG_INFO("##############################################################\n");
			}
#endif

			void ListPhysicalDeviceSupportedExtensions(const std::vector<VkExtensionProperties>& physicalDeviceSupportedExtensions)
			{
				// List Device Supported Exensions
				LOG_INFO("Vulkan Physical Device Supported Extensions: \n##############################################################\n");
				for (const VkExtensionProperties& extension : physicalDeviceSupportedExtensions)
				{
					LOG_INFO("\t%s v%u", extension.extensionName, extension.specVersion);
				}
				LOG_INFO("##############################################################\n");
			}

			void ListPhysicalDeviceProperties(const VkPhysicalDeviceProperties& physicalDeviceProperties)
			{
				// List Device Properties
				LOG_INFO("Vulkan Physical Device Properties: \n##############################################################\n");
				LOG_INFO("Vulkan Supported Device Version (apiVersion): %u (v%s)", physicalDeviceProperties.apiVersion, GetVulkanVersionStr(physicalDeviceProperties.apiVersion).c_str());
				LOG_INFO("driverVersion: %u", physicalDeviceProperties.driverVersion);
				LOG_INFO("vendorID: %u", physicalDeviceProperties.vendorID);
				LOG_INFO("deviceID: %u", physicalDeviceProperties.deviceID);
				LOG_INFO("deviceType: %u", physicalDeviceProperties.deviceType);
				LOG_INFO("deviceName: %s", physicalDeviceProperties.deviceName);
				LOG_INFO("limits.maxImageDimension1D: %u", physicalDeviceProperties.limits.maxImageDimension1D);
				LOG_INFO("limits.maxImageDimension2D: %u", physicalDeviceProperties.limits.maxImageDimension2D);
				LOG_INFO("limits.maxImageDimension3D: %u", physicalDeviceProperties.limits.maxImageDimension3D);
				LOG_INFO("limits.maxImageDimensionCube: %u", physicalDeviceProperties.limits.maxImageDimensionCube);
				LOG_INFO("limits.maxImageArrayLayers: %u", physicalDeviceProperties.limits.maxImageArrayLayers);
				LOG_INFO("limits.maxTexelBufferElements: %u", physicalDeviceProperties.limits.maxTexelBufferElements);
				LOG_INFO("limits.maxUniformBufferRange: %u", physicalDeviceProperties.limits.maxUniformBufferRange);
				LOG_INFO("limits.maxStorageBufferRange: %u", physicalDeviceProperties.limits.maxStorageBufferRange);
				LOG_INFO("limits.maxPushConstantsSize: %u", physicalDeviceProperties.limits.maxPushConstantsSize);
				LOG_INFO("limits.maxMemoryAllocationCount: %u", physicalDeviceProperties.limits.maxMemoryAllocationCount);
				LOG_INFO("limits.maxSamplerAllocationCount: %u", physicalDeviceProperties.limits.maxSamplerAllocationCount);
				LOG_INFO("limits.bufferImageGranularity: %llu", physicalDeviceProperties.limits.bufferImageGranularity);
				LOG_INFO("limits.sparseAddressSpaceSize: %llu", physicalDeviceProperties.limits.sparseAddressSpaceSize);
				LOG_INFO("limits.maxBoundDescriptorSets: %u", physicalDeviceProperties.limits.maxBoundDescriptorSets);
				LOG_INFO("limits.maxPerStageDescriptorSamplers: %u", physicalDeviceProperties.limits.maxPerStageDescriptorSamplers);
				LOG_INFO("limits.maxPerStageDescriptorUniformBuffers: %u", physicalDeviceProperties.limits.maxPerStageDescriptorUniformBuffers);
				LOG_INFO("limits.maxPerStageDescriptorStorageBuffers: %u", physicalDeviceProperties.limits.maxPerStageDescriptorStorageBuffers);
				LOG_INFO("limits.maxPerStageDescriptorSampledImages: %u", physicalDeviceProperties.limits.maxPerStageDescriptorSampledImages);
				LOG_INFO("limits.maxPerStageDescriptorStorageImages: %u", physicalDeviceProperties.limits.maxPerStageDescriptorStorageImages);
				LOG_INFO("limits.maxPerStageDescriptorInputAttachments: %u", physicalDeviceProperties.limits.maxPerStageDescriptorInputAttachments);
				LOG_INFO("limits.maxPerStageResources: %u", physicalDeviceProperties.limits.maxPerStageResources);
				LOG_INFO("limits.maxDescriptorSetSamplers: %u", physicalDeviceProperties.limits.maxDescriptorSetSamplers);
				LOG_INFO("limits.maxDescriptorSetUniformBuffers: %u", physicalDeviceProperties.limits.maxDescriptorSetUniformBuffers);
				LOG_INFO("limits.maxDescriptorSetUniformBuffersDynamic: %u", physicalDeviceProperties.limits.maxDescriptorSetUniformBuffersDynamic);
				LOG_INFO("limits.maxDescriptorSetStorageBuffers: %u", physicalDeviceProperties.limits.maxDescriptorSetStorageBuffers);
				LOG_INFO("limits.maxDescriptorSetStorageBuffersDynamic: %u", physicalDeviceProperties.limits.maxDescriptorSetStorageBuffersDynamic);
				LOG_INFO("limits.maxDescriptorSetSampledImages: %u", physicalDeviceProperties.limits.maxDescriptorSetSampledImages);
				LOG_INFO("limits.maxDescriptorSetStorageImages: %u", physicalDeviceProperties.limits.maxDescriptorSetStorageImages);
				LOG_INFO("limits.maxDescriptorSetInputAttachments: %u", physicalDeviceProperties.limits.maxDescriptorSetInputAttachments);
				LOG_INFO("limits.maxVertexInputAttributes: %u", physicalDeviceProperties.limits.maxVertexInputAttributes);
				LOG_INFO("limits.maxVertexInputBindings: %u", physicalDeviceProperties.limits.maxVertexInputBindings);
				LOG_INFO("limits.maxVertexInputAttributeOffset: %u", physicalDeviceProperties.limits.maxVertexInputAttributeOffset);
				LOG_INFO("limits.maxVertexInputBindingStride: %u", physicalDeviceProperties.limits.maxVertexInputBindingStride);
				LOG_INFO("limits.maxVertexOutputComponents: %u", physicalDeviceProperties.limits.maxVertexOutputComponents);
				LOG_INFO("limits.maxTessellationGenerationLevel: %u", physicalDeviceProperties.limits.maxTessellationGenerationLevel);
				LOG_INFO("limits.maxTessellationPatchSize: %u", physicalDeviceProperties.limits.maxTessellationPatchSize);
				LOG_INFO("limits.maxTessellationControlPerVertexInputComponents: %u", physicalDeviceProperties.limits.maxTessellationControlPerVertexInputComponents);
				LOG_INFO("limits.maxTessellationControlPerVertexOutputComponents: %u", physicalDeviceProperties.limits.maxTessellationControlPerVertexOutputComponents);
				LOG_INFO("limits.maxTessellationControlPerPatchOutputComponents: %u", physicalDeviceProperties.limits.maxTessellationControlPerPatchOutputComponents);
				LOG_INFO("limits.maxTessellationControlTotalOutputComponents: %u", physicalDeviceProperties.limits.maxTessellationControlTotalOutputComponents);
				LOG_INFO("limits.maxTessellationEvaluationInputComponents: %u", physicalDeviceProperties.limits.maxTessellationEvaluationInputComponents);
				LOG_INFO("limits.maxTessellationEvaluationOutputComponents: %u", physicalDeviceProperties.limits.maxTessellationEvaluationOutputComponents);
				LOG_INFO("limits.maxGeometryShaderInvocations: %u", physicalDeviceProperties.limits.maxGeometryShaderInvocations);
				LOG_INFO("limits.maxGeometryInputComponents: %u", physicalDeviceProperties.limits.maxGeometryInputComponents);
				LOG_INFO("limits.maxGeometryOutputComponents: %u", physicalDeviceProperties.limits.maxGeometryOutputComponents);
				LOG_INFO("limits.maxGeometryOutputVertices: %u", physicalDeviceProperties.limits.maxGeometryOutputVertices);
				LOG_INFO("limits.maxGeometryTotalOutputComponents: %u", physicalDeviceProperties.limits.maxGeometryTotalOutputComponents);
				LOG_INFO("limits.maxFragmentInputComponents: %u", physicalDeviceProperties.limits.maxFragmentInputComponents);
				LOG_INFO("limits.maxFragmentOutputAttachments: %u", physicalDeviceProperties.limits.maxFragmentOutputAttachments);
				LOG_INFO("limits.maxFragmentDualSrcAttachments: %u", physicalDeviceProperties.limits.maxFragmentDualSrcAttachments);
				LOG_INFO("limits.maxFragmentCombinedOutputResources:%u", physicalDeviceProperties.limits.maxFragmentCombinedOutputResources);
				LOG_INFO("limits.maxComputeSharedMemorySize: %u", physicalDeviceProperties.limits.maxComputeSharedMemorySize);
				LOG_INFO("limits.maxComputeWorkGroupCount: %u %u %u", physicalDeviceProperties.limits.maxComputeWorkGroupCount[0], physicalDeviceProperties.limits.maxComputeWorkGroupCount[1], physicalDeviceProperties.limits.maxComputeWorkGroupCount[2]);
				LOG_INFO("limits.maxComputeWorkGroupInvocations: %u", physicalDeviceProperties.limits.maxComputeWorkGroupInvocations);
				LOG_INFO("limits.maxComputeWorkGroupSize: %u %u %u", physicalDeviceProperties.limits.maxComputeWorkGroupSize[0], physicalDeviceProperties.limits.maxComputeWorkGroupSize[1], physicalDeviceProperties.limits.maxComputeWorkGroupSize[2]);
				LOG_INFO("limits.subPixelPrecisionBits: %u", physicalDeviceProperties.limits.subPixelPrecisionBits);
				LOG_INFO("limits.subTexelPrecisionBits: %u", physicalDeviceProperties.limits.subTexelPrecisionBits);
				LOG_INFO("limits.mipmapPrecisionBits: %u", physicalDeviceProperties.limits.mipmapPrecisionBits);
				LOG_INFO("limits.maxDrawIndexedIndexValue: %u", physicalDeviceProperties.limits.maxDrawIndexedIndexValue);
				LOG_INFO("limits.maxDrawIndirectCount: %u", physicalDeviceProperties.limits.maxDrawIndirectCount);
				LOG_INFO("limits.maxSamplerLodBias: %f", physicalDeviceProperties.limits.maxSamplerLodBias);
				LOG_INFO("limits.maxSamplerAnisotropy: %f", physicalDeviceProperties.limits.maxSamplerAnisotropy);
				LOG_INFO("limits.maxViewports: %u", physicalDeviceProperties.limits.maxViewports);
				LOG_INFO("limits.maxViewportDimensions: %u %u", physicalDeviceProperties.limits.maxViewportDimensions[0], physicalDeviceProperties.limits.maxViewportDimensions[1]);
				LOG_INFO("limits.viewportBoundsRange: %f %f", physicalDeviceProperties.limits.viewportBoundsRange[0], physicalDeviceProperties.limits.viewportBoundsRange[1]);
				LOG_INFO("limits.viewportSubPixelBits: %u", physicalDeviceProperties.limits.viewportSubPixelBits);
				LOG_INFO("limits.minMemoryMapAlignment: %llu", physicalDeviceProperties.limits.minMemoryMapAlignment);
				LOG_INFO("limits.minTexelBufferOffsetAlignment: %llu", physicalDeviceProperties.limits.minTexelBufferOffsetAlignment);
				LOG_INFO("limits.minUniformBufferOffsetAlignment: %llu", physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);
				LOG_INFO("limits.minStorageBufferOffsetAlignment: %llu", physicalDeviceProperties.limits.minStorageBufferOffsetAlignment);
				LOG_INFO("limits.minTexelOffset: %u", physicalDeviceProperties.limits.minTexelOffset);
				LOG_INFO("limits.maxTexelOffset: %u", physicalDeviceProperties.limits.maxTexelOffset);
				LOG_INFO("limits.minTexelGatherOffset: %u", physicalDeviceProperties.limits.minTexelGatherOffset);
				LOG_INFO("limits.maxTexelGatherOffset: %u", physicalDeviceProperties.limits.maxTexelGatherOffset);
				LOG_INFO("limits.minInterpolationOffset: %f", physicalDeviceProperties.limits.minInterpolationOffset);
				LOG_INFO("limits.maxInterpolationOffset: %f", physicalDeviceProperties.limits.maxInterpolationOffset);
				LOG_INFO("limits.subPixelInterpolationOffsetBits: %u", physicalDeviceProperties.limits.subPixelInterpolationOffsetBits);
				LOG_INFO("limits.maxFramebufferWidth: %u", physicalDeviceProperties.limits.maxFramebufferWidth);
				LOG_INFO("limits.maxFramebufferHeight: %u", physicalDeviceProperties.limits.maxFramebufferHeight);
				LOG_INFO("limits.maxFramebufferLayers: %u", physicalDeviceProperties.limits.maxFramebufferLayers);
				LOG_INFO("limits.framebufferColorSampleCounts: %u", physicalDeviceProperties.limits.framebufferColorSampleCounts);
				LOG_INFO("limits.framebufferDepthSampleCounts: %u", physicalDeviceProperties.limits.framebufferDepthSampleCounts);
				LOG_INFO("limits.framebufferStencilSampleCounts: %u", physicalDeviceProperties.limits.framebufferStencilSampleCounts);
				LOG_INFO("limits.framebufferNoAttachmentsSampleCounts: %u", physicalDeviceProperties.limits.framebufferNoAttachmentsSampleCounts);
				LOG_INFO("limits.maxColorAttachments: %u", physicalDeviceProperties.limits.maxColorAttachments);
				LOG_INFO("limits.sampledImageColorSampleCounts: %u", physicalDeviceProperties.limits.sampledImageColorSampleCounts);
				LOG_INFO("limits.sampledImageIntegerSampleCounts: %u", physicalDeviceProperties.limits.sampledImageIntegerSampleCounts);
				LOG_INFO("limits.sampledImageDepthSampleCounts: %u", physicalDeviceProperties.limits.sampledImageDepthSampleCounts);
				LOG_INFO("limits.sampledImageStencilSampleCounts: %u", physicalDeviceProperties.limits.sampledImageStencilSampleCounts);
				LOG_INFO("limits.storageImageSampleCounts: %u", physicalDeviceProperties.limits.storageImageSampleCounts);
				LOG_INFO("limits.maxSampleMaskWords: %u", physicalDeviceProperties.limits.maxSampleMaskWords);
				LOG_INFO("limits.timestampComputeAndGraphics: %u", physicalDeviceProperties.limits.timestampComputeAndGraphics);
				LOG_INFO("limits.timestampPeriod: %f", physicalDeviceProperties.limits.timestampPeriod);
				LOG_INFO("limits.maxClipDistances: %u", physicalDeviceProperties.limits.maxClipDistances);
				LOG_INFO("limits.maxCullDistances: %u", physicalDeviceProperties.limits.maxCullDistances);
				LOG_INFO("limits.maxCombinedClipAndCullDistances: %u", physicalDeviceProperties.limits.maxCombinedClipAndCullDistances);
				LOG_INFO("limits.discreteQueuePriorities: %u", physicalDeviceProperties.limits.discreteQueuePriorities);
				LOG_INFO("limits.pointSizeRange: %f %f", physicalDeviceProperties.limits.pointSizeRange[0], physicalDeviceProperties.limits.pointSizeRange[1]);
				LOG_INFO("limits.lineWidthRange: %f %f", physicalDeviceProperties.limits.lineWidthRange[0], physicalDeviceProperties.limits.lineWidthRange[1]);
				LOG_INFO("limits.pointSizeGranularity: %f", physicalDeviceProperties.limits.pointSizeGranularity);
				LOG_INFO("limits.lineWidthGranularity: %f", physicalDeviceProperties.limits.lineWidthGranularity);
				LOG_INFO("limits.strictLines: %u", physicalDeviceProperties.limits.strictLines);
				LOG_INFO("limits.standardSampleLocations: %u", physicalDeviceProperties.limits.standardSampleLocations);
				LOG_INFO("limits.optimalBufferCopyOffsetAlignment: %llu", physicalDeviceProperties.limits.optimalBufferCopyOffsetAlignment);
				LOG_INFO("limits.optimalBufferCopyRowPitchAlignment: %llu", physicalDeviceProperties.limits.optimalBufferCopyRowPitchAlignment);
				LOG_INFO("limits.nonCoherentAtomSize: %llu", physicalDeviceProperties.limits.nonCoherentAtomSize);
				LOG_INFO("##############################################################\n");
			}

			void ListPhysicalDeviceFeatures(const VkPhysicalDeviceFeatures& physicalDeviceFeatures)
			{
				// List Device Features
				LOG_INFO("Vulkan Physical Device Features: \n##############################################################\n");
				LOG_INFO("robustBufferAccess: %u", physicalDeviceFeatures.robustBufferAccess);
				LOG_INFO("fullDrawIndexUint32: %u", physicalDeviceFeatures.fullDrawIndexUint32);
				LOG_INFO("imageCubeArray: %u", physicalDeviceFeatures.imageCubeArray);
				LOG_INFO("independentBlend: %u", physicalDeviceFeatures.independentBlend);
				LOG_INFO("geometryShader: %u", physicalDeviceFeatures.geometryShader);
				LOG_INFO("tessellationShader: %u", physicalDeviceFeatures.tessellationShader);
				LOG_INFO("sampleRateShading: %u", physicalDeviceFeatures.sampleRateShading);
				LOG_INFO("dualSrcBlend: %u", physicalDeviceFeatures.dualSrcBlend);
				LOG_INFO("logicOp: %u", physicalDeviceFeatures.logicOp);
				LOG_INFO("multiDrawIndirect: %u", physicalDeviceFeatures.multiDrawIndirect);
				LOG_INFO("drawIndirectFirstInstance: %u", physicalDeviceFeatures.drawIndirectFirstInstance);
				LOG_INFO("depthClamp: %u", physicalDeviceFeatures.depthClamp);
				LOG_INFO("depthBiasClamp: %u", physicalDeviceFeatures.depthBiasClamp);
				LOG_INFO("fillModeNonSolid: %u", physicalDeviceFeatures.fillModeNonSolid);
				LOG_INFO("depthBounds: %u", physicalDeviceFeatures.depthBounds);
				LOG_INFO("wideLines: %u", physicalDeviceFeatures.wideLines);
				LOG_INFO("largePoints: %u", physicalDeviceFeatures.largePoints);
				LOG_INFO("alphaToOne: %u", physicalDeviceFeatures.alphaToOne);
				LOG_INFO("multiViewport: %u", physicalDeviceFeatures.multiViewport);
				LOG_INFO("samplerAnisotropy: %u", physicalDeviceFeatures.samplerAnisotropy);
				LOG_INFO("textureCompressionETC2: %u", physicalDeviceFeatures.textureCompressionETC2);
				LOG_INFO("textureCompressionASTC_LDR: %u", physicalDeviceFeatures.textureCompressionASTC_LDR);
				LOG_INFO("textureCompressionBC: %u", physicalDeviceFeatures.textureCompressionBC);
				LOG_INFO("occlusionQueryPrecise: %u", physicalDeviceFeatures.occlusionQueryPrecise);
				LOG_INFO("pipelineStatisticsQuery: %u", physicalDeviceFeatures.pipelineStatisticsQuery);
				LOG_INFO("vertexPipelineStoresAndAtomics: %u", physicalDeviceFeatures.vertexPipelineStoresAndAtomics);
				LOG_INFO("fragmentStoresAndAtomics: %u", physicalDeviceFeatures.fragmentStoresAndAtomics);
				LOG_INFO("shaderTessellationAndGeometryPointSize: %u", physicalDeviceFeatures.shaderTessellationAndGeometryPointSize);
				LOG_INFO("shaderImageGatherExtended: %u", physicalDeviceFeatures.shaderImageGatherExtended);
				LOG_INFO("shaderStorageImageExtendedFormats: %u", physicalDeviceFeatures.shaderStorageImageExtendedFormats);
				LOG_INFO("shaderStorageImageMultisample: %u", physicalDeviceFeatures.shaderStorageImageMultisample);
				LOG_INFO("shaderStorageImageReadWithoutFormat: %u", physicalDeviceFeatures.shaderStorageImageReadWithoutFormat);
				LOG_INFO("shaderStorageImageWriteWithoutFormat: %u", physicalDeviceFeatures.shaderStorageImageWriteWithoutFormat);
				LOG_INFO("shaderUniformBufferArrayDynamicIndexing: %u", physicalDeviceFeatures.shaderUniformBufferArrayDynamicIndexing);
				LOG_INFO("shaderSampledImageArrayDynamicIndexing: %u", physicalDeviceFeatures.shaderSampledImageArrayDynamicIndexing);
				LOG_INFO("shaderStorageBufferArrayDynamicIndexing: %u", physicalDeviceFeatures.shaderStorageBufferArrayDynamicIndexing);
				LOG_INFO("shaderStorageImageArrayDynamicIndexing: %u", physicalDeviceFeatures.shaderStorageImageArrayDynamicIndexing);
				LOG_INFO("shaderClipDistance: %u", physicalDeviceFeatures.shaderClipDistance);
				LOG_INFO("shaderCullDistance: %u", physicalDeviceFeatures.shaderCullDistance);
				LOG_INFO("shaderFloat64: %u", physicalDeviceFeatures.shaderFloat64);
				LOG_INFO("shaderInt64: %u", physicalDeviceFeatures.shaderInt64);
				LOG_INFO("shaderInt16: %u", physicalDeviceFeatures.shaderInt16);
				LOG_INFO("shaderResourceResidency: %u", physicalDeviceFeatures.shaderResourceResidency);
				LOG_INFO("shaderResourceMinLod: %u", physicalDeviceFeatures.shaderResourceMinLod);
				LOG_INFO("sparseBinding: %u", physicalDeviceFeatures.sparseBinding);
				LOG_INFO("sparseResidencyBuffer: %u", physicalDeviceFeatures.sparseResidencyBuffer);
				LOG_INFO("sparseResidencyImage2D: %u", physicalDeviceFeatures.sparseResidencyImage2D);
				LOG_INFO("sparseResidencyImage3D: %u", physicalDeviceFeatures.sparseResidencyImage3D);
				LOG_INFO("sparseResidency2Samples: %u", physicalDeviceFeatures.sparseResidency2Samples);
				LOG_INFO("sparseResidency4Samples: %u", physicalDeviceFeatures.sparseResidency4Samples);
				LOG_INFO("sparseResidency8Samples: %u", physicalDeviceFeatures.sparseResidency8Samples);
				LOG_INFO("sparseResidency16Samples: %u", physicalDeviceFeatures.sparseResidency16Samples);
				LOG_INFO("sparseResidencyAliased: %u", physicalDeviceFeatures.sparseResidencyAliased);
				LOG_INFO("variableMultisampleRate: %u", physicalDeviceFeatures.variableMultisampleRate);
				LOG_INFO("inheritedQueries: %u", physicalDeviceFeatures.inheritedQueries);
				LOG_INFO("##############################################################\n");
			}

			void ListPhysicalDeviceMemoryProperties(const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties)
			{
				const double _1Gb = 1073741824.0; // in bytes

				// List Device Memory Properties
				LOG_INFO("Vulkan Physical Device Memory Properties: \n##############################################################\n");
				LOG_INFO("memoryTypeCount: %u", physicalDeviceMemoryProperties.memoryTypeCount);
				LOG_INFO("memoryHeapCount: %u", physicalDeviceMemoryProperties.memoryHeapCount);
				for (size_t i = 0; i < physicalDeviceMemoryProperties.memoryHeapCount; ++i)
				{
					LOG_INFO("memoryHeapSize[%llu]: %.3lf (Gb)", i, (physicalDeviceMemoryProperties.memoryHeaps[i].size / _1Gb));
				}
				LOG_INFO("##############################################################\n");
			}

#if defined(VK_VERSION_1_1) && defined(VK_VERSION_1_2) //for Vulkan 1.2 and Vulkan 1.1 which ahs the KHR driver extensiosn
			void ListPhysicalDeviceDriverProperties(const VkPhysicalDeviceDriverProperties& physicalDeviceDriverProperties)
			{
				// List Device Driver Properties
				LOG_INFO("Vulkan Physical Device Driver Properties: \n##############################################################\n");
				LOG_INFO("driverID: %u", physicalDeviceDriverProperties.driverID);
				LOG_INFO("driverName: %s", physicalDeviceDriverProperties.driverName);
				LOG_INFO("driverInfo: %s", physicalDeviceDriverProperties.driverInfo);
				LOG_INFO("conformanceVersion.major: %u", physicalDeviceDriverProperties.conformanceVersion.major);
				LOG_INFO("conformanceVersion.minor: %u", physicalDeviceDriverProperties.conformanceVersion.minor);
				LOG_INFO("conformanceVersion.subminor: %u", physicalDeviceDriverProperties.conformanceVersion.subminor);
				LOG_INFO("conformanceVersion.patch: %u", physicalDeviceDriverProperties.conformanceVersion.patch);
				LOG_INFO("##############################################################\n");
			}
#endif // defined(VK_VERSION_1_1) && defined(VK_VERSION_1_2)
		}
	}
}