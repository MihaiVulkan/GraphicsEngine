#ifndef GRAPHICS_RENDERING_VULKAN_SHADERMODULE_HPP
#define GRAPHICS_RENDERING_VULKAN_SHADERMODULE_HPP

#include "Foundation/TypeDefs.hpp"
#include "vulkan/vulkan.h"
#include <vector>

#define USE_GLSLANG

#ifdef USE_GLSLANG
#include "glslang/SPIRV/GlslangToSpv.h"
#endif


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
		class VulkanShaderModule
		{
		public:
			VulkanShaderModule();
			explicit VulkanShaderModule(VulkanDevice* pDevice, VkShaderStageFlagBits shaderType, const char_t* pPath);
			virtual ~VulkanShaderModule();

			const VkShaderModule& GetHandle() const;
			VkShaderStageFlagBits GetShaderType() const;

		private:
			void Create(const char_t* pPath);
			void Destroy();

#ifdef USE_GLSLANG
			bool_t GLSLtoSPV(VkShaderStageFlagBits shaderType, const char_t* pShader, std::vector<uint32_t>& spirvVec);
			void InitResources(TBuiltInResource& Resources);
			EShLanguage FindLanguage(const VkShaderStageFlagBits shaderType);
#endif 
			VulkanDevice* mpDevice;

			VkShaderModule mHandle;
			VkShaderStageFlagBits mShaderType;
		};
	}
}

#endif // GRAPHICS_RENDERING_VULKAN_SHADERMODULE_HPP