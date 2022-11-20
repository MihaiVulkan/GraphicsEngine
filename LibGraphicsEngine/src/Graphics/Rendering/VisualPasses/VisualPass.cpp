#include "Graphics/Rendering/VisualPasses/VisualPass.hpp"
#include "Graphics/Rendering/VisualEffects/VisualEffect.hpp"
#include "Graphics/Rendering/Renderer.hpp"
#include "Graphics/Rendering/Resources/UniformBuffer.hpp"
#include "Graphics/ShaderTools/GLSL/GLSLShaderTypes.hpp"
#include "Graphics/ShaderTools/GLSL/GLSLShaderParser.hpp"
#include "Graphics/SceneGraph/GeometryNode.hpp"
#include "Graphics/SceneGraph/LightNode.hpp"
#include "Foundation/Logger.hpp"
#include <string>
#include <cassert>
#if defined(VULKAN_RENDERER)
#include "Graphics/Rendering/Backends/Vulkan/VisualPasses/VulkanVisualPass.hpp"
#define IS_GL_NDK 0
#elif defined(OPENGL_RENDERER)
#include "Graphics/Rendering/Backends/OpenGL/VisualPasses/OpenGLVisualPass.hpp"
#define IS_GL_NDK 1
#endif //

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VisualPass::VisualPass()
	: mPassType(PassType::GE_PT_COUNT)
	, mTransform(glm::mat4(1.0f)) //identity matrix
	, mPrimitiveTopology(VisualPass::PrimitiveTopology::GE_PT_COUNT)
	, mFaceWinding(VisualPass::FaceWinding::GE_FW_COUNT)
	, mPolygonMode(VisualPass::PolygonMode::GE_PM_COUNT)
	, mpVisualEffect(nullptr)
	, mpGeoNode(nullptr)
	, mpGADVisualPass(nullptr)
	, mpRenderer(nullptr)
	, mSkipUniformsSetup(false)
	, mIsDebug(false)
{}

VisualPass::VisualPass(VisualPass::PassType type)
	: VisualPass()
{
	mPassType = type;

	SetupDefaultPrimitives();
	SetupDefaultPipelineStates();
}

VisualPass::~VisualPass()
{
	mpVisualEffect = nullptr;
	mpGeoNode = nullptr;
	mpGADVisualPass = nullptr;
	mpRenderer = nullptr;
}

void VisualPass::Init(Renderer* pRenderer, VisualEffect* pVisualEffect)
{
	assert(pRenderer != nullptr);
	assert(pVisualEffect != nullptr);

	mpRenderer = pRenderer;
	mpVisualEffect = pVisualEffect;

	SetupRenderData();

	SetupUniforms();
	SetupShadowUniforms();

	BindLights();

	mpGADVisualPass = mpRenderer->Get(this);
	assert(mpGADVisualPass != nullptr);
}

void VisualPass::RenderNode(uint32_t currentBufferIdx)
{
	assert(mpGADVisualPass != nullptr);

	mpGADVisualPass->RenderNode(currentBufferIdx);
}

void VisualPass::UpdateNode(Camera* pCamera, float32_t crrTime)
{
	assert(pCamera != nullptr);
	assert(mpGADVisualPass != nullptr);

	mpGADVisualPass->UpdateNode(pCamera, crrTime);
}

void VisualPass::SetupRenderData()
{
	assert(mpRenderer != nullptr);

	switch(mPassType)
	{
	case PassType::GE_PT_STANDARD:
	{
		mRenderData.width = mpRenderer->GetWindowWidth();
		mRenderData.height = mpRenderer->GetWindowHeight();
		mRenderData.depth = 1.0f;
		mRenderData.stencil = 0.0f;
	} break;
	case PassType::GE_PT_OFFSCREEN:
	{
		assert(mRenderTargetMap.size() == 2);

		auto* pColorRT = mRenderTargetMap.begin()->second;
		assert(pColorRT != nullptr);

		mRenderData.width = pColorRT->GetWidth();
		mRenderData.height = pColorRT->GetHeight();
		mRenderData.depth = 1.0f;
		mRenderData.stencil = 0.0f;
	} break;
	case PassType::GE_PT_SHADOWS:
	{
		assert(mRenderTargetMap.size() == 1);

		auto* pDepthRT = mRenderTargetMap.begin()->second;
		assert(pDepthRT != nullptr);

		mRenderData.width = pDepthRT->GetWidth();
		mRenderData.height = pDepthRT->GetHeight();
		mRenderData.depth = 1.0f;
		mRenderData.stencil = 0.0f;
	} break;
	case PassType::GE_PT_COUNT:
		LOG_ERROR("Invalid pass type!");
		break;
	}
}

