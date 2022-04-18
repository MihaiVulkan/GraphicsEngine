#include "Graphics/Rendering/VisualEffects/LitEffects/LitMirrorTextureVisualEffect.hpp"
#include "Graphics/Rendering/VisualPasses/VisualPass.hpp"
#include "Graphics/Rendering/Resources/Shader.hpp"
#include "Graphics/Rendering/Resources/Texture.hpp"
#include "Graphics/Rendering/Resources/UniformBuffer.hpp"
#include "Graphics/SceneGraph/GeometryNode.hpp"
#include "Graphics/Components/VisualComponent.hpp"
#include "Graphics/ShaderTools/GLSL/GLSLShaderTypes.hpp"
#include "Graphics/ShaderTools/GLSL/GLSLShaderParser.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"
#include "glm/vec3.hpp"
#include "glm/ext/matrix_transform.hpp"
#include <string>
#include <cassert>

// offscreen render target size
#define RT_WIDTH 512
#define RT_HEIGHT 512

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

LitMirrorTextureVisualEffect::LitMirrorTextureVisualEffect(float32_t mirrorRoughness)
	: VisualEffect(VisualEffect::EffectType::GE_ET_LIT)
{
	mEffectName = GetClassName_();
	mMirrorRoughness = mirrorRoughness;
}

LitMirrorTextureVisualEffect::~LitMirrorTextureVisualEffect()
{}

