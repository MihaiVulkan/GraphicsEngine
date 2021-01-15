#ifndef GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_PIPELINE_CACHE_HPP
#define GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_PIPELINE_CACHE_HPP

#include "Graphics/Rendering/Backends/Vulkan/Common/VulkanObject.hpp"
#include <vector>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class VulkanDevice;

		/*
			*Wrapper for VkPipelineCache object*

			Pipeline cache objects allow the result of pipeline construction to be reused between pipelines and between
			runs of an application. Reuse between pipelines is achieved by passing the same pipeline cache object when 
			creating multiple related pipelines. Reuse across runs of an application is achieved by retrieving pipeline 
			cache contents in one run of an application, saving the contents, and using them to preinitialize a pipeline 
			cache on a subsequent run. The contents of the pipeline cache objects are managed by the implementation. 
			Applications can manage the host memory consumed by a pipeline cache object and control the amount of data 
			retrieved from a pipeline cache object.

		*/
		class VulkanPipelineCache : public VulkanObject
		{
			GE_RTTI(GraphicsEngine::Graphics::VulkanPipelineCache)

		public:
			VulkanPipelineCache();
			explicit VulkanPipelineCache(VulkanDevice* pDevice, std::vector<void*> initialData, VkPipelineCacheCreateFlags flags = 0);
			virtual ~VulkanPipelineCache();

			VkResult GetData(void* pData, size_t& pSize);
			VkResult Merge(const std::vector<VulkanPipelineCache*>& caches);

			const VkPipelineCache& GetHandle() const;

		private:
			void Create(const VkPipelineCacheCreateInfo& pipelineCacheCreateInfo);
			void Destroy();

			VulkanDevice* mpDevice;

			VkPipelineCache mHandle;
		};
	}
}

#endif // GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_PIPELINE_CACHE_HPP