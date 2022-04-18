#include "Graphics/Rendering/VisualEffects/LitEffects/LitColorAttributeVisualEffect.hpp"
#include "Graphics/Rendering/VisualPasses/VisualPass.hpp"
#include "Graphics/Rendering/Resources/Shader.hpp"
#include "Graphics/SceneGraph/GeometryNode.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"
#include <string>
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

LitColorAttributeVisualEffect::LitColorAttributeVisualEffect()
	: VisualEffect(VisualEffect::EffectType::GE_ET_LIT)
{
	mEffectName = GetClassName_();
}

LitColorAttributeVisualEffect::~LitColorAttributeVisualEffect()
{}

void LitColorAttributeVisualEffect::InitCustomEffect()
{
	auto* pPass = GE_ALLOC(VisualPass)(VisualPass::PassType::GE_PT_STANDARD);
	assert(pPass != nullptr);

	auto* pVertexShader = GE_ALLOC(Shader)(std::string() + GE_ASSET_PATH + "shaders/VisualEffects/LitEffects/litColorAttribute.vert");
	assert(pVertexShader != nullptr);
	auto* pFragmentShader = GE_ALLOC(Shader)(std::string() + GE_ASSET_PATH + "shaders/VisualEffects/LitEffects/litColorAttribute.frag");
	assert(pFragmentShader != nullptr);

	pPass->AddShader(pVertexShader);
	pPass->AddShader(pFragmentShader);

	assert(mpTargetNode != nullptr);
	mpTargetNode->SetIsLit(true); // lit node !
	mpTargetNode->AddAllowedPass(VisualPass::PassType::GE_PT_STANDARD);
	pPass->SetNode(mpTargetNode);

	mPassMap[pPass->GetPassType()].push_back(pPass);
}