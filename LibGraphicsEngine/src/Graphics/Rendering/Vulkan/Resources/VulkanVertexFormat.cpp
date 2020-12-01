#include "VulkanVertexFormat.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanCommon.hpp"
#include "Graphics/Rendering/Vulkan/Internal/VulkanInitializers.hpp"
#include "Foundation/Logger.hpp"
#include <cassert>


using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

GADRVertexFormat::GADRVertexFormat()
	: mpVertexFormat(nullptr)
{}

GADRVertexFormat::GADRVertexFormat(Renderer* pRenderer, VertexFormat* pVertexFormat)
	: mpVertexFormat(pVertexFormat)
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

	auto& attributes = mpVertexFormat->GetAttributes();

	assert(attributes.empty() == false);

	mInputAttributes.resize(attributes.size());

	// Inpute attribute bindings describe shader attribute locations and memory layouts
	uint8_t index = 0;
	for (auto it = attributes.begin(); it != attributes.end(); ++it)
	{
		VkFormat internalFormat = GADRVertexFormat::VertexFormatToVulkanVertexFormat(it->first);

		// index is the attribue location
		mInputAttributes[index] =
			VulkanInitializers::VertexInputAttributeDescription(index, VERTEX_BUFFER_BIND_ID, internalFormat, mpVertexFormat->GetAttributeOffset(it->first));

		++index;
	}
}

void GADRVertexFormat::Destroy()
{
	if (mpVertexFormat)
	{
		mpVertexFormat = nullptr;
	}

	mInputAttributes.clear();
}

VkFormat GADRVertexFormat::VertexFormatToVulkanVertexFormat(const VertexFormat::Attribute& att)
{
	VkFormat format = VkFormat::VK_FORMAT_MAX_ENUM;

	switch (att)
	{
	case VertexFormat::Attribute::POSITION:
	case VertexFormat::Attribute::NORMAL:
	case VertexFormat::Attribute::TANGENT:
	case VertexFormat::Attribute::COLOR:
		format = VkFormat::VK_FORMAT_R32G32B32_SFLOAT; // 3 floats
		break;
	case VertexFormat::Attribute::TEXTURE_COORD:
		format = VkFormat::VK_FORMAT_R32G32_SFLOAT; // 2 floats
		break;
	case VertexFormat::Attribute::COUNT:
	default:
		LOG_ERROR("Invalid Vertex Format Attrbute!");
	}

	return format;
}

const std::vector<VkVertexInputAttributeDescription>& GADRVertexFormat::GetVkInputAttributes() const
{
	return mInputAttributes;
}