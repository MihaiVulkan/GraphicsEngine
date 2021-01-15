#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanShader.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Common/VulkanUtils.hpp"
#include "Graphics/Rendering/Backends/Vulkan/VulkanRenderer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanShaderModule.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanInitializers.hpp"
#include "Graphics/ShaderTools/GLSL/GLSLShaderParser.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

GADRShader::GADRShader()
	: mpVulkanShaderModule(nullptr)
	, mpShader(nullptr)
{}

GADRShader::GADRShader(Renderer* pRenderer, Shader* pShader)
	: mpVulkanShaderModule(nullptr)
	, mpShader(pShader)
{
	Create(pRenderer);
}

GADRShader::~GADRShader()
{
	Destroy();
}

void GADRShader::Create(Renderer* pRenderer)
{
	assert(pRenderer != nullptr);
	assert(mpShader != nullptr);

	VkShaderStageFlagBits vulkanType = VulkanUtils::ShaderStageToVulkanShaderStage(mpShader->GetShaderStage());

	// pRenderer must be a pointer to VulkanRenderer otherwise the cast will fail!
	VulkanRenderer* pVulkanRenderer = dynamic_cast<VulkanRenderer*>(pRenderer);
	assert(pVulkanRenderer != nullptr);

	VulkanDevice* pDevice = pVulkanRenderer->GetDevice();
	assert(pDevice != nullptr);

	// setup Shader Module
	mpVulkanShaderModule = GE_ALLOC(VulkanShaderModule)
	(
		pDevice,
		vulkanType, mpShader->GetSourcePath().c_str()
	);
	assert(mpVulkanShaderModule != nullptr);
}

void GADRShader::Destroy()
{
	if (mpShader)
	{
		mpShader = nullptr;
	}

	GE_FREE(mpVulkanShaderModule);
}

Shader* GADRShader::GetShader() const
{
	return mpShader;
}

VulkanShaderModule* GADRShader::GetVkShaderModule() const
{
	return mpVulkanShaderModule;
}