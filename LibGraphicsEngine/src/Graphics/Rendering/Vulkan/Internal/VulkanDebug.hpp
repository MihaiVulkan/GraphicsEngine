#ifndef GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_DEBUG_HPP
#define GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_DEBUG_HPP

#include "Foundation/TypeDefs.hpp"
#include "vulkan/vulkan.h"

namespace GraphicsEngine
{
	namespace Graphics
	{
		/*
			Debug wrapper when working with Vulkan Validation layers
		*/
		namespace VulkanDebug
		{
			// Default validation layers
			//extern int validationLayerCount;
			//extern const char* validationLayerNames[];

			// Default debug callback
			VKAPI_ATTR VkBool32 VKAPI_CALL MessageCallback(
				VkDebugReportFlagsEXT flags,
				VkDebugReportObjectTypeEXT objType,
				uint64_t srcObject,
				size_t location,
				int32_t msgCode,
				const char_t* pLayerPrefix,
				const char_t* pMsg,
				void* pUserData);

			// Load debug function pointers and set debug callback
			// if callBack is NULL, default message callback will be used
			void SetupDebugging(
				VkInstance instanceHandle,
				VkDebugReportFlagsEXT flags,
				VkDebugReportCallbackEXT callBack);
			// Clear debug callback
			void FreeDebugCallback(VkInstance instanceHandle);
		}

		// Setup and functions for the VK_EXT_debug_marker_extension
		// Extension spec can be found at https://github.com/KhronosGroup/Vulkan-Docs/blob/1.0-VK_EXT_debug_marker/doc/specs/vulkan/appendices/VK_EXT_debug_marker.txt
		// Note that the extension will only be present if run from an offline debugging application
		// The actual check for extension presence and enabling it on the device is done in the example base class
		// See VulkanExampleBase::createInstance and VulkanExampleBase::createDevice (base/vulkanexamplebase.cpp)
		namespace VulkanDebugMarker
		{
			// Set to true if function pointer for the debug marker are available
			extern bool_t active;

			// Get function pointers for the debug report extensions from the device
			void Setup(VkDevice deviceHandle);

			// Sets the debug name of an object
			// All Objects in Vulkan are represented by their 64-bit handles which are passed into this function
			// along with the object type
			void SetObjectName(VkDevice deviceHandle, uint64_t object, VkDebugReportObjectTypeEXT objectType, const char_t* pObjectName);

			// Set the tag for an object
			void SetObjectTag(VkDevice deviceHandle, uint64_t object, VkDebugReportObjectTypeEXT objectType, uint64_t tagName, size_t tagSize, const void* pTag);

			// Start a new debug marker region
			void BeginRegion(VkCommandBuffer cmdbufferHandle, const char_t* pMarkerName, float32_t color[4]);

			// Insert a new debug marker into the command buffer
			void Insert(VkCommandBuffer cmdbufferHandle, const char_t* pMarkerName, float32_t color[4]);

			// End the current debug marker region
			void EndRegion(VkCommandBuffer cmdBufferHandle);

			// Object specific naming functions
			void SetCommandBufferName(VkDevice deviceHandle, VkCommandBuffer cmdBufferHandle, const char_t* pName);
			void SetQueueName(VkDevice deviceHandle, VkQueue queueHandle, const char_t* pName);
			void SetImageName(VkDevice deviceHandle, VkImage imageHandle, const char_t* pName);
			void SetSamplerName(VkDevice deviceHandle, VkSampler samplerHandle, const char_t* pName);
			void SetBufferName(VkDevice deviceHandle, VkBuffer bufferHandle, const char_t* pName);
			void SetDeviceMemoryName(VkDevice deviceHandle, VkDeviceMemory memoryHandle, const char_t* pName);
			void SetShaderModuleName(VkDevice deviceHandle, VkShaderModule shaderModuleHandle, const char_t* pName);
			void SetPipelineName(VkDevice deviceHandle, VkPipeline pipelineHandle, const char_t* pName);
			void SetPipelineLayoutName(VkDevice deviceHandle, VkPipelineLayout pipelineLayoutHandle, const char_t* pName);
			void SetRenderPassName(VkDevice deviceHandle, VkRenderPass renderPassHandle, const char_t* pName);
			void SetFramebufferName(VkDevice deviceHandle, VkFramebuffer framebufferHandle, const char_t* pName);
			void SetDescriptorSetLayoutName(VkDevice deviceHandle, VkDescriptorSetLayout descriptorSetLayoutHandle, const char_t* pName);
			void SetDescriptorSetName(VkDevice deviceHandle, VkDescriptorSet descriptorSetHandle, const char_t* pName);
			void SetSemaphoreName(VkDevice deviceHandle, VkSemaphore semaphoreHandle, const char_t* pName);
			void SetFenceName(VkDevice deviceHandle, VkFence fenceHandle, const char_t* pName);
			void SetEventName(VkDevice deviceHandle, VkEvent eventHandle, const char_t* pName);
		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_INTERNAL_VULKAN_DEBUG_HPP