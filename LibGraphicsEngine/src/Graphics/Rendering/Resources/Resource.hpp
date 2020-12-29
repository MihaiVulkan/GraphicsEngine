#ifndef GRAPHICS_RENDERING_RESOURCES_RESOURCE_HPP
#define GRAPHICS_RENDERING_RESOURCES_RESOURCE_HPP

#include "Foundation/Object.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Resource: public Object
		{
			GE_RTTI(GraphicsEngine::Graphics::Resource)

		public:
			Resource();
			virtual ~Resource();

			virtual void OnBind() {};
			virtual void OnUnBind() {};
		};
	}
}


#endif // GRAPHICS_RENDERING_RESOURCES_RESOURCE_HPP