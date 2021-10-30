#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanModel.hpp"
#include "Graphics/Rendering/Backends/Vulkan/VulkanRenderer.hpp"
#include "Graphics/Rendering/Resources/Model.hpp"
#include <functional>
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;



GADRModel::GADRModel()
	: mpModel(nullptr)
{}

GADRModel::GADRModel(Renderer* pRenderer, Model* pModel)
	: mpModel(pModel)
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

}

void GADRModel::Destroy()
{
	if (mpModel)
	{
		mpModel = nullptr;
	}
}

void GADRModel::Draw(std::function<void(uint32_t indexCount, uint32_t firstIndex)> onDrawCB)
{
	assert(mpModel != nullptr);

	mpModel->Draw(onDrawCB);
}