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
#include "Graphics/Rendering/Resources/Model.hpp"
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
#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanModel.hpp"
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

Renderer::Renderer(Platform::Window* pWindow, Renderer::RendererType type)
	: mRendererType(type)
	, mIsPrepared(false)
	, mWindowWidth(0)
	, mWindowHeight(0)
	, mpRenderQueue(nullptr)
	, mpRenderPass(nullptr)
{
	Init(pWindow);
}

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
		//NOTE! We have to clear the uniform buffer in the end too!
		auto* pFirst = it->first;
		GE_FREE(pFirst);

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

	for (auto it = mModelMap.begin(); it != mModelMap.end(); ++it)
	{
		GE_FREE(it->second);
	}
	mModelMap.clear();
}

void Renderer::CleanUpGAIR()
{
	for (auto it = mVertexFormatMap.begin(); it != mVertexFormatMap.end(); ++it)
	{
		auto* pFirst = it->first;
		GE_FREE(pFirst);
	}

	for (auto it = mVertexBufferMap.begin(); it != mVertexBufferMap.end(); ++it)
	{
		auto* pFirst = it->first;
		GE_FREE(pFirst);
	}

	for (auto it = mIndexBufferMap.begin(); it != mIndexBufferMap.end(); ++it)
	{
		auto* pFirst = it->first;
		GE_FREE(pFirst);
	}

	// NOTE! We do not clear the uniform buffer as it is needed per frame!
	/*for (auto it = mUniformBufferMap.begin(); it != mUniformBufferMap.end(); ++it)
	{
		auto* pFirst = it->first;
		GE_FREE(pFirst);
	}*/

	for (auto it = mTextureMap.begin(); it != mTextureMap.end(); ++it)
	{
		auto* pFirst = it->first;
		GE_FREE(pFirst);
	}

	for (auto it = mShaderMap.begin(); it != mShaderMap.end(); ++it)
	{
		auto* pFirst = it->first;
		GE_FREE(pFirst);
	}

	for (auto it = mMaterialMap.begin(); it != mMaterialMap.end(); ++it)
	{
		auto* pFirst = it->first;
		GE_FREE(pFirst);
	}
}

GADRVertexFormat* Renderer::Bind(VertexFormat* pVertexFormat)
{
	assert(pVertexFormat != nullptr);

	auto* pGADR = Get(pVertexFormat);
	
	if (pGADR)
		pGADR->OnBind();

	return pGADR;
}

void Renderer::UnBind(VertexFormat* pVertexFormat)
{
	assert(pVertexFormat != nullptr);

	auto* pGADR = Get(pVertexFormat);

	if (pGADR)
		pGADR->OnUnBind();
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

	auto* pGADR = Get(pVertexBuffer);
	
	if (pGADR)
		pGADR->OnBind(currentBufferIdx);

	return pGADR;
}

void Renderer::UnBind(VertexBuffer* pVertexBuffer)
{
	assert(pVertexBuffer != nullptr);

	auto* pGADR = Get(pVertexBuffer);

	if (pGADR)
		pGADR->OnUnBind();
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

	auto* pGADR = Get(pIndexBuffer);

	if (pGADR)
		pGADR->OnBind(currentBufferIdx);

	return pGADR;
}

void Renderer::UnBind(IndexBuffer* pIndexBuffer)
{
	assert(pIndexBuffer != nullptr);

	auto* pGADR = Get(pIndexBuffer);

	if (pGADR)
		pGADR->OnUnBind();
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

	auto* pGADR = Get(pUniformBuffer);

	if (pGADR)
		pGADR->OnBind();

	return pGADR;
}

void Renderer::UnBind(UniformBuffer* pUniformBuffer)
{
	assert(pUniformBuffer != nullptr);

	auto* pGADR = Get(pUniformBuffer);

	if (pGADR)
		pGADR->OnUnBind();
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

	auto* pGADR = Get(pTexture);

	if (pGADR)
		pGADR->OnBind();

	return pGADR;
}

void Renderer::UnBind(Texture* pTexture)
{
	assert(pTexture != nullptr);

	auto* pGADR = Get(pTexture);

	if (pGADR)
		pGADR->OnUnBind();
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

	auto* pGADR = Get(pRenderTarget);

	if (pGADR)
		pGADR->OnBind();

	return pGADR;
}

void Renderer::UnBind(RenderTarget* pRenderTarget)
{
	assert(pRenderTarget != nullptr);

	auto* pGADR = Get(pRenderTarget);

	if (pGADR)
		pGADR->OnUnBind();
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

	auto* pGADR = Get(pRenderFrameBuffer);

	if (pGADR)
		pGADR->OnBind();

	return pGADR;
}

void Renderer::UnBind(RenderFrameBuffer* pRenderFrameBuffer)
{
	assert(pRenderFrameBuffer != nullptr);

	auto* pGADR = Get(pRenderFrameBuffer);

	if (pGADR)
		pGADR->OnUnBind();
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

	auto* pGADR = Get(pShader);

	if (pGADR)
		pGADR->OnBind();

	return pGADR;
}

void Renderer::UnBind(Shader* pShader)
{
	assert(pShader != nullptr);

	auto* pGADR = Get(pShader);

	if (pGADR)
		pGADR->OnUnBind();
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

	auto* pGADR = Get(pMaterial);

	if (pGADR)
		pGADR->OnBind();

	return pGADR;
}

void Renderer::UnBind(Material* pMaterial)
{
	assert(pMaterial != nullptr);

	auto* pGADR = Get(pMaterial);

	if (pGADR)
		pGADR->OnUnBind();
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

GADRModel* Renderer::Bind(Model* pModel)
{
	assert(pModel != nullptr);

	auto* pGADR = Get(pModel);

	if (pGADR)
		pGADR->OnBind();

	return pGADR;
}

void Renderer::UnBind(Model* pModel)
{
	assert(pModel != nullptr);

	auto* pGADR = Get(pModel);

	if (pGADR)
		pGADR->OnUnBind();
}

GADRModel* Renderer::Get(Model* pModel)
{
	assert(pModel != nullptr);

	auto iter = mModelMap.find(pModel);
	if (iter != mModelMap.end())
	{
		return iter->second;
	}
	else
	{
		auto ref = mModelMap[pModel] = GE_ALLOC(GADRModel)(this, pModel);
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