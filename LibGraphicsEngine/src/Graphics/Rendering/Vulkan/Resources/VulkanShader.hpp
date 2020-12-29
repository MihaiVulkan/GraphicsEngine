#ifndef GRAPHICS_RENDERING_VULKAN_RESOURCES_VULKAN_SHADER_HPP
#define GRAPHICS_RENDERING_VULKAN_RESOURCES_VULKAN_SHADER_HPP

#include "VulkanResource.hpp"
#include "Graphics/Rendering/Resources/Shader.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Renderer;
		class VulkanShaderModule;

		// Vulkan implementation of the Graphics API Dependent Resource
		class GADRShader : public GADRResource
		{
			GE_RTTI(GraphicsEngine::Graphics::GADRShader)

		public:
			GADRShader();
			explicit GADRShader(Renderer* pRenderer, Shader* pShader);
			virtual ~GADRShader();

			Shader* GetShader() const;

			//
			VulkanShaderModule* GetVkShaderModule() const;

		private:
			void Create(Renderer* pRenderer);
			void Destroy();

			VulkanShaderModule* mpVulkanShaderModule;

			Shader* mpShader;
		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_RESOURCES_VULKAN_SHADER_HPP