void VisualPass::SetupDefaultPrimitives()
{
	// set default states if not set

	if (mPrimitiveTopology == VisualPass::PrimitiveTopology::GE_PT_COUNT)
		mPrimitiveTopology = VisualPass::PrimitiveTopology::GE_PT_TRIANGLE_LIST;
	if (mFaceWinding == VisualPass::FaceWinding::GE_FW_COUNT)
		mFaceWinding = VisualPass::FaceWinding::GE_FW_COUNTER_CLOCKWISE;
	if (mPolygonMode == VisualPass::PolygonMode::GE_PM_COUNT)
		mPolygonMode = VisualPass::PolygonMode::GE_PM_FILL;
}

void VisualPass::SetupDefaultPipelineStates()
{
	//set default values for the pipeline states
	CullFaceState cullState(true);
	cullState.SetCullMode(CullFaceState::CullMode::GE_CM_BACK);
	mCullFaceState = cullState;

	// depth test enabled
	DepthStencilState depthStencilState(true);
	depthStencilState.SetIsDepthEnabled(true);
	//depthStencilState.SetIsDepthWritable(true);
	depthStencilState.SetDepthCompareOp(DepthStencilState::CompareOp::GE_CO_LESS_OR_EQUAL);

	// stencil test disabled
	depthStencilState.SetIsStencilEnabled(false);
	mDepthStencilState = depthStencilState;

	ColorBlendState colorBlendState(true);
	colorBlendState.SetIsBlendEnabled(false);
	colorBlendState.SetSrcColorBlendFactor(ColorBlendState::BlendFactor::GE_BF_ZERO);
	colorBlendState.SetDstColorBlendFactor(ColorBlendState::BlendFactor::GE_BF_ZERO);
	colorBlendState.SetColorBlendOp(ColorBlendState::BlendOp::GE_BO_ADD);
	colorBlendState.SetSrcAlphaBlendFactor(ColorBlendState::BlendFactor::GE_BF_ZERO);
	colorBlendState.SetDstAlphaBlendFactor(ColorBlendState::BlendFactor::GE_BF_ZERO);
	colorBlendState.SetAlphaBlendOp(ColorBlendState::BlendOp::GE_BO_ADD);
	colorBlendState.SetColorWriteMask(0x0f);
	mColorBlendState = colorBlendState;

	DynamicState dynamicState;
	dynamicState.Add(DynamicState::State::GE_DS_VIEWPORT);
	dynamicState.Add(DynamicState::State::GE_DS_SCISSOR);
	if (mPassType == PassType::GE_PT_SHADOWS)
	{
		dynamicState.Add(DynamicState::State::GE_DS_DEPTH_BIAS);
	}
	mDynamicState = dynamicState;
}

