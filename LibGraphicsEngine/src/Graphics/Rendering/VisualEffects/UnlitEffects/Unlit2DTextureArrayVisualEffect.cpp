#include "Graphics/Rendering/VisualEffects/UnlitEffects/Unlit2DTextureArrayVisualEffect.hpp"
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

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

Unlit2DTextureArrayVisualEffect::Unlit2DTextureArrayVisualEffect(Texture2DArray* pTexture2DArray)
	: VisualEffect(VisualEffect::EffectType::GE_ET_UNLIT)
	, mpTexture2DArray(pTexture2DArray)
{
	assert(mpTexture2DArray != nullptr);

	mEffectName = GetClassName_();
}

Unlit2DTextureArrayVisualEffect::~Unlit2DTextureArrayVisualEffect()
{}

void Unlit2DTextureArrayVisualEffect::InitCustomEffect()
{
	//NOTE! This effect needs only one visual pass

	auto* pPass = GE_ALLOC(VisualPass)(VisualPass::PassType::GE_PT_STANDARD);
	assert(pPass != nullptr);

	auto* pVertexShader = GE_ALLOC(Shader)(std::string() + GE_ASSET_PATH + "shaders/VisualEffects/UnlitEffects/unlit2DTextureArray.vert");
	assert(pVertexShader != nullptr);
	auto* pFragmentShader = GE_ALLOC(Shader)(std::string() + GE_ASSET_PATH + "shaders/VisualEffects/UnlitEffects/unlit2DTextureArray.frag");
	assert(pFragmentShader != nullptr);

	pPass->AddShader(pVertexShader);
	pPass->AddShader(pFragmentShader);

	auto* pFragParser = pFragmentShader->GetGLSLParser();
	assert(pFragParser != nullptr);

	const auto& uniforms = pFragParser->GetUniforms();
	auto it = uniforms.find(GLSLShaderTypes::Constants::UNIFORM_2D_TEXTURE_ARRAY);
	if (it != uniforms.end() && it->second.type == GLSLShaderTypes::Constants::SAMPLER_2D_ARRAY_TYPE)
	{
		pPass->AddTexture(Shader::ShaderStage::GE_SS_FRAGMENT, mpTexture2DArray);
	}
	else
	{
		LOG_ERROR("2D texture array uniform not found in shader: %s! Abort!", pFragmentShader->GetSourcePath().c_str());
	}

	assert(mpTargetNode != nullptr);
	mpTargetNode->AddAllowedPass(VisualPass::PassType::GE_PT_STANDARD);
	pPass->SetNode(mpTargetNode);

	mPassMap[pPass->GetPassType()].push_back(pPass);
}