#ifndef GRAPHICS_RENDERING_VULKAN_QUERY_POOL_HPP
#define GRAPHICS_RENDERING_VULKAN_QUERY_POOL_HPP

#include "Foundation/TypeDefs.hpp"
#include "vulkan/vulkan.h"
#include <vector>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class VulkanDevice;

		/*
			*Wrapper for VkQueryPool object*

			Queries provide a mechanism to return information about the processing of a sequence of Vulkan commands. 
			Query operations are asynchronous, and as such, their results are not returned immediately. Instead, 
			their results, and their availability status are stored in a Query Pool. The state of these queries can 
			be read back on the host, or copied to a buffer object on the device.

			The supported query types are Occlusion Queries, Pipeline Statistics Queries, and Timestamp Queries. Performance Queries
			are also supported if the associated extension is available. Intel performance queries are also supported if the 
			associated extension is available.

			Queries are managed using query pool objects. Each query pool is a collection of a specific number of queries of a particular type.

		*/

		class VulkanQueryPool
		{
		public:
			VulkanQueryPool();
			explicit VulkanQueryPool(VulkanDevice* pDevice, VkQueryType queryType, uint32_t queryCount, VkQueryPipelineStatisticFlags pipelineStatistics, 
									VkQueryPoolCreateFlags flags = 0);
			virtual ~VulkanQueryPool();

			void BeginQuery(VkCommandBuffer commandBufferHandle);
			void EndQuery(VkCommandBuffer commandBufferHandle);
			void ResetQuery(VkCommandBuffer commandBufferHandle, uint32_t queryCount);

			void GetQueryResults(uint32_t queryCount, void* pData, size_t dataSize, VkDeviceSize stride, VkQueryResultFlags flags);

			const VkQueryPool& GetHandle() const;

		private:
			void Create(const VkQueryPoolCreateInfo& queryPoolInfo);
			void Destroy();

			VulkanDevice* mpDevice;

			VkQueryPool mHandle;
		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_QUERY_POOL_HPP