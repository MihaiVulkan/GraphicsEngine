#ifndef GRAPHICS_RENDERING_BACKENDS_VULKAN_COMMON_VULKAN_OBJECT_HPP
#define GRAPHICS_RENDERING_BACKENDS_VULKAN_COMMON_VULKAN_OBJECT_HPP

#include "Foundation/Object.hpp"
#include "vulkan/vulkan.h"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class VulkanObject : public Object
		{
		public:
			VulkanObject();
			virtual ~VulkanObject();
		};

	}
}

#endif // GRAPHICS_RENDERING_BACKENDS_VULKAN_VULKAN_OBJECT_HPP