#ifndef GRAPHICS_RENDERING_VULKAN_RESOURCES_VULKAN_MATERIAL_HPP
#define GRAPHICS_RENDERING_VULKAN_RESOURCES_VULKAN_MATERIAL_HPP

#include "VulkanResource.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Renderer;
		class VulkanRenderer;

		class Material;

		// Vulkan implementation of the Graphics API Dependent Resource
		// INFO : basic default material
		class GADRMaterial : public GADRResource
		{
			GE_RTTI(GraphicsEngine::Graphics::GADRMaterial)

		public:
			GADRMaterial();
			explicit GADRMaterial(Renderer* pRenderer, Material* pMaterial);
			virtual ~GADRMaterial();

			void Bind(uint32_t currentBufferIdx);


		private:
			void Create(Renderer* pRenderer);
			void Destroy();

			VulkanRenderer* mpRenderer;

			Material* mpMaterial;
		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_RESOURCES_VULKAN_MATERIAL_HPP