void VisualPass::SetupUniforms()
{
	if (mSkipUniformsSetup)
		return;

	if (mPassType == VisualPass::PassType::GE_PT_SHADOWS)
		return;

	assert(false == mShaderMap.empty());

	///////// SETUP PVM MATRIX 
	auto it = mShaderMap.find(Shader::ShaderStage::GE_SS_VERTEX);
	assert(it != mShaderMap.end());

	auto* pVertexShader = it->second;
	assert(pVertexShader != nullptr);

	auto* pVertParser = pVertexShader->GetGLSLParser();
	assert(pVertParser != nullptr);

	// uniform buffer is present
	if (pVertParser->GetUniformBlock().IsValid())
	{
		auto& uboMembers = pVertParser->GetUniformBlock().members;

		UniformBuffer* pUB = nullptr;
		bool_t shouldAddUb = false;
		auto ubIt = mUniformBufferMap.find(Shader::ShaderStage::GE_SS_VERTEX);
		if (ubIt != mUniformBufferMap.end())
		{
			pUB = ubIt->second;
		}
		else
		{
			pUB = GE_ALLOC(UniformBuffer);
			shouldAddUb = true;
		}
		assert(pUB != nullptr);

		auto it = uboMembers.find(GLSLShaderTypes::Constants::UNIFORM_PVM_MATRIX); // mandatory uniform for all vertex shaders!);
		if (it != uboMembers.end() && it->second.type == GLSLShaderTypes::Constants::MAT4_TYPE)
		{
			pUB->AddUniform(GLSLShaderTypes::UniformType::GE_UT_PVM_MATRIX4); // no value added as it is gonna be updated per frame!
		}
		else
		{
			LOG_ERROR("PVM uniform not found in shader: %s! Abort!", pVertexShader->GetSourcePath().c_str());
			return;
		}

		if (shouldAddUb)
		{
			AddUniformBuffer(Shader::ShaderStage::GE_SS_VERTEX, pUB);
		}
	}
	else
	{
		// NOTE! In debug mode we can have shaders like debugShadowRT.vert which don't have a UBO
		if (false == GetIsDebug())
		{
			LOG_ERROR("Vertex shader UniformBlock is invalid!");
		}
	}

	////////// SETUP LIGHT UNIFORMS FOR LIT EFFECTS
	if (mpVisualEffect->GetEffectType() != VisualEffect::EffectType::GE_ET_LIT &&
		mpVisualEffect->GetEffectType() != VisualEffect::EffectType::GE_ET_LIT_SHADOWS)
		return;

	auto* pVertexParser = pVertexShader->GetGLSLParser();
	assert(pVertexParser != nullptr);

	// uniform buffer is present
	if (pVertParser->GetUniformBlock().IsValid())
	{
		auto* pUB = GetUniformBuffer(Shader::ShaderStage::GE_SS_VERTEX);
		assert(pUB != nullptr);

		auto& uboMembers = pVertParser->GetUniformBlock().members;

		auto it = uboMembers.find(GLSLShaderTypes::Constants::UNIFORM_MODEL_MATRIX);
		if (it != uboMembers.end() && it->second.type == GLSLShaderTypes::Constants::MAT4_TYPE)
		{
			pUB->AddUniform(GLSLShaderTypes::UniformType::GE_UT_MODEL_MATRIX4); // no value added as it is gonna be updated per frame!
		}
		else
		{
			LOG_ERROR("Model matrix uniform not found in shader: %s! Abort!", pVertexShader->GetSourcePath().c_str());
			return;
		}

		it = uboMembers.find(GLSLShaderTypes::Constants::UNIFORM_NORMAL_MATRIX);
		if (it != uboMembers.end() && it->second.type == GLSLShaderTypes::Constants::MAT4_TYPE)
		{
			pUB->AddUniform(GLSLShaderTypes::UniformType::GE_UT_NORMAL_MATRIX4); // no value added as it is gonna be updated per frame!
		}
		else
		{
			LOG_ERROR("Normal matrix uniform not found in shader: %s! Abort!", pVertexShader->GetSourcePath().c_str());
			return;
		}

		it = uboMembers.find(GLSLShaderTypes::Constants::UNIFORM_CAMERA_POS);
		if (it != uboMembers.end() && it->second.type == GLSLShaderTypes::Constants::VEC4_TYPE)
		{
			pUB->AddUniform(GLSLShaderTypes::UniformType::GE_UT_CAMERA_POS); // no value added as it is gonna be updated per frame!
		}
		else
		{
			LOG_ERROR("Camera pos uniform not found in shader: %s! Abort!", pVertexShader->GetSourcePath().c_str());
			return;
		}

		// some lighting effect need this flag, but not all effect
		it = uboMembers.find(GLSLShaderTypes::Constants::UNIFORM_IS_GL_NDK);
		if (it != uboMembers.end() && it->second.type == GLSLShaderTypes::Constants::INT_TYPE)
		{
			// NOTE! Vulkan NDK is in [0, 1], OpenGL NDK is in [-1, 1]
			// we need the shadow coords in [0, 1] texture space
			// in case of OpenGL we transform the coords in [-1, 1] to [0, 1]

			pUB->AddUniform(GLSLShaderTypes::UniformType::GE_UT_IS_GL_NDK, IS_GL_NDK);
		}
	}

	it = mShaderMap.find(Shader::ShaderStage::GE_SS_FRAGMENT);
	assert(it != mShaderMap.end());

	auto* pFragmentShader = it->second;
	assert(pFragmentShader != nullptr);

	auto* pFragParser = pFragmentShader->GetGLSLParser();
	assert(pFragParser != nullptr);

	if (pFragParser->GetUniformBlock().IsValid())
	{
		auto& uboMembers = pFragParser->GetUniformBlock().members;

		UniformBuffer* pUB = nullptr;
		bool_t shouldAddUb = false;
		auto ubIt = mUniformBufferMap.find(Shader::ShaderStage::GE_SS_FRAGMENT);
		if (ubIt != mUniformBufferMap.end())
		{
			pUB = ubIt->second;
		}
		else
		{
			pUB = GE_ALLOC(UniformBuffer);
			shouldAddUb = true;
		}
		assert(pUB != nullptr);

		// in case it's directional light
		bool_t hasDirLight = false;
		auto dirLightIt = uboMembers.find(GLSLShaderTypes::Constants::UNIFORM_LIGHT_DIR);
		if (dirLightIt != uboMembers.end())
		{
			hasDirLight = true;
		}

		// in case it's point light
		bool_t hasPointLight = false;
		auto pointLightIt = uboMembers.find(GLSLShaderTypes::Constants::UNIFORM_LIGHT_POS);
		if (pointLightIt != uboMembers.end())
		{
			hasPointLight = true;
		}

		if (hasDirLight && dirLightIt->second.type == GLSLShaderTypes::Constants::VEC4_TYPE)
		{
			pUB->AddUniform(GLSLShaderTypes::UniformType::GE_UT_LIGHT_DIR); // no value added as it is gonna be set later
		}
		else
		{
			if (hasPointLight == false)
			{
				LOG_ERROR("Light dir uniform not found in shader: %s! Abort!", pFragmentShader->GetSourcePath().c_str());
				return;
			}
		}

		if (hasPointLight && pointLightIt->second.type == GLSLShaderTypes::Constants::VEC4_TYPE)
		{
			pUB->AddUniform(GLSLShaderTypes::UniformType::GE_UT_LIGHT_POS); // no value added as it is gonna be set later
		}
		else
		{
			if (hasDirLight == false)
			{
				LOG_ERROR("Light dir uniform not found in shader: %s! Abort!", pFragmentShader->GetSourcePath().c_str());
				return;
			}
		}

		auto it = uboMembers.find(GLSLShaderTypes::Constants::UNIFORM_LIGHT_COLOR);
		if (it != uboMembers.end() && it->second.type == GLSLShaderTypes::Constants::VEC4_TYPE)
		{
			pUB->AddUniform(GLSLShaderTypes::UniformType::GE_UT_LIGHT_COLOR); // no value added as it is gonna be set later
		}
		else
		{
			LOG_ERROR("Light color uniform not found in shader: %s! Abort!", pFragmentShader->GetSourcePath().c_str());
			return;
		}

		if (shouldAddUb)
		{
			AddUniformBuffer(Shader::ShaderStage::GE_SS_FRAGMENT, pUB);
		}
	}
}

