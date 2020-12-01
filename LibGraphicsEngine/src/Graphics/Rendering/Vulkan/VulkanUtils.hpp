#ifndef GRAPHICS_RENDERING_VULKAN_UTILS_HPP
#define GRAPHICS_RENDERING_VULKAN_UTILS_HPP

#include "Foundation/TypeDefs.hpp"
#include "vulkan/vulkan.h"

#include "Graphics/GeometricPrimitives/GeometricPrimitive.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		namespace VulkanUtils
		{
			VkPrimitiveTopology PrimitiveTopologyToVulkanTopolgy(GeometricPrimitive::PrimitiveTopology topology);
		}
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_UTILS_HPP