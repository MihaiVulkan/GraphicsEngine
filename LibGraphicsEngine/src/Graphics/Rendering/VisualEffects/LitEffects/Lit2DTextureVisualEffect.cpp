#include "Graphics/Rendering/VisualEffects/LitEffects/Lit2DTextureVisualEffect.hpp"
#include "Graphics/Rendering/VisualPasses/VisualPass.hpp"
#include "Graphics/Rendering/Resources/Shader.hpp"
#include "Graphics/Rendering/Resources/Texture.hpp"
#include "Graphics/SceneGraph/GeometryNode.hpp"
#include "Graphics/Rendering/Resources/UniformBuffer.hpp"
#include "Graphics/ShaderTools/GLSL/GLSLShaderTypes.hpp"
#include "Graphics/ShaderTools/GLSL/GLSLShaderParser.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"
#include <string>
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

Lit2DTextureVisualEffect::Lit2DTextureVisualEffect(Texture2D* pTexture2D)
	: VisualEffect(VisualEffect::EffectType::GE_ET_LIT)
	, mpTexture2D(pTexture2D)
{
	assert(mpTexture2D != nullptr);

	mEffectName = GetClassName_();
}

Lit2DTextureVisualEffect::~Lit2DTextureVisualEffect()
{}

void Lit2DTextureVisualEffect::InitCustomEffect()
{
	//NOTE! This effect needs only one visual pass

	auto* pPass = GE_ALLOC(VisualPass)(VisualPass::PassType::GE_PT_STANDARD);
	assert(pPass != nullptr);

	auto* pVertexShader = GE_ALLOC(Shader)(std::string() + GE_ASSET_PATH + "shaders/VisualEffects/LitEffects/lit2DTexture.vert");
	assert(pVertexShader != nullptr);
	auto* pFragmentShader = GE_ALLOC(Shader)(std::string() + GE_ASSET_PATH + "shaders/VisualEffects/LitEffects/lit2DTexture.frag");
	assert(pFragmentShader != nullptr);

	pPass->AddShader(pVertexShader);
	pPass->AddShader(pFragmentShader);

	auto* pFragParser = pFragmentShader->GetGLSLParser();
	assert(pFragParser != nullptr);

	const auto& fragUniforms = pFragParser->GetUniforms();
	auto it = fragUniforms.find(GLSLShaderTypes::Constants::UNIFORM_2D_TEXTURE);
	if (it != fragUniforms.end() && it->second.type == GLSLShaderTypes::Constants::SAMPLER_2D_TYPE)
	{
		pPass->AddTexture(Shader::ShaderStage::GE_SS_FRAGMENT, mpTexture2D);
	}
	else
	{
		LOG_ERROR("2D texture uniform not found in shader: %s! Abort!", pFragmentShader->GetSourcePath().c_str());
	}

	assert(mpTargetNode != nullptr);
	mpTargetNode->SetIsLit(true); // lit node !
	mpTargetNode->AddAllowedPass(VisualPass::PassType::GE_PT_STANDARD);
	pPass->SetNode(mpTargetNode);

	mPassMap[pPass->GetPassType()].push_back(pPass);
}