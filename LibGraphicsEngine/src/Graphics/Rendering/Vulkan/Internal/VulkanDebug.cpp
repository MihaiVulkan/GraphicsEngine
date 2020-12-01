#include "VulkanDebug.hpp"
#include "VulkanInitializers.hpp"
#include "Foundation/Logger.hpp"
#include <iostream>
#include <sstream>
#include <cassert>

namespace GraphicsEngine
{
	namespace Graphics
	{
		namespace VulkanDebug
		{
			// On desktop the LunarG loaders exposes a meta layer that contains all layers
			//int32_t validationLayerCount = 1;
			//const char* validationLayerNames[] = {
			//	"VK_LAYER_LUNARG_standard_validation",
			//};

			PFN_vkCreateDebugReportCallbackEXT pfnCreateDebugReportCallback = VK_NULL_HANDLE;
			PFN_vkDestroyDebugReportCallbackEXT pfnDestroyDebugReportCallback = VK_NULL_HANDLE;
			PFN_vkDebugReportMessageEXT pfnDbgBreakCallback = VK_NULL_HANDLE;

			VkDebugReportCallbackEXT msgCallback;

			VKAPI_ATTR VkBool32 VKAPI_CALL MessageCallback(
				VkDebugReportFlagsEXT flags,
				VkDebugReportObjectTypeEXT objType,
				uint64_t srcObject,
				size_t location,
				int32_t msgCode,
				const char_t* pLayerPrefix,
				const char_t* pMsg,
				void* pUserData)
			{
				// Select prefix depending on flags passed to the callback
				// Note that multiple flags may be set for a single validation message
				std::string prefix("");

				// Error that may result in undefined behaviour
				if (flags & VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_ERROR_BIT_EXT)
				{
					prefix += "ERROR:";
				};
				// Warnings may hint at unexpected / non-spec API usage
				if (flags & VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_WARNING_BIT_EXT)
				{
					prefix += "WARNING:";
				};
				// May indicate sub-optimal usage of the API
				if (flags & VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
				{
					prefix += "PERFORMANCE:";
				};
				// Informal messages that may become handy during debugging
				if (flags & VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
				{
					prefix += "INFO:";
				}
				// Diagnostic info from the Vulkan loader and layers
				// Usually not helpful in terms of API usage, but may help to debug layer and loader problems 
				if (flags & VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_DEBUG_BIT_EXT)
				{
					prefix += "DEBUG:";
				}

				// Display message to default output (console/logcat)
				std::stringstream debugMessage;
				debugMessage << prefix << " [" << pLayerPrefix << "] Code " << msgCode << " : " << pMsg;

				//if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) 
				//{
				//	std::cerr << debugMessage.str() << std::endl;
				//}
				//else 
				//{
				//	std::cout << debugMessage.str() << std::endl;
				//}

				// info level as we do not want to report the file, func and line, only the message
				LOG_INFO(debugMessage.str().c_str());

				fflush(stdout);

				// The return value of this callback controls wether the Vulkan call that caused
				// the validation message will be aborted or not
				// We return VK_FALSE as we DON'T want Vulkan calls that cause a validation message 
				// (and return a VkResult) to abort
				// If you instead want to have calls abort, pass in VK_TRUE and the function will 
				// return VK_ERROR_VALIDATION_FAILED_EXT 
				return VK_FALSE;
			}

			void SetupDebugging(VkInstance instanceHandle, VkDebugReportFlagsEXT flags, VkDebugReportCallbackEXT callBack)
			{
				pfnCreateDebugReportCallback = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(instanceHandle, "vkCreateDebugReportCallbackEXT"));
				pfnDestroyDebugReportCallback = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(instanceHandle, "vkDestroyDebugReportCallbackEXT"));
				pfnDbgBreakCallback = reinterpret_cast<PFN_vkDebugReportMessageEXT>(vkGetInstanceProcAddr(instanceHandle, "vkDebugReportMessageEXT"));

				VkDebugReportCallbackCreateInfoEXT debugReportCallbackCreateInfo = VulkanInitializers::DebugReportCallbackCreateInfo((PFN_vkDebugReportCallbackEXT)MessageCallback, flags);
				VkResult err = pfnCreateDebugReportCallback(
					instanceHandle,
					&debugReportCallbackCreateInfo,
					nullptr,
					(callBack != VK_NULL_HANDLE) ? &callBack : &msgCallback);
				assert(!err);
			}

			void FreeDebugCallback(VkInstance instanceHandle)
			{
				if (msgCallback != VK_NULL_HANDLE)
				{
					pfnDestroyDebugReportCallback(instanceHandle, msgCallback, nullptr);
				}
			}
		}

		namespace VulkanDebugMarker
		{
			bool_t active = false;

