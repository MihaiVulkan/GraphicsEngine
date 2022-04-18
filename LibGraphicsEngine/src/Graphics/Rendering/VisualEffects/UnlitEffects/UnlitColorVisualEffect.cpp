#include "Graphics/Rendering/VisualEffects/UnlitEffects/UnlitColorVisualEffect.hpp"
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


UnlitColorVisualEffect::UnlitColorVisualEffect(const Color4f& color)
	: VisualEffect(VisualEffect::EffectType::GE_ET_UNLIT)
	, mColor(color)
{
	mEffectName = GetClassName_();
}

UnlitColorVisualEffect::~UnlitColorVisualEffect()
{}

void UnlitColorVisualEffect::InitCustomEffect()
{
	//NOTE! This effect needs only one visual pass

	auto* pPass = GE_ALLOC(VisualPass)(VisualPass::PassType::GE_PT_STANDARD);
	assert(pPass != nullptr);
	auto* pVertexShader = GE_ALLOC(Shader)(std::string() + GE_ASSET_PATH + "shaders/VisualEffects/UnlitEffects/unlitColor.vert");
	assert(pVertexShader != nullptr);
	auto* pFragmentShader = GE_ALLOC(Shader)(std::string() + GE_ASSET_PATH + "shaders/VisualEffects/UnlitEffects/unlitColor.frag");
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

		auto it = uboMembers.find(GLSLShaderTypes::Constants::UNIFORM_COLOR); // mandatory uniform for all vertex shaders!);
		if (it != uboMembers.end() && it->second.type == GLSLShaderTypes::Constants::VEC4_TYPE)
		{
			pUB->AddUniform(GLSLShaderTypes::UniformType::GE_UT_COLOR_VEC4, mColor); // no value added as it is gonna be updated per frame!
		}
		else
		{
			LOG_ERROR("Color uniform not found in shader: %s! Abort!", pFragmentShader->GetSourcePath().c_str());
		}
		pPass->AddUniformBuffer(Shader::ShaderStage::GE_SS_FRAGMENT, pUB);
	}

	assert(mpTargetNode != nullptr);
	mpTargetNode->AddAllowedPass(VisualPass::PassType::GE_PT_STANDARD);
	pPass->SetNode(mpTargetNode);

	mPassMap[pPass->GetPassType()].push_back(pPass);
}