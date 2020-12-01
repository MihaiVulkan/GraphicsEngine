#include "VulkanUtils.hpp"
#include "Foundation/Logger.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		namespace VulkanUtils
		{
			VkPrimitiveTopology PrimitiveTopologyToVulkanTopolgy(GeometricPrimitive::PrimitiveTopology topology)
			{
				VkPrimitiveTopology vulkanTopology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;

				switch (topology)
				{
				case GeometricPrimitive::PrimitiveTopology::PT_POINT_LIST:
					vulkanTopology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
					break;
				case GeometricPrimitive::PrimitiveTopology::PT_LINE_LIST:
					vulkanTopology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
					break;
				case GeometricPrimitive::PrimitiveTopology::PT_LINE_LOOP:
					LOG_ERROR("Line loop topology is not supported by Vulkan API!");
					break;
				case GeometricPrimitive::PrimitiveTopology::PT_LINE_STRIP:
					vulkanTopology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
					break;
				case GeometricPrimitive::PrimitiveTopology::PT_TRIANGLE_LIST:
					vulkanTopology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
					break;
				case GeometricPrimitive::PrimitiveTopology::PT_TRIANGLE_STRIP:
					vulkanTopology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
					break;
				case GeometricPrimitive::PrimitiveTopology::PT_TRIANGLE_FAN:
					vulkanTopology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
					break;
				case GeometricPrimitive::PrimitiveTopology::PT_PATCH_LIST:
					vulkanTopology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
					break;

				default:
					;
				}

				return vulkanTopology;
			}
		}
	}
}