			PFN_vkDebugMarkerSetObjectTagEXT pfnDebugMarkerSetObjectTag = VK_NULL_HANDLE;
			PFN_vkDebugMarkerSetObjectNameEXT pfnDebugMarkerSetObjectName = VK_NULL_HANDLE;
			PFN_vkCmdDebugMarkerBeginEXT pfnCmdDebugMarkerBegin = VK_NULL_HANDLE;
			PFN_vkCmdDebugMarkerEndEXT pfnCmdDebugMarkerEnd = VK_NULL_HANDLE;
			PFN_vkCmdDebugMarkerInsertEXT pfnCmdDebugMarkerInsert = VK_NULL_HANDLE;

			void Setup(VkDevice deviceHandle)
			{
				pfnDebugMarkerSetObjectTag = reinterpret_cast<PFN_vkDebugMarkerSetObjectTagEXT>(vkGetDeviceProcAddr(deviceHandle, "vkDebugMarkerSetObjectTagEXT"));
				pfnDebugMarkerSetObjectName = reinterpret_cast<PFN_vkDebugMarkerSetObjectNameEXT>(vkGetDeviceProcAddr(deviceHandle, "vkDebugMarkerSetObjectNameEXT"));
				pfnCmdDebugMarkerBegin = reinterpret_cast<PFN_vkCmdDebugMarkerBeginEXT>(vkGetDeviceProcAddr(deviceHandle, "vkCmdDebugMarkerBeginEXT"));
				pfnCmdDebugMarkerEnd = reinterpret_cast<PFN_vkCmdDebugMarkerEndEXT>(vkGetDeviceProcAddr(deviceHandle, "vkCmdDebugMarkerEndEXT"));
				pfnCmdDebugMarkerInsert = reinterpret_cast<PFN_vkCmdDebugMarkerInsertEXT>(vkGetDeviceProcAddr(deviceHandle, "vkCmdDebugMarkerInsertEXT"));

				// Set flag if at least one function pointer is present
				active = (pfnDebugMarkerSetObjectName != VK_NULL_HANDLE);
			}

			void SetObjectName(VkDevice deviceHandle, uint64_t object, VkDebugReportObjectTypeEXT objectType, const char_t* pObjectName)
			{
				// Check for valid function pointer (may not be present if not running in a debugging application)
				if (pfnDebugMarkerSetObjectName)
				{
					VkDebugMarkerObjectNameInfoEXT debugMarkerObjectNameInfo = VulkanInitializers::DebugMarkerObjectNameInfo(objectType, object, pObjectName);
					pfnDebugMarkerSetObjectName(deviceHandle, &debugMarkerObjectNameInfo);
				}
			}

			void SetObjectTag(VkDevice deviceHandle, uint64_t object, VkDebugReportObjectTypeEXT objectType, uint64_t tagName, size_t tagSize, const void* pTag)
			{
				// Check for valid function pointer (may not be present if not running in a debugging application)
				if (pfnDebugMarkerSetObjectTag)
				{
					VkDebugMarkerObjectTagInfoEXT debugMarkerObjectTagInfo = VulkanInitializers::DebugMarkerObjectTagInfo(objectType, object, tagName, tagSize, pTag);
					pfnDebugMarkerSetObjectTag(deviceHandle, &debugMarkerObjectTagInfo);
				}
			}

			void BeginRegion(VkCommandBuffer cmdbufferHandle, const char_t* pMarkerName, bfloat32_t color[4])
			{
				// Check for valid function pointer (may not be present if not running in a debugging application)
				if (pfnCmdDebugMarkerBegin)
				{
					VkDebugMarkerMarkerInfoEXT debugMarkerMarkerInfo = VulkanInitializers::DebugMarkerMarkerInfo(pMarkerName, color);
					pfnCmdDebugMarkerBegin(cmdbufferHandle, &debugMarkerMarkerInfo);
				}
			}

			void Insert(VkCommandBuffer cmdbufferHandle, const char_t* pMarkerName, bfloat32_t color[4])
			{
				// Check for valid function pointer (may not be present if not running in a debugging application)
				if (pfnCmdDebugMarkerInsert)
				{
					VkDebugMarkerMarkerInfoEXT debugMarkerMarkerInfo = VulkanInitializers::DebugMarkerMarkerInfo(pMarkerName, color);
					pfnCmdDebugMarkerInsert(cmdbufferHandle, &debugMarkerMarkerInfo);
				}
			}

			void EndRegion(VkCommandBuffer cmdbufferHandle)
			{
				// Check for valid function (may not be present if not runnin in a debugging application)
				if (pfnCmdDebugMarkerEnd)
				{
					pfnCmdDebugMarkerEnd(cmdbufferHandle);
				}
			}

			void SetCommandBufferName(VkDevice deviceHandle, VkCommandBuffer cmdBuffer, const char_t* pName)
			{
				SetObjectName(deviceHandle, reinterpret_cast<uint64_t>(cmdBuffer), VkDebugReportObjectTypeEXT::VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT, pName);
			}

