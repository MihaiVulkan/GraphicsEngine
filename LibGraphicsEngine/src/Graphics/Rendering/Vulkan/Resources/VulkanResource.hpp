#ifndef GRAPHICS_RENDERING_VULKAN_RESOURCES_RESOURCE_HPP
#define GRAPHICS_RENDERING_VULKAN_RESOURCES_RESOURCE_HPP

#include "Foundation/TypeDefs.hpp"
#include "Foundation/RTTI.hpp"
#include "vulkan/vulkan.h"

namespace GraphicsEngine
{
	namespace Graphics
	{
		// Vulkan implementation of the GADR Resource base class
		class GADRResource : public RTTI
		{
			GE_RTTI(GraphicsEngine::Graphics::GADRResource)

		public:
			GADRResource();
			virtual ~GADRResource();
		};
	}

}

#endif // GRAPHICS_RENDERING_VULKAN_RESOURCES_RESOURCE_HPP