void VisualPass::SetupShadowUniforms()
{
	if (mpVisualEffect->GetEffectType() != VisualEffect::EffectType::GE_ET_LIT_SHADOWS)
		return;

	assert(false == mShaderMap.empty());

	///////// SETUP LIGHT PVM MATRIX 
	auto it = mShaderMap.find(Shader::ShaderStage::GE_SS_VERTEX);
	assert(it != mShaderMap.end());

	auto* pVertexShader = it->second;
	assert(pVertexShader != nullptr);

	auto* pVertParser = pVertexShader->GetGLSLParser();
	assert(pVertParser != nullptr);

	// uniform buffer is present
	if (pVertParser->GetUniformBlock().IsValid())
	{
		auto& uboMembers = pVertParser->GetUniformBlock().members;

		UniformBuffer* pUB = nullptr;
		bool_t shouldAddUb = false;
		auto ubIt = mUniformBufferMap.find(Shader::ShaderStage::GE_SS_VERTEX);
		if (ubIt != mUniformBufferMap.end())
		{
			pUB = ubIt->second;
		}
		else
		{
			pUB = GE_ALLOC(UniformBuffer);
			shouldAddUb = true;
		}
		assert(pUB != nullptr);

		auto it = uboMembers.find(GLSLShaderTypes::Constants::UNIFORM_LIGHT_PVM_MATRIX); // light MVP;
		if (it != uboMembers.end() && it->second.type == GLSLShaderTypes::Constants::MAT4_TYPE)
		{
			pUB->AddUniform(GLSLShaderTypes::UniformType::GE_UT_LIGHT_PVM_MATRIX4); // no value added as it is gonna be updated per frame!
		}
		else
		{
			LOG_ERROR("Light PVM matrix uniform not found in shader: %s! Abort!", pVertexShader->GetSourcePath().c_str());
			return;
		}

		it = uboMembers.find(GLSLShaderTypes::Constants::UNIFORM_MODEL_MATRIX); // Model;
		if (it != uboMembers.end() && it->second.type == GLSLShaderTypes::Constants::MAT4_TYPE)
		{
			pUB->AddUniform(GLSLShaderTypes::UniformType::GE_UT_MODEL_MATRIX4); // no value added as it is gonna be updated per frame!
		}
		else
		{
			LOG_ERROR("Model matrix uniform not found in shader: %s! Abort!", pVertexShader->GetSourcePath().c_str());
			return;
		}

		if (shouldAddUb)
		{
			AddUniformBuffer(Shader::ShaderStage::GE_SS_VERTEX, pUB);
		}
	}
}

