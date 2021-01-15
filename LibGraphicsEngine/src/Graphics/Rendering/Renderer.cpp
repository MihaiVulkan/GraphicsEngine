#include "Graphics/Rendering/Renderer.hpp"

#include "Graphics/Rendering/Resources/VertexFormat.hpp"
#include "Graphics/Rendering/Resources/VertexBuffer.hpp"
#include "Graphics/Rendering/Resources/IndexBuffer.hpp"
#include "Graphics/Rendering/Resources/UniformBuffer.hpp"
#include "Graphics/Rendering/Resources/Texture.hpp"
#include "Graphics/Rendering/Resources/RenderTarget.hpp"
#include "Graphics/Rendering/Resources/RenderFrameBuffer.hpp"
#include "Graphics/Rendering/Resources/Shader.hpp"
#include "Graphics/Rendering/Resources/Material.hpp"
#include "Graphics/SceneGraph/GeometryNode.hpp"

// Resources
#if defined(VULKAN_RENDERER)
#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanVertexFormat.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanVertexBuffer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanIndexBuffer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanUniformBuffer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanTexture.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanRenderTarget.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanRenderFrameBuffer.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanShader.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanMaterial.hpp"
#endif // VULKAN_RENDERER

#include "Foundation/Platform/Platform.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

Renderer::Renderer()
	: mRendererType(RendererType::GE_RT_FORWARD)
	, mIsPrepared(false)
	, mWindowWidth(0)
	, mWindowHeight(0)
	, mpRenderQueue(nullptr)
	, mpRenderPass(nullptr)
{}

Renderer::Renderer(Renderer::RendererType type)
	: mRendererType(type)
	, mIsPrepared(false)
	, mWindowWidth(0)
	, mWindowHeight(0)
	, mpRenderQueue(nullptr)
	, mpRenderPass(nullptr)
{}

Renderer::~Renderer()
{ 
	Terminate();
}

void Renderer::Init(Platform::Window* pWindow)
{
	assert(pWindow != nullptr);
	
	pWindow->GetWindowSize(&mWindowWidth, &mWindowHeight);
}


void Renderer::Terminate()
{
	mWindowWidth = 0;
	mWindowHeight = 0;

	mpRenderQueue = nullptr;
	mpRenderPass = nullptr;

	CleanUpResources();
}

void Renderer::CleanUpResources()
{
	for (auto it = mVertexFormatMap.begin(); it != mVertexFormatMap.end(); ++it)
	{
		GE_FREE(it->second);
	}
	mVertexFormatMap.clear();

	for (auto it = mVertexBufferMap.begin(); it != mVertexBufferMap.end(); ++it)
	{
		GE_FREE(it->second);
	}
	mVertexBufferMap.clear();

	for (auto it = mIndexBufferMap.begin(); it != mIndexBufferMap.end(); ++it)
	{
		GE_FREE(it->second);
	}
	mIndexBufferMap.clear();

	for (auto it = mUniformBufferMap.begin(); it != mUniformBufferMap.end(); ++it)
	{
		GE_FREE(it->second);
	}
	mUniformBufferMap.clear();

	for (auto it = mTextureMap.begin(); it != mTextureMap.end(); ++it)
	{
		GE_FREE(it->second);
	}
	mTextureMap.clear();

	for (auto it = mRenderTargetMap.begin(); it != mRenderTargetMap.end(); ++it)
	{
		GE_FREE(it->second);
	}
	mRenderTargetMap.clear();

	for (auto it = mRenderFrameBufferMap.begin(); it != mRenderFrameBufferMap.end(); ++it)
	{
		GE_FREE(it->second);
	}
	mRenderFrameBufferMap.clear();

	for (auto it = mShaderMap.begin(); it != mShaderMap.end(); ++it)
	{
		GE_FREE(it->second);
	}
	mShaderMap.clear();

	for (auto it = mMaterialMap.begin(); it != mMaterialMap.end(); ++it)
	{
		GE_FREE(it->second);
	}
	mMaterialMap.clear();
}

GADRVertexFormat* Renderer::Bind(VertexFormat* pVertexFormat)
{
	assert(pVertexFormat != nullptr);

	auto pGADRVertexFormat = Get(pVertexFormat);
	
	if (pGADRVertexFormat)
		pGADRVertexFormat->OnBind();

	return pGADRVertexFormat;
}

void Renderer::UnBind(VertexFormat* pVertexFormat)
{
	assert(pVertexFormat != nullptr);

	auto iter = mVertexFormatMap.find(pVertexFormat);
	if (iter != mVertexFormatMap.end())
	{
		auto ref = iter->second;
		if (ref)
			ref->OnUnBind();

		GE_FREE(ref);
		mVertexFormatMap.erase(iter);
	}
}

GADRVertexFormat* Renderer::Get(VertexFormat* pVertexFormat)
{
	assert(pVertexFormat != nullptr);

	auto iter = mVertexFormatMap.find(pVertexFormat);
	if (iter != mVertexFormatMap.end())
	{
		return iter->second;
	}
	else
	{
		auto ref = mVertexFormatMap[pVertexFormat] = GE_ALLOC(GADRVertexFormat)(this, pVertexFormat);
		assert(ref != nullptr);

		return ref;
	}

	return nullptr;
}

