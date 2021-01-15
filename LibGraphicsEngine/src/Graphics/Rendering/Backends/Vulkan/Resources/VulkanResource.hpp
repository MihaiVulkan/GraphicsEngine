#ifndef GRAPHICS_RENDERING_BACKENDS_VULKAN_RESOURCES_VULKAN_RESOURCE_HPP
#define GRAPHICS_RENDERING_BACKENDS_VULKAN_RESOURCES_VULKAN_RESOURCE_HPP

#include "Graphics/Rendering/Backends/Vulkan/Common/VulkanObject.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		// Vulkan implementation of the GADR Resource base class
		class GADRResource : public VulkanObject
		{
			GE_RTTI(GraphicsEngine::Graphics::GADRResource)

		public:
			GADRResource();
			virtual ~GADRResource();

			virtual void OnBind(uint32_t currentBufferIdx = 0) {};
			virtual void OnUnBind(uint32_t currentBufferIdx = 0) {};
		};
	}

}

#endif // GRAPHICS_RENDERING_BACKENDS_VULKAN_RESOURCES_VULKAN_RESOURCE_HPP