void VisualPass::BindLights()
{
	assert(mpVisualEffect != nullptr);

	if (mpVisualEffect->GetEffectType() != VisualEffect::EffectType::GE_ET_LIT &&
		mpVisualEffect->GetEffectType() != VisualEffect::EffectType::GE_ET_LIT_SHADOWS)
		return;

	assert(mpRenderer != nullptr);

	auto* pRenderQueue = mpRenderer->GetRenderQueue();
	assert(pRenderQueue != nullptr);

	if (false == pRenderQueue->HasLights())
	{
		LOG_INFO("No lights to bind!");
		return;
	}

	auto* pNode = GetNode();
	if (pNode && pNode->IsLit())
	{
		pRenderQueue->ForEach([&, this](const LightNode* pLightNode)
			{
				mpRenderer->BindLight(this, pLightNode, pNode);
			});
	}
}


void VisualPass::AddShader(Shader* pShader)
{
	assert(pShader != nullptr);

	mShaderMap[pShader->GetShaderStage()] = pShader;
}

Shader* VisualPass::GetShader(Shader::ShaderStage shaderStage)
{
	assert(shaderStage < Shader::ShaderStage::GE_SS_COUNT);

	auto iter = mShaderMap.find(shaderStage);

	if (iter != mShaderMap.end())
		return iter->second;

	return nullptr;
}

const VisualPass::ShaderMap& VisualPass::GetShaders() const
{
	return mShaderMap;
}

bool_t VisualPass::HasShaders() const
{
	return mShaderMap.empty() == false;
}

void VisualPass::AddTexture(Shader::ShaderStage shaderStage, Texture* pTexture)
{
	assert(shaderStage < Shader::ShaderStage::GE_SS_COUNT);
	assert(pTexture != nullptr);

	auto& textures = mTextureMap[shaderStage];

	bool_t foundTexture = false;
	for (auto& pTex : textures)
	{
		if (pTex == pTexture)
		{
			foundTexture = true;
			break;
		}
	}

	if (! foundTexture)
		textures.push_back(pTexture);
}

Texture* VisualPass::GetTexture(Shader::ShaderStage shaderStage, uint8_t id) const
{
	assert(shaderStage < Shader::ShaderStage::GE_SS_COUNT);

	auto iter = mTextureMap.find(shaderStage);

	if (iter != mTextureMap.end())
	{
		const auto& textures = iter->second;
		assert(id < textures.size());

		return textures[id];
	}

	return nullptr;
}
const VisualPass::TextureMap& VisualPass::GetTextures() const
{
	return mTextureMap;
}

bool_t VisualPass::HasTextures() const
{
	return mTextureMap.empty() == false;
}

void VisualPass::AddRenderTarget(RenderTarget* pRenderTarget)
{
	assert(pRenderTarget != nullptr);

	mRenderTargetMap[pRenderTarget->GetTargetType()] = pRenderTarget;
}

RenderTarget* VisualPass::GetRenderTarget(RenderTarget::TargetType targetType)
{
	assert(targetType < RenderTarget::TargetType::GE_TT_COUNT);

	auto iter = mRenderTargetMap.find(targetType);

	if (iter != mRenderTargetMap.end())
	{
		return iter->second;
	}

	return nullptr;
}

const VisualPass::RenderTargetMap& VisualPass::GetRenderTargets() const
{
	return mRenderTargetMap;
}

bool_t VisualPass::HasRenderTargets() const
{
	return mRenderTargetMap.empty() == false;
}

