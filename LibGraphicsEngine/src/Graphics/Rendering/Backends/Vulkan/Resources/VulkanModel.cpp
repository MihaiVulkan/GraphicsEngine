#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanModel.hpp"
#include "Graphics/Rendering/Backends/Vulkan/VulkanRenderer.hpp"
#include "Graphics/Rendering/Resources/Model.hpp"
#include <functional>
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;



GADRModel::GADRModel()
	: mpRenderer(nullptr)
	, mpModel(nullptr)
{}

GADRModel::GADRModel(Renderer* pRenderer, Model* pModel)
	: mpRenderer(nullptr)
	, mpModel(pModel)
{
	Create(pRenderer);
}

GADRModel::~GADRModel()
{
	Destroy();
}

void GADRModel::Create(Renderer* pRenderer)
{
	assert(pRenderer != nullptr);
	assert(mpModel != nullptr);

	// pRenderer must be a pointer to VulkanRenderer otherwise the cast will fail!
	mpRenderer = dynamic_cast<VulkanRenderer*>(pRenderer);
	assert(mpRenderer != nullptr);

	VulkanDevice* pDevice = mpRenderer->GetDevice();
	assert(pDevice != nullptr);
}

void GADRModel::Destroy()
{
	if (mpModel)
	{
		mpModel = nullptr;
	}
}

void GADRModel::Draw(uint32_t currentBufferIdx)
{
	assert(mpModel != nullptr);

	mpModel->Draw(
		[this, &currentBufferIdx](uint32_t indexCount, uint32_t firstIndex)
		{
			if (mpRenderer)
				mpRenderer->DrawDirect(indexCount, firstIndex, 1, currentBufferIdx, true);
		});
}