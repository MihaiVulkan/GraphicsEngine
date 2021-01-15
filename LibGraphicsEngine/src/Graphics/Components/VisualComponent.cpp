#include "Graphics/Components/VisualComponent.hpp"
#include "Graphics/Rendering/Resources/UniformBuffer.hpp"
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
}

void VisualComponent::Destroy()
{
	mShaderMap.clear();
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

Shader* VisualComponent::GetShader(Shader::ShaderStage stage) const
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

const CullFaceState& VisualComponent::GetCullFaceState() const
{
	return mCullFaceState;
}

void VisualComponent::SetCullFaceState(const CullFaceState& cullFaceState)
{
	mCullFaceState = cullFaceState;
}

const DepthStencilState& VisualComponent::GetDepthStencilState() const
{
	return mDepthStencilState;
}

void VisualComponent::SetDepthStencilState(const DepthStencilState& depthStencilState)
{
	mDepthStencilState = depthStencilState;
}

const ColorBlendState& VisualComponent::GetColorBlendState() const
{
	return mColorBlendState;
}

void VisualComponent::SetColorBlendState(const ColorBlendState& colorBlendState)
{
	mColorBlendState = colorBlendState;
}

const DynamicState& VisualComponent::GetDynamicState() const
{
	return mDynamicState;
}

void VisualComponent::SetDynamicState(const DynamicState& dynamicState)
{
	mDynamicState = dynamicState;
}