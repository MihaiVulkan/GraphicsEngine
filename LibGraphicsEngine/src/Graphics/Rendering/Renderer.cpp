#include "Renderer.hpp"

#include "Resources/VertexFormat.hpp"
#include "Resources/VertexBuffer.hpp"
#include "Resources/IndexBuffer.hpp"
#include "Resources/UniformBuffer.hpp"
#include "Resources/Texture.hpp"
#include "Resources/RenderTarget.hpp"
#include "Resources/RenderFrameBuffer.hpp"
#include "Resources/Shader.hpp"
#include "Resources/Material.hpp"
#include "Graphics/SceneGraph/GeometryNode.hpp"
#include "Graphics/GeometricPrimitives/GeometricPrimitive.hpp"

// Resources
#if defined(VULKAN_RENDERER)
#include "Vulkan/Resources/VulkanVertexFormat.hpp"
#include "Vulkan/Resources/VulkanVertexBuffer.hpp"
#include "Vulkan/Resources/VulkanIndexBuffer.hpp"
#include "Vulkan/Resources/VulkanUniformBuffer.hpp"
#include "Vulkan/Resources/VulkanTexture.hpp"
#include "Vulkan/Resources/VulkanRenderTarget.hpp"
#include "Vulkan/Resources/VulkanRenderFrameBuffer.hpp"
#include "Vulkan/Resources/VulkanShader.hpp"
#include "Vulkan/Resources/VulkanMaterial.hpp"
#endif // VULKAN_RENDERER

#include "Graphics/Camera/Camera.hpp"
#include "Foundation/Platform/Platform.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

Renderer::Renderer()
	: mType(RendererType::FORWARD)
	, mIsPrepared(false)
	, mWindowWidth(0)
	, mWindowHeight(0)
	, mpRenderQueue(nullptr)
	, mpRenderPass(nullptr)
	, mpCamera(nullptr)
{}

Renderer::Renderer(Renderer::RendererType type)
	: mType(type)
	, mIsPrepared(false)
	, mWindowWidth(0)
	, mWindowHeight(0)
	, mpRenderQueue(nullptr)
	, mpRenderPass(nullptr)
	, mpCamera(nullptr)
{}

Renderer::~Renderer()
{ 
	Terminate();
}

void Renderer::Init(Platform::GE_Window* pWindow)
{
	assert(pWindow != nullptr);
	assert(pWindow->width > 0);
	assert(pWindow->height > 0);

	mWindowWidth = pWindow->width;
	mWindowHeight = pWindow->height;
}


void Renderer::Terminate()
{
	mWindowWidth = 0;
	mWindowHeight = 0;

	mpRenderQueue = nullptr;
	mpRenderPass = nullptr;
	mpCamera = nullptr;

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

	if (mVertexFormatMap.find(pVertexFormat) == mVertexFormatMap.end())
	{
		mVertexFormatMap[pVertexFormat] = GE_ALLOC(GADRVertexFormat)(this, pVertexFormat);
	}

	return mVertexFormatMap[pVertexFormat];
}

void Renderer::UnBind(VertexFormat* pVertexFormat)
{
	assert(pVertexFormat != nullptr);

	VertexFormatMap::iterator iter = mVertexFormatMap.find(pVertexFormat);
	if (iter != mVertexFormatMap.end())
	{
		GE_FREE(iter->second);
		mVertexFormatMap.erase(iter);
	}
}

GADRVertexBuffer* Renderer::Bind(VertexBuffer* pVertexBuffer)
{
	assert(pVertexBuffer != nullptr);

	if (mVertexBufferMap.find(pVertexBuffer) == mVertexBufferMap.end())
	{
		mVertexBufferMap[pVertexBuffer] = GE_ALLOC(GADRVertexBuffer)(this, pVertexBuffer);
	}

	return mVertexBufferMap[pVertexBuffer];
}

void Renderer::UnBind(VertexBuffer* pVertexBuffer)
{
	assert(pVertexBuffer != nullptr);

	VertexBufferMap::iterator iter = mVertexBufferMap.find(pVertexBuffer);
	if (iter != mVertexBufferMap.end())
	{
		GE_FREE(iter->second);
		mVertexBufferMap.erase(iter);
	}
}

GADRIndexBuffer* Renderer::Bind(IndexBuffer* pIndexBuffer)
{
	assert(pIndexBuffer != nullptr);

	if (mIndexBufferMap.find(pIndexBuffer) == mIndexBufferMap.end())
	{
		mIndexBufferMap[pIndexBuffer] = GE_ALLOC(GADRIndexBuffer)(this, pIndexBuffer);
	}

	return mIndexBufferMap[pIndexBuffer];
}

void Renderer::UnBind(IndexBuffer* pIndexBuffer)
{
	assert(pIndexBuffer != nullptr);

	IndexBufferMap::iterator iter = mIndexBufferMap.find(pIndexBuffer);
	if (iter != mIndexBufferMap.end())
	{
		GE_FREE(iter->second);
		mIndexBufferMap.erase(iter);
	}
}

