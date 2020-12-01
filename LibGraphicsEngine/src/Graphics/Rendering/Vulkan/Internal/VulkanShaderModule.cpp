#include "VulkanShaderModule.hpp"
#include "VulkanDevice.hpp"
#include "VulkanInitializers.hpp"
#include "VulkanHelpers.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/FileUtils.hpp"
#include <cassert>


using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanShaderModule::VulkanShaderModule()
	: mpDevice(nullptr)
	, mHandle(VK_NULL_HANDLE)
	, mShaderType(VkShaderStageFlagBits::VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM)
{}

VulkanShaderModule::VulkanShaderModule(VulkanDevice* pDevice, VkShaderStageFlagBits shaderType, const char_t* pPath)
	: mpDevice(pDevice)
	, mHandle(VK_NULL_HANDLE)
	, mShaderType(shaderType)
{
	Create(pPath);
}

VulkanShaderModule::~VulkanShaderModule()
{
	Destroy();
}

void VulkanShaderModule::Create(const char_t* pPath)
{
	assert(mpDevice != nullptr);
	assert(pPath != nullptr);

#ifdef USE_GLSLANG
	glslang::InitializeProcess();

	{
		uint32_t size = 0;
		char_t* pShaderCode = FileUtils::ReadFile(pPath, size);
		assert(pShaderCode != nullptr);

		std::vector<uint32_t> shaderSPV;
		assert(GLSLtoSPV(mShaderType, pShaderCode, shaderSPV) == true);

		VkShaderModuleCreateInfo shaderModuleCreateInfo = VulkanInitializers::ShaderModuleCreateInfo(shaderSPV.size() * sizeof(uint32_t), shaderSPV.data());

		VK_CHECK_RESULT(vkCreateShaderModule(mpDevice->GetDeviceHandle(), &shaderModuleCreateInfo, nullptr, &mHandle));

		GE_FREE_ARRAY(pShaderCode);
	}

	glslang::FinalizeProcess();
#else
	{
		uint32_t size = 0;
		char_t* pShaderCode = FileUtils::ReadFile(pPath, size);
		assert(pShaderCode != nullptr);

		VkShaderModuleCreateInfo shaderModuleCreateInfo = VulkanInitializers::ShaderModuleCreateInfo(size * sizeof(uint32_t), (uint32_t*)pShaderCode);

		VK_CHECK_RESULT(vkCreateShaderModule(mpDevice->GetDeviceHandle(), &shaderModuleCreateInfo, nullptr, &mHandle));

		GE_FREE_ARRAY(pShaderCode);
	}
#endif // USE_GLSLANG
}

