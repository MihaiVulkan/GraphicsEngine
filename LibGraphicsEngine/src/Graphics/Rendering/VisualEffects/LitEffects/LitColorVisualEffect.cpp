#include "Graphics/Rendering/VisualEffects/LitEffects/LitColorVisualEffect.hpp"
#include "Graphics/Rendering/VisualPasses/VisualPass.hpp"
#include "Graphics/Rendering/Resources/Shader.hpp"
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

LitColorVisualEffect::LitColorVisualEffect(const Color4f& color)
	: VisualEffect(VisualEffect::EffectType::GE_ET_LIT)
	, mColor(color)
{
	mEffectName = GetClassName_();
}

LitColorVisualEffect::~LitColorVisualEffect()
{}

void LitColorVisualEffect::InitCustomEffect()
{
	auto* pPass = GE_ALLOC(VisualPass)(VisualPass::PassType::GE_PT_STANDARD);
	assert(pPass != nullptr);

	auto* pVertexShader = GE_ALLOC(Shader)(std::string() + GE_ASSET_PATH + "shaders/VisualEffects/LitEffects/litColor.vert");
	assert(pVertexShader != nullptr);
	auto* pFragmentShader = GE_ALLOC(Shader)(std::string() + GE_ASSET_PATH + "shaders/VisualEffects/LitEffects/litColor.frag");
	assert(pFragmentShader != nullptr);

	pPass->AddShader(pVertexShader);
	pPass->AddShader(pFragmentShader);

	auto* pFragParser = pFragmentShader->GetGLSLParser();
	assert(pFragParser != nullptr);

	if (pFragParser->GetUniformBlock().IsValid())
	{
		auto& uboMembers = pFragParser->GetUniformBlock().members;

		auto* pUB = GE_ALLOC(UniformBuffer);
		assert(pUB != nullptr);

		auto it = uboMembers.find(GLSLShaderTypes::Constants::UNIFORM_COLOR);
		if (it != uboMembers.end() && it->second.type == GLSLShaderTypes::Constants::VEC4_TYPE)
		{
			pUB->AddUniform(GLSLShaderTypes::UniformType::GE_UT_COLOR_VEC4, mColor);
		}
		else
		{
			LOG_ERROR("Color uniform not found in shader: %s! Abort!", pFragmentShader->GetSourcePath().c_str());
		}
		pPass->AddUniformBuffer(Shader::ShaderStage::GE_SS_FRAGMENT, pUB);
	}

	assert(mpTargetNode != nullptr);
	mpTargetNode->SetIsLit(true); // lit node !
	mpTargetNode->AddAllowedPass(VisualPass::PassType::GE_PT_STANDARD);
	pPass->SetNode(mpTargetNode);

	mPassMap[pPass->GetPassType()].push_back(pPass);
}