GADRVertexBuffer* Renderer::Bind(VertexBuffer* pVertexBuffer, uint32_t currentBufferIdx)
{
	assert(pVertexBuffer != nullptr);

	auto pGADRVertexBuffer = Get(pVertexBuffer);
	
	if (pGADRVertexBuffer)
		pGADRVertexBuffer->OnBind(currentBufferIdx);

	return pGADRVertexBuffer;
}

void Renderer::UnBind(VertexBuffer* pVertexBuffer)
{
	assert(pVertexBuffer != nullptr);

	auto iter = mVertexBufferMap.find(pVertexBuffer);
	if (iter != mVertexBufferMap.end())
	{
		auto ref = iter->second;
		if (ref)
			ref->OnUnBind();

		GE_FREE(ref);
		mVertexBufferMap.erase(iter);
	}
}

GADRVertexBuffer* Renderer::Get(VertexBuffer* pVertexBuffer)
{
	assert(pVertexBuffer != nullptr);

	auto iter = mVertexBufferMap.find(pVertexBuffer);
	if (iter != mVertexBufferMap.end())
	{
		return iter->second;
	}
	else
	{
		auto ref = mVertexBufferMap[pVertexBuffer] = GE_ALLOC(GADRVertexBuffer)(this, pVertexBuffer);
		assert(ref != nullptr);

		return ref;
	}

	return nullptr;
}

GADRIndexBuffer* Renderer::Bind(IndexBuffer* pIndexBuffer, uint32_t currentBufferIdx)
{
	assert(pIndexBuffer != nullptr);

	auto pGADRIndexBuffer = Get(pIndexBuffer);

	if (pGADRIndexBuffer)
		pGADRIndexBuffer->OnBind(currentBufferIdx);

	return pGADRIndexBuffer;
}

void Renderer::UnBind(IndexBuffer* pIndexBuffer)
{
	assert(pIndexBuffer != nullptr);

	auto iter = mIndexBufferMap.find(pIndexBuffer);
	if (iter != mIndexBufferMap.end())
	{
		auto ref = iter->second;
		if (ref)
			ref->OnUnBind();

		GE_FREE(ref);
		mIndexBufferMap.erase(iter);
	}
}

GADRIndexBuffer* Renderer::Get(IndexBuffer* pIndexBuffer)
{
	assert(pIndexBuffer != nullptr);

	auto iter = mIndexBufferMap.find(pIndexBuffer);
	if (iter != mIndexBufferMap.end())
	{
		return iter->second;
	}
	else
	{
		auto ref = mIndexBufferMap[pIndexBuffer] = GE_ALLOC(GADRIndexBuffer)(this, pIndexBuffer);
		assert(ref != nullptr);

		return ref;
	}

	return nullptr;
}

GADRUniformBuffer* Renderer::Bind(UniformBuffer* pUniformBuffer)
{
	assert(pUniformBuffer != nullptr);

	auto pGADRUniformBuffer = Get(pUniformBuffer);

	if (pGADRUniformBuffer)
		pGADRUniformBuffer->OnBind();

	return pGADRUniformBuffer;
}

void Renderer::UnBind(UniformBuffer* pUniformBuffer)
{
	assert(pUniformBuffer != nullptr);

	auto iter = mUniformBufferMap.find(pUniformBuffer);
	if (iter != mUniformBufferMap.end())
	{
		auto ref = iter->second;
		if (ref)
			ref->OnUnBind();

		GE_FREE(ref);
		mUniformBufferMap.erase(iter);
	}
}

GADRUniformBuffer* Renderer::Get(UniformBuffer* pUniformBuffer)
{
	assert(pUniformBuffer != nullptr);

	auto iter = mUniformBufferMap.find(pUniformBuffer);
	if (iter != mUniformBufferMap.end())
	{
		return iter->second;
	}
	else
	{
		auto ref = mUniformBufferMap[pUniformBuffer] = GE_ALLOC(GADRUniformBuffer)(this, pUniformBuffer);
		assert(ref != nullptr);

		return ref;
	}

	return nullptr;
}

GADRTexture* Renderer::Bind(Texture* pTexture)
{
	assert(pTexture != nullptr);

	auto pGADRTexture = Get(pTexture);

	if (pGADRTexture)
		pGADRTexture->OnBind();

	return pGADRTexture;
}

void Renderer::UnBind(Texture* pTexture)
{
	assert(pTexture != nullptr);

	auto iter = mTextureMap.find(pTexture);
	if (iter != mTextureMap.end())
	{
		auto ref = iter->second;
		if (ref)
			ref->OnUnBind();

		GE_FREE(ref);
		mTextureMap.erase(iter);
	}
}

GADRTexture* Renderer::Get(Texture* pTexture)
{
	assert(pTexture != nullptr);

	auto iter = mTextureMap.find(pTexture);
	if (iter != mTextureMap.end())
	{
		return iter->second;
	}
	else
	{
		auto ref = mTextureMap[pTexture] = GE_ALLOC(GADRTexture)(this, pTexture);
		assert(ref != nullptr);

		return ref;
	}

	return nullptr;
}

