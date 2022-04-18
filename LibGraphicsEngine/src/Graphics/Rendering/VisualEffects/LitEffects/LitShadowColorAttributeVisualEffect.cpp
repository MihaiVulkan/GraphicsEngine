#include "Graphics/Rendering/VisualEffects/LitEffects/LitShadowColorAttributeVisualEffect.hpp"
#include "Graphics/Rendering/VisualPasses/VisualPass.hpp"
#include "Graphics/Rendering/Resources/Shader.hpp"
#include "Graphics/Rendering/Resources/Texture.hpp"
#include "Graphics/SceneGraph/GeometryNode.hpp"
#include "Graphics/ShaderTools/GLSL/GLSLShaderTypes.hpp"
#include "Graphics/ShaderTools/GLSL/GLSLShaderParser.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"
#include <string>
#include <cassert>

// shadow render target size
#define RT_WIDTH 2048
#define RT_HEIGHT 2048

//#define DEBUG_SHADOW_RT

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

RenderTarget* LitShadowColorAttributeVisualEffect::mpDepthRT = nullptr;

LitShadowColorAttributeVisualEffect::LitShadowColorAttributeVisualEffect()
	: VisualEffect(VisualEffect::EffectType::GE_ET_LIT_SHADOWS)
{
	mEffectName = GetClassName_();
}

LitShadowColorAttributeVisualEffect::~LitShadowColorAttributeVisualEffect()
{}

void LitShadowColorAttributeVisualEffect::InitCustomEffect()
{
	// This effect has 2 passes
	// 1. generaate shadow by rendering the scene offscreen from the light point of view
	// 2. apply the generated texture (RT) as texture to each node effect

	assert(mpTargetNode != nullptr);
	mpTargetNode->SetIsLit(true); // lit node !
	mpTargetNode->AddAllowedPass(VisualPass::PassType::GE_PT_STANDARD);
	mpTargetNode->AddAllowedPass(VisualPass::PassType::GE_PT_SHADOWS);

	//// 1st pass - generate shadow by rendering the scene offscreen from the light point of view
	//// only vertex shader needed for this pass

	if (mpDepthRT == nullptr)
	{
		mpDepthRT = GE_ALLOC(RenderTarget)(RenderTarget::TargetType::GE_TT_DEPTH, RenderTarget::TargetOutput::GE_TO_RENDER_SAMPLING, RT_WIDTH, RT_HEIGHT);
	}
	assert(mpDepthRT != nullptr);

	auto* pPass = GE_ALLOC(VisualPass)(VisualPass::PassType::GE_PT_SHADOWS);
	assert(pPass != nullptr);

	pPass->SetNode(mpTargetNode);

	pPass->AddRenderTarget(mpDepthRT);

	auto* pVertexShader = GE_ALLOC(Shader)(std::string() + GE_ASSET_PATH + "shaders/VisualEffects/LitEffects/litShadowCompute.vert");
	assert(pVertexShader != nullptr);

	pPass->AddShader(pVertexShader);

	mPassMap[pPass->GetPassType()].push_back(pPass);


	// 2nd pass - apply shadow to the node's effect
	pPass = GE_ALLOC(VisualPass)(VisualPass::PassType::GE_PT_STANDARD);
	assert(pPass != nullptr);

	pPass->SetNode(mpTargetNode);

#ifdef DEBUG_SHADOW_RT
	pVertexShader = GE_ALLOC(Shader)(std::string() + GE_ASSET_PATH + "shaders/VisualEffects/LitEffects/debugShadowRT.vert");
	assert(pVertexShader != nullptr);
	auto* pFragmentShader = GE_ALLOC(Shader)(std::string() + GE_ASSET_PATH + "shaders/VisualEffects/LitEffects/debugShadowRT.frag");
	assert(pFragmentShader != nullptr);

	pPass->SetIsDebug(true);
#else
	pVertexShader = GE_ALLOC(Shader)(std::string() + GE_ASSET_PATH + "shaders/VisualEffects/LitEffects/litShadowColorAttribute.vert");
	assert(pVertexShader != nullptr);
	auto* pFragmentShader = GE_ALLOC(Shader)(std::string() + GE_ASSET_PATH + "shaders/VisualEffects/LitEffects/litShadowColorAttribute.frag");
	assert(pFragmentShader != nullptr);
#endif
	pPass->AddShader(pVertexShader);
	pPass->AddShader(pFragmentShader);

	auto* pFragParser = pFragmentShader->GetGLSLParser();
	assert(pFragParser != nullptr);

	const auto& fragUniforms = pFragParser->GetUniforms();

	auto it = fragUniforms.find(GLSLShaderTypes::Constants::UNIFORM_2D_SHADOW_TEXTURE);
	if (it != fragUniforms.end() && it->second.type == GLSLShaderTypes::Constants::SAMPLER_2D_TYPE)
	{
		pPass->AddTexture(Shader::ShaderStage::GE_SS_FRAGMENT, mpDepthRT->GetTexture());
	}
	else
	{
		LOG_ERROR("2D shadow texture uniform not found in shader: %s! Abort!", pFragmentShader->GetSourcePath().c_str());
	}

	mPassMap[pPass->GetPassType()].push_back(pPass);
}