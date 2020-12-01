#ifndef GRAPHICS_RENDERING_VULKAN_RESOURCES_MATERIAL_HPP
#define GRAPHICS_RENDERING_VULKAN_RESOURCES_MATERIAL_HPP

#include "VulkanResource.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Renderer;
		class VulkanRenderer;

		class VulkanDescriptorPool;
		class VulkanDescriptorSetLayout;
		class VulkanDescriptorSet;

		class VulkanPipelineLayout;
		class VulkanGraphicsPipeline;
		
		class Material;
		//TODO workaround to have geometric primitive info
		class GeometricPrimitive;

		// Vulkan implementation of the Graphics API Dependent Resource
		// INFO : basic default material
		class GADRMaterial : public GADRResource
		{
			GE_RTTI(GraphicsEngine::Graphics::GADRMaterial)

		public:
			GADRMaterial();
			explicit GADRMaterial(Renderer* pRenderer, Material* pMaterial, GeometricPrimitive* pGeoPrimitive);
			virtual ~GADRMaterial();

			void Bind(uint32_t currentBufferIdx);


		private:
			void Create(Renderer* pRenderer, GeometricPrimitive* pGeoPrimitive);
			void Destroy();

			VulkanRenderer* mpRenderer;
			
			VulkanDescriptorPool* mpVulkanDescriptorPool;
			VulkanDescriptorSetLayout* mpVulkanDescriptorSetLayout;
			VulkanDescriptorSet* mpVulkanDescriptorSet;

			VulkanPipelineLayout* mpVulkanPipelineLayout;
			VulkanGraphicsPipeline* mpVulkanGraphicsPipeline;

			Material* mpMaterial;
		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_RESOURCES_MATERIAL_HPP