void LitMirrorTextureVisualEffect::InitCustomEffect()
{
	//NOTE! This effect needs 2 visual passes:
	// 1st pass - compute the mirror effect in a RT with a list of impacted nodes
	// 2nd pass - apply the obtained effect on the target geometry 

	////// 1st pass - offscreen
	auto* pColorRT = GE_ALLOC(RenderTarget)(RenderTarget::TargetType::GE_TT_COLOR, RenderTarget::TargetOutput::GE_TO_RENDER_SAMPLING, RT_WIDTH, RT_HEIGHT);
	assert(pColorRT != nullptr);
	auto* pDepthRT = GE_ALLOC(RenderTarget)(RenderTarget::TargetType::GE_TT_DEPTH_STENCIL, RenderTarget::TargetOutput::GE_TO_RENDER, RT_WIDTH, RT_HEIGHT);
	assert(pDepthRT != nullptr);

	// NOTE! This pass will use the below transform and cull mode to affect the dependency nodes!
	CullFaceState cfs;
	cfs.SetCullMode(CullFaceState::CullMode::GE_CM_FRONT);

	glm::mat4 transform(glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, -1.0f, 1.0f)));

	assert(false == mDependencyNodes.empty());

	for (auto& pGeoNode : mDependencyNodes)
	{
		if (pGeoNode)
		{
			auto* pPass = GE_ALLOC(VisualPass)(VisualPass::PassType::GE_PT_OFFSCREEN);
			assert(pPass != nullptr);

			pGeoNode->AddAllowedPass(VisualPass::PassType::GE_PT_OFFSCREEN);
			pPass->SetNode(pGeoNode);

			pPass->AddRenderTarget(pColorRT);
			pPass->AddRenderTarget(pDepthRT);

			pPass->SetCullFaceState(cfs);
			pPass->SetTransform(transform);

			pPass->SkipUniformsSetup(); //////////

			auto* pVisualEffect = pGeoNode->GetComponent<VisualComponent>()->GetVisualEffect();
			assert(pVisualEffect != nullptr);

			//TODO - improve this id the effect has several passes!
			auto& passMap = pVisualEffect->GetPasses();
			auto itStd = passMap.find(VisualPass::PassType::GE_PT_STANDARD);
			if (itStd != passMap.end())
			{
				auto& standardPass = itStd->second[0];
				auto& shaders = standardPass->GetShaders();
				for (auto& shaderIt : shaders)
				{
					pPass->AddShader(shaderIt.second);
				}
				auto& textures = standardPass->GetTextures();
				for (auto& textureIt : textures)
				{
					for (auto& tex : textureIt.second)
					{
						pPass->AddTexture(textureIt.first, tex);
					}
				}
				auto& uniformBuffers = standardPass->GetUniformBuffers();
				for (const auto& bufferIt : uniformBuffers)
				{
					auto* pNewUB = GE_ALLOC(UniformBuffer)(*bufferIt.second);
					pPass->AddUniformBuffer(bufferIt.first, pNewUB);
				}
			}

			mPassMap[pPass->GetPassType()].push_back(pPass);
		}
	}

	////////////////////////////////////

	// 2nd pass - standard
	auto* pPass = GE_ALLOC(VisualPass)(VisualPass::PassType::GE_PT_STANDARD);
	assert(pPass != nullptr);

	assert(mpTargetNode != nullptr);
	mpTargetNode->SetIsLit(true); // lit node !
	mpTargetNode->AddAllowedPass(VisualPass::PassType::GE_PT_STANDARD);
	pPass->SetNode(mpTargetNode);

	auto* pVertexShader = GE_ALLOC(Shader)(std::string() + GE_ASSET_PATH + "shaders/VisualEffects/LitEffects/litMirrorTexture.vert");
	assert(pVertexShader != nullptr);
	auto* pFragmentShader = GE_ALLOC(Shader)(std::string() + GE_ASSET_PATH + "shaders/VisualEffects/LitEffects/litMirrorTexture.frag");
	assert(pFragmentShader != nullptr);

	pPass->AddShader(pVertexShader);
	pPass->AddShader(pFragmentShader);

	auto normalTexture = GE_ALLOC(Texture2D)(std::string() + GE_ASSET_PATH + "textures/stonefloor01_normal_rgba.ktx2");
	assert(normalTexture != nullptr);

	auto* pFragParser = pFragmentShader->GetGLSLParser();
	assert(pFragParser != nullptr);

	auto& uboMembers = pFragParser->GetUniformBlock().members;

	// add surface roughness factor
	auto ubIt = uboMembers.find(GLSLShaderTypes::Constants::UNIFORM_ROUGHNESS);
	if (ubIt != uboMembers.end() && ubIt->second.type == GLSLShaderTypes::Constants::FLOAT_TYPE)
	{
		auto* pUB = GE_ALLOC(UniformBuffer);
		assert(pUB != nullptr);

		pUB->AddUniform(GLSLShaderTypes::UniformType::GE_UT_ROUGHNESS, mMirrorRoughness);

		pPass->AddUniformBuffer(Shader::ShaderStage::GE_SS_FRAGMENT, pUB);
	}
	else
	{
		LOG_ERROR("Roughness uniform not found in shader: %s! Abort!", pFragmentShader->GetSourcePath().c_str());
	}

	const auto& fragUniforms = pFragParser->GetUniforms();

	// add color render target texture as a sampler in fragment shader
	auto it = fragUniforms.find(GLSLShaderTypes::Constants::UNIFORM_2D_COLOR_TEXTURE);
	if (it != fragUniforms.end() && it->second.type == GLSLShaderTypes::Constants::SAMPLER_2D_TYPE)
	{
		pPass->AddTexture(Shader::ShaderStage::GE_SS_FRAGMENT, pColorRT->GetTexture());
	}
	else
	{
		LOG_ERROR("2D color texture uniform not found in shader: %s! Abort!", pFragmentShader->GetSourcePath().c_str());
	}

	it = fragUniforms.find(GLSLShaderTypes::Constants::UNIFORM_2D_NORMAL_TEXTURE);
	if (it != fragUniforms.end() && it->second.type == GLSLShaderTypes::Constants::SAMPLER_2D_TYPE)
	{
		pPass->AddTexture(Shader::ShaderStage::GE_SS_FRAGMENT, normalTexture);
	}
	else
	{
		LOG_ERROR("2D normal texture uniform not found in shader: %s! Abort!", pFragmentShader->GetSourcePath().c_str());
	}

	mPassMap[pPass->GetPassType()].push_back(pPass);
}