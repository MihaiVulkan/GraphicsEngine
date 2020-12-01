#ifndef GRAPHICS_RENDERING_RESOURCES_RESOURCE_HPP
#define GRAPHICS_RENDERING_RESOURCES_RESOURCE_HPP

#include "Foundation/TypeDefs.hpp"
#include "Foundation/RTTI.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Resource: public RTTI
		{
			GE_RTTI(GraphicsEngine::Graphics::Resource)

		public:
			Resource();
			virtual ~Resource();
		};
	}
}


#endif // GRAPHICS_RENDERING_RESOURCES_RESOURCE_HPP