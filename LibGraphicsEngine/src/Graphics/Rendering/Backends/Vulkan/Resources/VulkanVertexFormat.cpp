#if defined(VULKAN_RENDERER)
#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanVertexFormat.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Common/VulkanCommon.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Common/VulkanUtils.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanInitializers.hpp"
#include "Foundation/Logger.hpp"
#include <cassert>


using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;


GADRVertexFormat::GADRVertexFormat()
	: mpVertexFormat(nullptr)
	, mInputBinding()
{}

GADRVertexFormat::GADRVertexFormat(Renderer* pRenderer, VertexFormat* pVertexFormat)
	: mpVertexFormat(pVertexFormat)
	, mInputBinding()
{
	Create(pRenderer);
}

GADRVertexFormat::~GADRVertexFormat()
{
	Destroy();
}

void GADRVertexFormat::Create(Renderer* pRenderer)
{
	assert(pRenderer != nullptr);
	assert(mpVertexFormat != nullptr);

	auto& attributes = mpVertexFormat->GetVertexAttributes();

	assert(attributes.empty() == false);

	size_t index = 0;
	for (auto iter = attributes.begin(); iter != attributes.end(); ++ iter)
	{
		VkFormat internalFormat = VulkanUtils::VertexFormatToVulkanVertexFormat(iter->first, iter->second);

		VkVertexInputAttributeDescription attributeDesc{};
		attributeDesc.location = 0; // NOTE! Has to be updated later based on vertex shader input data~
		attributeDesc.binding = VERTEX_BUFFER_BIND_ID; //NOTE! Not reflected in shader code
		attributeDesc.format = internalFormat;
		attributeDesc.offset = mpVertexFormat->GetVertexAttributeOffset(iter->first);

		mInputAttributeMap[iter->first] = attributeDesc;
	}

	//////////////////////////////////////
	VkVertexInputRate vulkanInputRate = VulkanUtils::VertexInputRateToVulkanVertexInputRate(mpVertexFormat->GetVertexInputRate());

	mInputBinding.binding = VERTEX_BUFFER_BIND_ID; //NOTE! Not reflected in shader code
	mInputBinding.stride = mpVertexFormat->GetVertexTotalStride();
	mInputBinding.inputRate = vulkanInputRate;
}

void GADRVertexFormat::Destroy()
{
	if (mpVertexFormat)
	{
		mpVertexFormat = nullptr;
	}

	mInputAttributeMap.clear();
}

const GADRVertexFormat::InputAttributeMap& GADRVertexFormat::GetVkInputAttributes() const
{
	return mInputAttributeMap;
}

const VertexFormat::VertexInputRate& GADRVertexFormat::GetVertexInputRate() const
{
	assert(mpVertexFormat != nullptr);

	return mpVertexFormat->GetVertexInputRate();
}

const VkVertexInputBindingDescription& GADRVertexFormat::GetVkInputBinding() const
{
	return mInputBinding;
}
#endif // VULKAN_RENDERER