void VulkanShaderModule::Destroy()
{
	mShaderType = {};

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

VkShaderStageFlagBits VulkanShaderModule::GetShaderType() const
{
	return mShaderType;
}

#ifdef USE_GLSLANG
//
// Compile a given string containing GLSL into SPV for use by VK
// Return value of false means an error was encountered.
//
bool VulkanShaderModule::GLSLtoSPV(VkShaderStageFlagBits shaderType, const char_t* pShader, std::vector<uint32_t>& spirvVec)
{
	EShLanguage stage = FindLanguage(shaderType);
	glslang::TShader shader(stage);
	glslang::TProgram program;
	const char_t* shaderStrings[1];

	TBuiltInResource Resources = {};
	InitResources(Resources);

	// Enable SPIR-V and Vulkan rules when parsing GLSL
	EShMessages messages = (EShMessages)(EShMessages::EShMsgSpvRules | EShMessages::EShMsgVulkanRules);

	shaderStrings[0] = pShader;
	shader.setStrings(shaderStrings, 1);

	if (false == shader.parse(&Resources, 100, false, messages))
	{
		puts(shader.getInfoLog());
		puts(shader.getInfoDebugLog());
		return false;  // something didn't work
	}

	program.addShader(&shader);

	//
	// Program-level processing...
	//

	if (false == program.link(messages))
	{
		puts(shader.getInfoLog());
		puts(shader.getInfoDebugLog());
		fflush(stdout);
		return false;
	}

	glslang::TIntermediate* pProg = program.getIntermediate(stage);
	assert(nullptr != pProg);

	glslang::GlslangToSpv(*pProg, spirvVec);

	return true;
}

void VulkanShaderModule::InitResources(TBuiltInResource& Resources)
{
	Resources.maxLights = 32;
	Resources.maxClipPlanes = 6;
	Resources.maxTextureUnits = 32;
	Resources.maxTextureCoords = 32;
	Resources.maxVertexAttribs = 64;
	Resources.maxVertexUniformComponents = 4096;
	Resources.maxVaryingFloats = 64;
	Resources.maxVertexTextureImageUnits = 32;
	Resources.maxCombinedTextureImageUnits = 80;
	Resources.maxTextureImageUnits = 32;
	Resources.maxFragmentUniformComponents = 4096;
	Resources.maxDrawBuffers = 32;
	Resources.maxVertexUniformVectors = 128;
	Resources.maxVaryingVectors = 8;
	Resources.maxFragmentUniformVectors = 16;
	Resources.maxVertexOutputVectors = 16;
	Resources.maxFragmentInputVectors = 15;
	Resources.minProgramTexelOffset = -8;
	Resources.maxProgramTexelOffset = 7;
	Resources.maxClipDistances = 8;
	Resources.maxComputeWorkGroupCountX = 65535;
	Resources.maxComputeWorkGroupCountY = 65535;
	Resources.maxComputeWorkGroupCountZ = 65535;
	Resources.maxComputeWorkGroupSizeX = 1024;
	Resources.maxComputeWorkGroupSizeY = 1024;
	Resources.maxComputeWorkGroupSizeZ = 64;
	Resources.maxComputeUniformComponents = 1024;
	Resources.maxComputeTextureImageUnits = 16;
	Resources.maxComputeImageUniforms = 8;
	Resources.maxComputeAtomicCounters = 8;
	Resources.maxComputeAtomicCounterBuffers = 1;
	Resources.maxVaryingComponents = 60;
	Resources.maxVertexOutputComponents = 64;
	Resources.maxGeometryInputComponents = 64;
	Resources.maxGeometryOutputComponents = 128;
	Resources.maxFragmentInputComponents = 128;
	Resources.maxImageUnits = 8;
	Resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
	Resources.maxCombinedShaderOutputResources = 8;
	Resources.maxImageSamples = 0;
	Resources.maxVertexImageUniforms = 0;
	Resources.maxTessControlImageUniforms = 0;
	Resources.maxTessEvaluationImageUniforms = 0;
	Resources.maxGeometryImageUniforms = 0;
	Resources.maxFragmentImageUniforms = 8;
	Resources.maxCombinedImageUniforms = 8;
	Resources.maxGeometryTextureImageUnits = 16;
	Resources.maxGeometryOutputVertices = 256;
	Resources.maxGeometryTotalOutputComponents = 1024;
	Resources.maxGeometryUniformComponents = 1024;
	Resources.maxGeometryVaryingComponents = 64;
	Resources.maxTessControlInputComponents = 128;
	Resources.maxTessControlOutputComponents = 128;
	Resources.maxTessControlTextureImageUnits = 16;
	Resources.maxTessControlUniformComponents = 1024;
	Resources.maxTessControlTotalOutputComponents = 4096;
	Resources.maxTessEvaluationInputComponents = 128;
	Resources.maxTessEvaluationOutputComponents = 128;
	Resources.maxTessEvaluationTextureImageUnits = 16;
	Resources.maxTessEvaluationUniformComponents = 1024;
	Resources.maxTessPatchComponents = 120;
	Resources.maxPatchVertices = 32;
	Resources.maxTessGenLevel = 64;
	Resources.maxViewports = 16;
	Resources.maxVertexAtomicCounters = 0;
	Resources.maxTessControlAtomicCounters = 0;
	Resources.maxTessEvaluationAtomicCounters = 0;
	Resources.maxGeometryAtomicCounters = 0;
	Resources.maxFragmentAtomicCounters = 8;
	Resources.maxCombinedAtomicCounters = 8;
	Resources.maxAtomicCounterBindings = 1;
	Resources.maxVertexAtomicCounterBuffers = 0;
	Resources.maxTessControlAtomicCounterBuffers = 0;
	Resources.maxTessEvaluationAtomicCounterBuffers = 0;
	Resources.maxGeometryAtomicCounterBuffers = 0;
	Resources.maxFragmentAtomicCounterBuffers = 1;
	Resources.maxCombinedAtomicCounterBuffers = 1;
	Resources.maxAtomicCounterBufferSize = 16384;
	Resources.maxTransformFeedbackBuffers = 4;
	Resources.maxTransformFeedbackInterleavedComponents = 64;
	Resources.maxCullDistances = 8;
	Resources.maxCombinedClipAndCullDistances = 8;
	Resources.maxSamples = 4;
	Resources.maxMeshOutputVerticesNV = 256;
	Resources.maxMeshOutputPrimitivesNV = 512;
	Resources.maxMeshWorkGroupSizeX_NV = 32;
	Resources.maxMeshWorkGroupSizeY_NV = 1;
	Resources.maxMeshWorkGroupSizeZ_NV = 1;
	Resources.maxTaskWorkGroupSizeX_NV = 32;
	Resources.maxTaskWorkGroupSizeY_NV = 1;
	Resources.maxTaskWorkGroupSizeZ_NV = 1;
	Resources.maxMeshViewCountNV = 4;
	Resources.limits.nonInductiveForLoops = 1;
	Resources.limits.whileLoops = 1;
	Resources.limits.doWhileLoops = 1;
	Resources.limits.generalUniformIndexing = 1;
	Resources.limits.generalAttributeMatrixVectorIndexing = 1;
	Resources.limits.generalVaryingIndexing = 1;
	Resources.limits.generalSamplerIndexing = 1;
	Resources.limits.generalVariableIndexing = 1;
	Resources.limits.generalConstantMatrixVectorIndexing = 1;
}

EShLanguage VulkanShaderModule::FindLanguage(const VkShaderStageFlagBits shaderType)
{
	switch (shaderType)
	{
	case VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT:
		return EShLangVertex;

	case VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
		return EShLanguage::EShLangTessControl;

	case VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
		return EShLanguage::EShLangTessEvaluation;

	case VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT:
		return EShLanguage::EShLangGeometry;

	case VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT:
		return EShLanguage::EShLangFragment;

	case VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT:
		return EShLanguage::EShLangCompute;

	default:
		return EShLanguage::EShLangVertex;
	}
}
#endif // USE_GLSLANG