#include "VulkanRenderPassAttachment.hpp"
#include "VulkanInitializers.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VulkanRenderPassAttachment::VulkanRenderPassAttachment()
	: mAttachmentDescription{}
	, mType(Type::COUNT)
{}

VulkanRenderPassAttachment::VulkanRenderPassAttachment( VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp,
														VkAttachmentLoadOp stencilLoadOp, VkAttachmentStoreOp stencilStoreOp, VkImageLayout initialLayout,
														VkImageLayout finalLayout, Type type)

	: mAttachmentDescription{}
	, mType(type)
{
	mAttachmentDescription = VulkanInitializers::AttachmentDescription(format, samples, loadOp, storeOp, stencilLoadOp, stencilStoreOp, initialLayout, finalLayout);
}

VulkanRenderPassAttachment::~VulkanRenderPassAttachment()
{
	mAttachmentDescription = {};
	mType = Type::COUNT;
}

const VkAttachmentDescription& VulkanRenderPassAttachment::GetAttachmentDescription() const
{
	return mAttachmentDescription;
}

const VulkanRenderPassAttachment::Type& VulkanRenderPassAttachment::GetType() const
{
	return mType;
}