GADRRenderTarget* Renderer::Bind(RenderTarget* pRenderTarget)
{
	assert(pRenderTarget != nullptr);

	auto pGADRRenderTarget = Get(pRenderTarget);

	if (pGADRRenderTarget)
		pGADRRenderTarget->OnBind();

	return pGADRRenderTarget;
}

void Renderer::UnBind(RenderTarget* pRenderTarget)
{
	assert(pRenderTarget != nullptr);

	auto iter = mRenderTargetMap.find(pRenderTarget);
	if (iter != mRenderTargetMap.end())
	{
		auto ref = iter->second;
		if (ref)
			ref->OnUnBind();

		GE_FREE(ref);
		mRenderTargetMap.erase(iter);
	}
}

GADRRenderTarget* Renderer::Get(RenderTarget* pRenderTarget)
{
	assert(pRenderTarget != nullptr);

	auto iter = mRenderTargetMap.find(pRenderTarget);
	if (iter != mRenderTargetMap.end())
	{
		return iter->second;
	}
	else
	{
		auto ref = mRenderTargetMap[pRenderTarget] = GE_ALLOC(GADRRenderTarget)(this, pRenderTarget);
		assert(ref != nullptr);

		return ref;
	}

	return nullptr;
}

GADRRenderFrameBuffer* Renderer::Bind(RenderFrameBuffer* pRenderFrameBuffer)
{
	assert(pRenderFrameBuffer != nullptr);

	auto pGADRRenderTarget = Get(pRenderFrameBuffer);

	if (pGADRRenderTarget)
		pGADRRenderTarget->OnBind();

	return pGADRRenderTarget;
}

void Renderer::UnBind(RenderFrameBuffer* pRenderFrameBuffer)
{
	assert(pRenderFrameBuffer != nullptr);

	auto iter = mRenderFrameBufferMap.find(pRenderFrameBuffer);
	if (iter != mRenderFrameBufferMap.end())
	{
		auto ref = iter->second;
		if (ref)
			ref->OnUnBind();

		GE_FREE(ref);
		mRenderFrameBufferMap.erase(iter);
	}
}

GADRRenderFrameBuffer* Renderer::Get(RenderFrameBuffer* pRenderFrameBuffer)
{
	assert(pRenderFrameBuffer != nullptr);

	auto iter = mRenderFrameBufferMap.find(pRenderFrameBuffer);
	if (iter != mRenderFrameBufferMap.end())
	{
		return iter->second;
	}
	else
	{
		auto ref = mRenderFrameBufferMap[pRenderFrameBuffer] = GE_ALLOC(GADRRenderFrameBuffer)(this, pRenderFrameBuffer);
		assert(ref != nullptr);

		return ref;
	}

	return nullptr;
}

GADRShader* Renderer::Bind(Shader* pShader)
{
	assert(pShader != nullptr);

	auto pGADRShader = Get(pShader);

	if (pGADRShader)
		pGADRShader->OnBind();

	return pGADRShader;
}

void Renderer::UnBind(Shader* pShader)
{
	assert(pShader != nullptr);

	auto iter = mShaderMap.find(pShader);
	if (iter != mShaderMap.end())
	{
		auto ref = iter->second;
		if (ref)
			ref->OnUnBind();

		GE_FREE(ref);
		mShaderMap.erase(iter);
	}
}

GADRShader* Renderer::Get(Shader* pShader)
{
	assert(pShader != nullptr);

	auto iter = mShaderMap.find(pShader);
	if (iter != mShaderMap.end())
	{
		return iter->second;
	}
	else
	{
		auto ref = mShaderMap[pShader] = GE_ALLOC(GADRShader)(this, pShader);
		assert(ref != nullptr);

		return ref;
	}

	return nullptr;
}

GADRMaterial* Renderer::Bind(Material* pMaterial)
{
	assert(pMaterial != nullptr);

	auto pGADRShader = Get(pMaterial);

	if (pGADRShader)
		pGADRShader->OnBind();

	return pGADRShader;
}

void Renderer::UnBind(Material* pMaterial)
{
	assert(pMaterial != nullptr);

	auto iter = mMaterialMap.find(pMaterial);
	if (iter != mMaterialMap.end())
	{
		auto ref = iter->second;
		if (ref)
			ref->OnUnBind();

		GE_FREE(ref);
		mMaterialMap.erase(iter);
	}
}

GADRMaterial* Renderer::Get(Material* pMaterial)
{
	assert(pMaterial != nullptr);

	auto iter = mMaterialMap.find(pMaterial);
	if (iter != mMaterialMap.end())
	{
		return iter->second;
	}
	else
	{
		auto ref = mMaterialMap[pMaterial] = GE_ALLOC(GADRMaterial)(this, pMaterial);
		assert(ref != nullptr);

		return ref;
	}

	return nullptr;
}

uint32_t Renderer::GetWindowWidth() const
{
	return mWindowWidth;
}

uint32_t Renderer::GetWindowHeight() const
{
	return mWindowHeight;
}