void VisualPass::AddUniformBuffer(Shader::ShaderStage shaderStage, UniformBuffer* pUniformBuffer)
{
	assert(shaderStage < Shader::ShaderStage::GE_SS_COUNT);
	assert(pUniformBuffer != nullptr);

	mUniformBufferMap[shaderStage] = pUniformBuffer;
}

UniformBuffer* VisualPass::GetUniformBuffer(Shader::ShaderStage shaderStage) const
{
	assert(shaderStage < Shader::ShaderStage::GE_SS_COUNT);

	auto iter = mUniformBufferMap.find(shaderStage);

	if (iter != mUniformBufferMap.end())
		return iter->second;

	return nullptr;
}

const VisualPass::UniformBufferMap& VisualPass::GetUniformBuffers() const
{
	return mUniformBufferMap;
}

bool_t VisualPass::HasUniformBuffers() const
{
	return mUniformBufferMap.empty() == false;
}

const CullFaceState& VisualPass::GetCullFaceState() const
{
	return mCullFaceState;
}

void VisualPass::SetCullFaceState(const CullFaceState& cullFaceState)
{
	mCullFaceState = cullFaceState;
}

const DepthStencilState& VisualPass::GetDepthStencilState() const
{;
	return mDepthStencilState;
}

void VisualPass::SetDepthStencilState(const DepthStencilState& depthStencilState)
{
	mDepthStencilState = depthStencilState;
}

const ColorBlendState& VisualPass::GetColorBlendState() const
{
	return mColorBlendState;
}

void VisualPass::SetColorBlendState(const ColorBlendState& colorBlendState)
{
	mColorBlendState = colorBlendState;
}

const DynamicState& VisualPass::GetDynamicState() const
{
	return mDynamicState;
}

void VisualPass::SetDynamicState(const DynamicState& dynamicState)
{
	mDynamicState = dynamicState;
}

void VisualPass::SetTransform(const glm::mat4& transform)
{
	mTransform = transform;
}

const glm::mat4& VisualPass::GetTransform()
{
	return mTransform;
}

VisualPass::PrimitiveTopology VisualPass::GetPrimitiveTopology() const
{
	return mPrimitiveTopology;
}

void VisualPass::SetPrimitiveTopology(VisualPass::PrimitiveTopology topology)
{
	if (topology != VisualPass::PrimitiveTopology::GE_PT_COUNT)
		mPrimitiveTopology = topology;
}

VisualPass::FaceWinding VisualPass::GetFaceWinding() const
{
	return mFaceWinding;
}

void VisualPass::SetFaceWinding(VisualPass::FaceWinding winding)
{
	if (winding != VisualPass::FaceWinding::GE_FW_COUNT)
		mFaceWinding = winding;
}

VisualPass::PolygonMode VisualPass::GetPolygonMode() const
{
	return mPolygonMode;
}

void VisualPass::SetPolygonMode(VisualPass::PolygonMode mode)
{
	if (mode != VisualPass::PolygonMode::GE_PM_COUNT)
		mPolygonMode = mode;
}

const VisualPass::RenderData& VisualPass::GetRenderData() const
{
	return mRenderData;
}

void VisualPass::SetRenderData(const VisualPass::RenderData& data)
{
	mRenderData = data;
}

GeometryNode* VisualPass::GetNode()
{
	return mpGeoNode;
}

void VisualPass::SetNode(GeometryNode* pGeoNode)
{
	mpGeoNode = pGeoNode;
}

VisualEffect* VisualPass::GetVisualEffect()
{
	return mpVisualEffect;
}

VisualPass::PassType VisualPass::GetPassType() const
{
	return mPassType;
}

std::string VisualPass::PassTypeToStr(VisualPass::PassType type)
{
	std::string str;
	switch (type)
	{
	case PassType::GE_PT_OFFSCREEN:
		str = "Offscreen";
		break;
	case PassType::GE_PT_SHADOWS:
		str = "Shadows";
		break;
	case PassType::GE_PT_STANDARD:
		str = "Standard";
		break;
	case PassType::GE_PT_COUNT:
	default:
		LOG_ERROR("Invalid visual pass type");
	}

	return str;
}

void VisualPass::SkipUniformsSetup()
{
	mSkipUniformsSetup = true;
}

void VisualPass::SetIsDebug(bool_t val)
{
	mIsDebug = val;
}

bool_t VisualPass::GetIsDebug() const
{
	return mIsDebug;
}