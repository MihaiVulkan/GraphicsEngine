#include "Graphics/Components/VisualComponent.hpp"
#include "Graphics/Rendering/Resources/UniformBuffer.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VisualComponent::VisualComponent()
	: NodeComponent()
{
	Create();
}

VisualComponent::VisualComponent(const std::string& name)
	: NodeComponent(name)
{
	Create();
}

VisualComponent::~VisualComponent()
{
	Destroy();
}

void VisualComponent::Create()
{
	SetName(GetClassName_());

	//set default values for the pipeline states
	CullFaceState cull_s(true);
	cull_s.SetCullMode(CullFaceState::CullMode::GE_CM_BACK);
	SetCullFaceState(cull_s);

	DepthStencilState depthStencil_s(true);
	depthStencil_s.SetIsDepthEnabled(true);
	depthStencil_s.SetIsDepthWritable(true);
	depthStencil_s.SetDepthCompareOp(DepthStencilState::CompareOp::GE_CO_LESS_OR_EQUAL);

	depthStencil_s.SetIsStencilEnabled(false);
	SetDepthStencilState(depthStencil_s);

	ColorBlendState colorBlend_s(true);
	colorBlend_s.SetIsBlendEnabled(false);
	colorBlend_s.SetSrcColorBlendFactor(ColorBlendState::BlendFactor::GE_BF_ZERO);
	colorBlend_s.SetDstColorBlendFactor(ColorBlendState::BlendFactor::GE_BF_ZERO);
	colorBlend_s.SetColorBlendOp(ColorBlendState::BlendOp::GE_BO_ADD);
	colorBlend_s.SetSrcAlphaBlendFactor(ColorBlendState::BlendFactor::GE_BF_ZERO);
	colorBlend_s.SetDstAlphaBlendFactor(ColorBlendState::BlendFactor::GE_BF_ZERO);
	colorBlend_s.SetAlphaBlendOp(ColorBlendState::BlendOp::GE_BO_ADD);
	colorBlend_s.SetColorWriteMask(0x0f);
	SetColorBlendState(colorBlend_s);

	DynamicState dynamic_s;
	dynamic_s.Add(DynamicState::State::GE_DS_VIEWPORT);
	dynamic_s.Add(DynamicState::State::GE_DS_SCISSOR);
	SetDynamicState(dynamic_s);

	// Uniform Buffer - vertex shader
	auto* pUB = GE_ALLOC(UniformBuffer);
	assert(pUB != nullptr);
	pUB->AddUniform(GLSLShaderTypes::UniformType::GE_UT_PVM_MATRIX4);

	AddUniformBuffer(Shader::ShaderStage::GE_SS_VERTEX, pUB);
}

void VisualComponent::Destroy()
{
	mShaderMap.clear();
	mTextureMap.clear();

	for (auto it = mUniformBufferMap.begin(); it != mUniformBufferMap.end(); ++it)
	{
		GE_FREE(it->second);
	}
	mUniformBufferMap.clear();
}

void VisualComponent::Start()
{
	//TODO
}

void VisualComponent::Update(float32_t deltaTime)
{
	//TODO
}

void VisualComponent::OnAttach()
{
	//TODO
}

void VisualComponent::OnDettach()
{
	//TODO
}

void VisualComponent::AddShader(Shader* pShader)
{
	assert(pShader != nullptr);

	mShaderMap[pShader->GetShaderStage()] = pShader;
}

Shader* VisualComponent::GetShader(Shader::ShaderStage stage)
{
	auto iter = mShaderMap.find(stage);

	if (iter != mShaderMap.end())
		return iter->second;

	return nullptr;
}

void VisualComponent::SetShaders(const VisualComponent::ShaderMap& shaders)
{
	mShaderMap = shaders;
}

const VisualComponent::ShaderMap& VisualComponent::GetShaders() const
{
	return mShaderMap;
}

void VisualComponent::AddTexture(Texture* pTexture, Shader::ShaderStage shaderStage)
{
	assert(shaderStage < Shader::ShaderStage::GE_SS_COUNT);
	assert(pTexture != nullptr);

	mTextureMap[shaderStage].push_back(pTexture);
}

bool_t VisualComponent::HasTextures() const
{
	return (mTextureMap.empty() == false);
}

const VisualComponent::TextureMap& VisualComponent::GetTextures() const
{
	return mTextureMap;
}

void VisualComponent::AddUniformBuffer(Shader::ShaderStage stage, UniformBuffer* pUniformBuffer)
{
	assert(stage < Shader::ShaderStage::GE_SS_COUNT);

	// nOTE! If stage alreau exists it shall be overwritten!
	mUniformBufferMap[stage] = pUniformBuffer;
}

UniformBuffer* VisualComponent::GetUniformBuffer(Shader::ShaderStage stage) const
{
	auto iter = mUniformBufferMap.find(stage);
	if (iter != mUniformBufferMap.end())
	{
		return iter->second;
	}

	return nullptr;
}

bool_t VisualComponent::HasUniformBuffers() const
{
	return (mUniformBufferMap.empty() != true);
}

CullFaceState& VisualComponent::GetCullFaceState()
{
	return mCullFaceState;
}

void VisualComponent::SetCullFaceState(const CullFaceState& cullFaceState)
{
	mCullFaceState = cullFaceState;
}

DepthStencilState& VisualComponent::GetDepthStencilState()
{
	return mDepthStencilState;
}

void VisualComponent::SetDepthStencilState(const DepthStencilState& depthStencilState)
{
	mDepthStencilState = depthStencilState;
}

ColorBlendState& VisualComponent::GetColorBlendState()
{
	return mColorBlendState;
}

void VisualComponent::SetColorBlendState(const ColorBlendState& colorBlendState)
{
	mColorBlendState = colorBlendState;
}

DynamicState& VisualComponent::GetDynamicState()
{
	return mDynamicState;
}

void VisualComponent::SetDynamicState(const DynamicState& dynamicState)
{
	mDynamicState = dynamicState;
}