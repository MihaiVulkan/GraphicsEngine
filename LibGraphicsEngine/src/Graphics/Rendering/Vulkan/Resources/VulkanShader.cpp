#include "VulkanShader.hpp"
#include "Graphics/Rendering/Vulkan/VulkanRenderer.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanShaderModule.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanInitializers.hpp"
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

	VkShaderStageFlagBits vulkanType = TypeToVulkanType(mpShader->GetType());

	// pRenderer must be a pointer to VulkanRenderer otherwise the cast will fail!
	VulkanRenderer* pVulkanRenderer = dynamic_cast<VulkanRenderer*>(pRenderer);
	assert(pVulkanRenderer != nullptr);

	VulkanDevice* pDevice = pVulkanRenderer->GetDevice();
	assert(pDevice != nullptr);

	// setup Shader Module
	mpVulkanShaderModule = GE_ALLOC(VulkanShaderModule)(pDevice, vulkanType, mpShader->GetPath().c_str());
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

VkShaderStageFlagBits GADRShader::TypeToVulkanType(const Shader::Type& type) const
{
	VkShaderStageFlagBits vulkanType = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;

	switch (type)
	{
	case Shader::Type::T_VERTEX:
		vulkanType = VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
		break;
	case Shader::Type::T_TESSELATION_CONTROL:
		vulkanType = VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		break;
	case Shader::Type::T_TESSELATION_EVALUATION:
		vulkanType = VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		break;
	case Shader::Type::T_GEOMETRY:
		vulkanType = VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT;
		break;
	case Shader::Type::T_FRAGMENT:
		vulkanType = VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
		break;
	case Shader::Type::T_ALL_GRAPHICS:
		vulkanType = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL_GRAPHICS;
		break;
	case Shader::Type::T_COMPUTE:
		vulkanType = VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;
		break;
	case Shader::Type::T_COUNT:
	default:
		LOG_ERROR("Invalid Vulkan Shader Type!");
	}

	return vulkanType;
}

VulkanShaderModule* GADRShader::GetVkShaderModule() const
{
	return mpVulkanShaderModule;
}