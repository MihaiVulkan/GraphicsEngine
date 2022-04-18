#include "Graphics/Rendering/VisualEffects/UnlitEffects/UnlitCubemapTextureVisualEffect.hpp"
#include "Graphics/Rendering/Resources/Texture.hpp"
#include "Graphics/SceneGraph/GeometryNode.hpp"
#include "Graphics/Rendering/VisualPasses/VisualPass.hpp"
#include "Graphics/ShaderTools/GLSL/GLSLShaderTypes.hpp"
#include "Graphics/ShaderTools/GLSL/GLSLShaderParser.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"
#include <string>
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

UnlitCubemapTextureVisualEffect::UnlitCubemapTextureVisualEffect(TextureCubeMap* pTextureCubeMap)
	: VisualEffect(VisualEffect::EffectType::GE_ET_UNLIT)
	, mpTextureCubeMap(pTextureCubeMap)
{
	assert(mpTextureCubeMap != nullptr);

	mEffectName = GetClassName_();
}

UnlitCubemapTextureVisualEffect::~UnlitCubemapTextureVisualEffect()
{}

void UnlitCubemapTextureVisualEffect::InitCustomEffect()
{
	// All cubemap cube geoemtry needs reversed face winding
	SetFaceWinding(VisualPass::FaceWinding::GE_FW_CLOCKWISE);

	////NOTE! This effect needs only one visual pass

	auto* pPass = GE_ALLOC(VisualPass)(VisualPass::PassType::GE_PT_STANDARD);
	assert(pPass != nullptr);

	auto* pVertexShader = GE_ALLOC(Shader)(std::string() + GE_ASSET_PATH + "shaders/VisualEffects/UnlitEffects/unlitCubemapTexture.vert");
	assert(pVertexShader != nullptr);
	auto* pFragmentShader = GE_ALLOC(Shader)(std::string() + GE_ASSET_PATH + "shaders/VisualEffects/UnlitEffects/unlitCubemapTexture.frag");
	assert(pFragmentShader != nullptr);

	pPass->AddShader(pVertexShader);
	pPass->AddShader(pFragmentShader);

	auto* pFragParser = pFragmentShader->GetGLSLParser();
	assert(pFragParser != nullptr);

	const auto& uniforms = pFragParser->GetUniforms();
	auto it = uniforms.find(GLSLShaderTypes::Constants::UNIFORM_CUBEMAP_TEXTURE);
	if (it != uniforms.end() && it->second.type == GLSLShaderTypes::Constants::SAMPLER_CUBEMAP_TYPE)
	{
		pPass->AddTexture(Shader::ShaderStage::GE_SS_FRAGMENT, mpTextureCubeMap);
	}
	else
	{
		LOG_ERROR("Cubemap texture uniform not found in shader: %s! Abort!", pFragmentShader->GetSourcePath().c_str());
	}

	assert(mpTargetNode != nullptr);
	mpTargetNode->AddAllowedPass(VisualPass::PassType::GE_PT_STANDARD);
	pPass->SetNode(mpTargetNode);

	mPassMap[pPass->GetPassType()].push_back(pPass);
}