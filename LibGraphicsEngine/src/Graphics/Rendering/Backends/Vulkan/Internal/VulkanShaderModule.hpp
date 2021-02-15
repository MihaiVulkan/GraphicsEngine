#ifndef GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_SHADER_MODULE_HPP
#define GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_SHADER_MODULE_HPP

#include "Core/AppConfig.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Common/VulkanObject.hpp"
#include <vector>
#include <string>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class VulkanDevice;

		/*
			*Wrapper for VkShaderModule object*

			Shader modules contain shader code and one or more entry points. Shaders are selected from a shader module by specifying 
			an entry point as part of pipeline creation. The stages of a pipeline can use shaders that come from different modules. 
			The shader code defining a shader module must be in the SPIR-V format, as described by the Vulkan Environment for SPIR-V appendix.
		*/
		class VulkanShaderModule : public VulkanObject
		{
			GE_RTTI(GraphicsEngine::Graphics::VulkanShaderModule)

		public:
			VulkanShaderModule();
			explicit VulkanShaderModule(VulkanDevice* pDevice, VkShaderStageFlagBits shaderStage, const std::string& sourcePath);
			virtual ~VulkanShaderModule();

			const VkShaderModule& GetHandle() const;
			VkShaderStageFlagBits GetShaderStage() const;

		private:
			void Create(const std::string& sourcePath);
			void Destroy();

			VulkanDevice* mpDevice;

			VkShaderModule mHandle;
			VkShaderStageFlagBits mShaderStage;
		};
	}
}

#endif // GRAPHICS_RENDERING_BACKENDS_VULKAN_INTERNAL_VULKAN_SHADER_MODULE_HPP