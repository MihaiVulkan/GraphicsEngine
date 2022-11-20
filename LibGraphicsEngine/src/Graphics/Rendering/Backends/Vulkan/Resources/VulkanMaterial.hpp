#ifndef GRAPHICS_RENDERING_BACKENDS_VULKAN_RESOURCES_VULKAN_MATERIAL_HPP
#define GRAPHICS_RENDERING_BACKENDS_VULKAN_RESOURCES_VULKAN_MATERIAL_HPP

#if defined(VULKAN_RENDERER)
#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanResource.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Renderer;
		class Material;
		class VulkanRenderer;

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
#endif // VULKAN_RENDERER

#endif // GRAPHICS_RENDERING_BACKENDS_VULKAN_RESOURCES_VULKAN_MATERIAL_HPP