#if defined(VULKAN_RENDERER)
#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanMaterial.hpp"
#include "Graphics/Rendering/Backends/Vulkan/VulkanRenderer.hpp"
#include "Graphics/Rendering/Resources/Material.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;



GADRMaterial::GADRMaterial()
	: mpRenderer(nullptr)
	, mpMaterial(nullptr)
{}

GADRMaterial::GADRMaterial(Renderer* pRenderer, Material* pMaterial)
	: mpRenderer(nullptr)
	, mpMaterial(pMaterial)
{
	Create(pRenderer);
}

GADRMaterial::~GADRMaterial()
{
	Destroy();
}

void GADRMaterial::Create(Renderer* pRenderer)
{
	assert(pRenderer != nullptr);
	assert(mpMaterial != nullptr);

	// pRenderer must be a pointer to VulkanRenderer otherwise the cast will fail!
	mpRenderer = dynamic_cast<VulkanRenderer*>(pRenderer);
	assert(mpRenderer != nullptr);

	VulkanDevice* pDevice = mpRenderer->GetDevice();
	assert(pDevice != nullptr);
}

void GADRMaterial::Destroy()
{
	if (mpMaterial)
	{
		mpMaterial = nullptr;
	}
}

void GADRMaterial::Bind(uint32_t currentBufferIdx)
{
	//
}
#endif // VULKAN_RENDERER