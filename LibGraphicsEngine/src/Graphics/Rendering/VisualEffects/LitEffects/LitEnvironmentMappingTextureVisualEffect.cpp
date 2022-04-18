#include "Graphics/Rendering/VisualEffects/LitEffects/LitEnvironmentMappingTextureVisualEffect.hpp"
#include "Graphics/Rendering/VisualPasses/VisualPass.hpp"
#include "Graphics/Rendering/Resources/Shader.hpp"
#include "Graphics/Rendering/Resources/Texture.hpp"
#include "Graphics/Rendering/Resources/UniformBuffer.hpp"
#include "Graphics/SceneGraph/GeometryNode.hpp"
#include "Graphics/ShaderTools/GLSL/GLSLShaderTypes.hpp"
#include "Graphics/ShaderTools/GLSL/GLSLShaderParser.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"
#include <string>
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

LitEnvironmentMappingTextureVisualEffect::LitEnvironmentMappingTextureVisualEffect(TextureCubeMap* pTextureCubeMap,
											LitEnvironmentMappingTextureVisualEffect::MappingType mappingType)
	: VisualEffect(VisualEffect::EffectType::GE_ET_LIT)
	, mMappingType(mappingType)
	, mpTextureCubeMap(pTextureCubeMap)
{
	assert(mMappingType < MappingType::GE_MT_COUNT);
	assert(mpTextureCubeMap != nullptr);

	mEffectName = GetClassName_();
}

LitEnvironmentMappingTextureVisualEffect::~LitEnvironmentMappingTextureVisualEffect()
{}

void LitEnvironmentMappingTextureVisualEffect::InitCustomEffect()
{
	//NOTE! This effect needs only one visual pass

	auto* pPass = GE_ALLOC(VisualPass)(VisualPass::PassType::GE_PT_STANDARD);
	assert(pPass != nullptr);

	auto* pVertexShader = GE_ALLOC(Shader)(std::string() + GE_ASSET_PATH + "shaders/VisualEffects/LitEffects/litEnvironmentMappingTexture.vert");
	assert(pVertexShader != nullptr);
	
	std::string fragShader = (mMappingType == MappingType::GE_MT_REFLECTIVE ? "litEnvironmentMappingTextureReflect.frag" : "litEnvironmentMappingTextureRefract.frag");
	auto* pFragmentShader = GE_ALLOC(Shader)(std::string() + GE_ASSET_PATH + "shaders/VisualEffects/LitEffects/" + fragShader);
	assert(pFragmentShader != nullptr);

	pPass->AddShader(pVertexShader);
	pPass->AddShader(pFragmentShader);

	auto* pFragParser = pFragmentShader->GetGLSLParser();
	assert(pFragParser != nullptr);

	const auto& fragUniforms = pFragParser->GetUniforms();
	auto it = fragUniforms.find(GLSLShaderTypes::Constants::UNIFORM_CUBEMAP_TEXTURE);
	if (it != fragUniforms.end() && it->second.type == GLSLShaderTypes::Constants::SAMPLER_CUBEMAP_TYPE)
	{
		pPass->AddTexture(Shader::ShaderStage::GE_SS_FRAGMENT, mpTextureCubeMap);
	}
	else
	{
		LOG_ERROR("Cubemap texture uniform not found in shader: %s! Abort!", pFragmentShader->GetSourcePath().c_str());
	}

	assert(mpTargetNode != nullptr);
	mpTargetNode->SetIsLit(true); // lit node !
	mpTargetNode->AddAllowedPass(VisualPass::PassType::GE_PT_STANDARD);
	pPass->SetNode(mpTargetNode);

	mPassMap[pPass->GetPassType()].push_back(pPass);
}