			void SetQueueName(VkDevice deviceHandle, VkQueue queueHandle, const char_t* pName)
			{
				SetObjectName(deviceHandle, reinterpret_cast<uint64_t>(queueHandle), VkDebugReportObjectTypeEXT::VK_DEBUG_REPORT_OBJECT_TYPE_QUEUE_EXT, pName);
			}

			void SetImageName(VkDevice deviceHandle, VkImage imageHandle, const char_t* pName)
			{
				SetObjectName(deviceHandle, reinterpret_cast<uint64_t>(imageHandle), VkDebugReportObjectTypeEXT::VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT, pName);
			}

			void SetSamplerName(VkDevice deviceHandle, VkSampler samplerHandle, const char_t* pName)
			{
				SetObjectName(deviceHandle, reinterpret_cast<uint64_t>(samplerHandle), VkDebugReportObjectTypeEXT::VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_EXT, pName);
			}

			void SetBufferName(VkDevice deviceHandle, VkBuffer bufferHandle, const char_t* pName)
			{
				SetObjectName(deviceHandle, reinterpret_cast<uint64_t>(bufferHandle), VkDebugReportObjectTypeEXT::VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT, pName);
			}

			void SetDeviceMemoryName(VkDevice deviceHandle, VkDeviceMemory memoryHandle, const char_t* pName)
			{
				SetObjectName(deviceHandle, reinterpret_cast<uint64_t>(memoryHandle), VkDebugReportObjectTypeEXT::VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_MEMORY_EXT, pName);
			}

			void SetShaderModuleName(VkDevice deviceHandle, VkShaderModule shaderModuleHandle, const char_t* pName)
			{
				SetObjectName(deviceHandle, reinterpret_cast<uint64_t>(shaderModuleHandle), VkDebugReportObjectTypeEXT::VK_DEBUG_REPORT_OBJECT_TYPE_SHADER_MODULE_EXT, pName);
			}

			void SetPipelineName(VkDevice deviceHandle, VkPipeline pipelineHandle, const char_t* pName)
			{
				SetObjectName(deviceHandle, reinterpret_cast<uint64_t>(pipelineHandle), VkDebugReportObjectTypeEXT::VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_EXT, pName);
			}

			void SetPipelineLayoutName(VkDevice deviceHandle, VkPipelineLayout pipelineLayout, const char_t* pName)
			{
				SetObjectName(deviceHandle, reinterpret_cast<uint64_t>(pipelineLayout), VkDebugReportObjectTypeEXT::VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_LAYOUT_EXT, pName);
			}

			void SetRenderPassName(VkDevice deviceHandle, VkRenderPass renderPassHandle, const char_t* pName)
			{
				SetObjectName(deviceHandle, reinterpret_cast<uint64_t>(renderPassHandle), VkDebugReportObjectTypeEXT::VK_DEBUG_REPORT_OBJECT_TYPE_RENDER_PASS_EXT, pName);
			}

			void SetFramebufferName(VkDevice deviceHandle, VkFramebuffer framebufferHandle, const char_t* pName)
			{
				SetObjectName(deviceHandle, reinterpret_cast<uint64_t>(framebufferHandle), VkDebugReportObjectTypeEXT::VK_DEBUG_REPORT_OBJECT_TYPE_FRAMEBUFFER_EXT, pName);
			}

			void SetDescriptorSetLayoutName(VkDevice deviceHandle, VkDescriptorSetLayout descriptorSetLayoutHandle, const char_t* pName)
			{
				SetObjectName(deviceHandle, reinterpret_cast<uint64_t>(descriptorSetLayoutHandle), VkDebugReportObjectTypeEXT::VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT_EXT, pName);
			}

			void SetDescriptorSetName(VkDevice deviceHandle, VkDescriptorSet descriptorSetHandle, const char_t* pName)
			{
				SetObjectName(deviceHandle, reinterpret_cast<uint64_t>(descriptorSetHandle), VkDebugReportObjectTypeEXT::VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_EXT, pName);
			}

			void SetSemaphoreName(VkDevice deviceHandle, VkSemaphore semaphoreHandle, const char_t* pName)
			{
				SetObjectName(deviceHandle, reinterpret_cast<uint64_t>(semaphoreHandle), VkDebugReportObjectTypeEXT::VK_DEBUG_REPORT_OBJECT_TYPE_SEMAPHORE_EXT, pName);
			}

			void SetFenceName(VkDevice deviceHandle, VkFence fenceHandle, const char_t* pName)
			{
				SetObjectName(deviceHandle, reinterpret_cast<uint64_t>(fenceHandle), VkDebugReportObjectTypeEXT::VK_DEBUG_REPORT_OBJECT_TYPE_FENCE_EXT, pName);
			}

			void SetEventName(VkDevice deviceHandle, VkEvent eventHandle, const char_t* pName)
			{
				SetObjectName(deviceHandle, reinterpret_cast<uint64_t>(eventHandle), VkDebugReportObjectTypeEXT::VK_DEBUG_REPORT_OBJECT_TYPE_EVENT_EXT, pName);
			}
		}
	}
}