GADRUniformBuffer* Renderer::Bind(UniformBuffer* pUniformBuffer)
{
	assert(pUniformBuffer != nullptr);

	if (mUniformBufferMap.find(pUniformBuffer) == mUniformBufferMap.end())
	{
		mUniformBufferMap[pUniformBuffer] = GE_ALLOC(GADRUniformBuffer)(this, pUniformBuffer);
	}

	return mUniformBufferMap[pUniformBuffer];
}

void Renderer::UnBind(UniformBuffer* pUniformBuffer)
{
	assert(pUniformBuffer != nullptr);

	UniformBufferMap::iterator iter = mUniformBufferMap.find(pUniformBuffer);
	if (iter != mUniformBufferMap.end())
	{
		GE_FREE(iter->second);
		mUniformBufferMap.erase(iter);
	}
}

GADRTexture* Renderer::Bind(Texture* pTexture)
{
	assert(pTexture != nullptr);

	if (mTextureMap.find(pTexture) == mTextureMap.end())
	{
		mTextureMap[pTexture] = GE_ALLOC(GADRTexture)(this, pTexture);
	}

	return mTextureMap[pTexture];
}

void Renderer::UnBind(Texture* pTexture)
{
	assert(pTexture != nullptr);

	TextureMap::iterator iter = mTextureMap.find(pTexture);
	if (iter != mTextureMap.end())
	{
		GE_FREE(iter->second);
		mTextureMap.erase(iter);
	}
}

GADRRenderTarget* Renderer::Bind(RenderTarget* pRenderTarget)
{
	assert(pRenderTarget != nullptr);

	if (mRenderTargetMap.find(pRenderTarget) == mRenderTargetMap.end())
	{
		mRenderTargetMap[pRenderTarget] = GE_ALLOC(GADRRenderTarget)(this, pRenderTarget);
	}

	return mRenderTargetMap[pRenderTarget];
}

void Renderer::UnBind(RenderTarget* pRenderTarget)
{
	assert(pRenderTarget != nullptr);

	RenderTargetMap::iterator iter = mRenderTargetMap.find(pRenderTarget);
	if (iter != mRenderTargetMap.end())
	{
		GE_FREE(iter->second);
		mRenderTargetMap.erase(iter);
	}
}

GADRRenderFrameBuffer* Renderer::Bind(RenderFrameBuffer* pRenderFrameBuffer)
{
	assert(pRenderFrameBuffer != nullptr);

	if (mRenderFrameBufferMap.find(pRenderFrameBuffer) == mRenderFrameBufferMap.end())
	{
		mRenderFrameBufferMap[pRenderFrameBuffer] = GE_ALLOC(GADRRenderFrameBuffer)(this, pRenderFrameBuffer);
	}

	return mRenderFrameBufferMap[pRenderFrameBuffer];
}

void Renderer::UnBind(RenderFrameBuffer* pRenderFrameBuffer)
{
	assert(pRenderFrameBuffer != nullptr);

	RenderFrameBufferMap::iterator iter = mRenderFrameBufferMap.find(pRenderFrameBuffer);
	if (iter != mRenderFrameBufferMap.end())
	{
		GE_FREE(iter->second);
		mRenderFrameBufferMap.erase(iter);
	}
}

GADRShader* Renderer::Bind(Shader* pShader)
{
	assert(pShader != nullptr);

	if (mShaderMap.find(pShader) == mShaderMap.end())
	{
		mShaderMap[pShader] = GE_ALLOC(GADRShader)(this, pShader);
	}

	return mShaderMap[pShader];
}

void Renderer::UnBind(Shader* pShader)
{
	assert(pShader != nullptr);

	ShaderMap::iterator iter = mShaderMap.find(pShader);
	if (iter != mShaderMap.end())
	{
		GE_FREE(iter->second);
		mShaderMap.erase(iter);
	}
}

GADRMaterial* Renderer::Bind(Material* pMaterial, GeometricPrimitive* pGeoPrimitive, uint32_t currentBufferIdx)
{
	assert(pMaterial != nullptr);
	assert(pGeoPrimitive != nullptr);

	if (mMaterialMap.find(pMaterial) == mMaterialMap.end())
	{
		mMaterialMap[pMaterial] = GE_ALLOC(GADRMaterial)(this, pMaterial, pGeoPrimitive);
	}

	OnBindMaterial(mMaterialMap[pMaterial], currentBufferIdx);

	return mMaterialMap[pMaterial];
}

void Renderer::UnBind(Material* pMaterial)
{
	assert(pMaterial != nullptr);

	MaterialMap::iterator iter = mMaterialMap.find(pMaterial);
	if (iter != mMaterialMap.end())
	{
		GE_FREE(iter->second);
		mMaterialMap.erase(iter);
	}
}

void Renderer::OnBindMaterial(GADRMaterial* pGADRMaterial, uint32_t currentBufferIdx)
{
	assert(pGADRMaterial != nullptr);

	// bind GADR material
	pGADRMaterial->Bind(currentBufferIdx);
}