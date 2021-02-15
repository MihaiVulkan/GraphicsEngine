#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanShaderModule.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanDevice.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanInitializers.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanHelpers.hpp"
#include "Foundation/FileUtils.hpp"
#include "Foundation/Logger.hpp"
#include <string>
#include <cassert>


using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

#define SPIRV_EXT ".spv"

VulkanShaderModule::VulkanShaderModule()
	: mpDevice(nullptr)
	, mHandle(VK_NULL_HANDLE)
	, mShaderStage(VkShaderStageFlagBits::VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM)
{}

VulkanShaderModule::VulkanShaderModule(VulkanDevice* pDevice, VkShaderStageFlagBits shaderStage, const std::string& sourcePath)
	: mpDevice(pDevice)
	, mHandle(VK_NULL_HANDLE)
	, mShaderStage(shaderStage)
{
	Create(sourcePath);
}

VulkanShaderModule::~VulkanShaderModule()
{
	Destroy();
}

void VulkanShaderModule::Create(const std::string& sourcePath)
{
	assert(mpDevice != nullptr);
	assert(sourcePath.empty() == false);

	std::string sourcePathTmp(sourcePath);

	//check if file path is a SPIR-V binary file
	if (sourcePathTmp.find(SPIRV_EXT) == std::string::npos) //if not found
	{
		//add it
		sourcePathTmp += SPIRV_EXT;
	}

	std::vector<char_t> shaderBinary;
	FileUtils::ReadBinaryFile(sourcePathTmp, shaderBinary);
	assert(shaderBinary.empty() == false);

	VkShaderModuleCreateInfo shaderModuleCreateInfo = VulkanInitializers::ShaderModuleCreateInfo(shaderBinary.size(), (uint32_t*)shaderBinary.data());

	VK_CHECK_RESULT(vkCreateShaderModule(mpDevice->GetDeviceHandle(), &shaderModuleCreateInfo, nullptr, &mHandle));

}

void VulkanShaderModule::Destroy()
{
	assert(mpDevice != nullptr);

	mShaderStage = VkShaderStageFlagBits::VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;

	if (mHandle)
	{
		vkDestroyShaderModule(mpDevice->GetDeviceHandle(), mHandle, nullptr);
		mHandle = VK_NULL_HANDLE;
	}

	if (mpDevice)
	{
		mpDevice = nullptr;
	}
}

const VkShaderModule& VulkanShaderModule::GetHandle() const
{
	return mHandle;
}

VkShaderStageFlagBits VulkanShaderModule::GetShaderStage() const
{
	return mShaderStage;
}