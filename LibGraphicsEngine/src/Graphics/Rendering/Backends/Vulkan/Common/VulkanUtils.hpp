#ifndef GRAPHICS_RENDERING_BACKENDS_VULKAN_COMMON_VULKAN_UTILS_HPP
#define GRAPHICS_RENDERING_BACKENDS_VULKAN_COMMON_VULKAN_UTILS_HPP

#include "Foundation/TypeDefines.hpp"
#include "vulkan/vulkan.h"

#include "Graphics/Rendering/Resources/VertexFormat.hpp"
#include "Graphics/Rendering/Resources/VertexBuffer.hpp"
#include "Graphics/Rendering/Resources/IndexBuffer.hpp"
#include "Graphics/Rendering/Resources/Texture.hpp"
#include "Graphics/Rendering/Resources/Shader.hpp"

#include "Graphics/Rendering/VisualPasses/VisualPass.hpp"
#include "Graphics/Rendering/PipelineStates/CullFaceState.hpp"
#include "Graphics/Rendering/PipelineStates/DepthStencilState.hpp"
#include "Graphics/Rendering/PipelineStates/ColorBlendState.hpp"
#include "Graphics/Rendering/PipelineStates/DynamicState.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		namespace VulkanUtils
		{
			// vertex format
			VkFormat VertexFormatToVulkanVertexFormat(const VertexFormat::VertexAttribute& attribute, uint8_t dimension);

			// vertex input rate
			VkVertexInputRate VertexInputRateToVulkanVertexInputRate(const VertexFormat::VertexInputRate& vertexInputRate);

			// index type
			VkIndexType IndexTypeToVulkanIndexType(const IndexBuffer::IndexType& indexType);

			// geometric primitive
			VkPrimitiveTopology PrimitiveTopologyToVulkanTopolgy(VisualPass::PrimitiveTopology topology);
			VkFrontFace PrimitiveFaceWindingToVulkanFaceWinding(VisualPass::FaceWinding faceWinding);
			VkPolygonMode PrimitivePolygonModeToVulkanPolygonMode(VisualPass::PolygonMode polygonMode);

			// texture
			VkImageType TextureTypeToVulkanImageType(const Texture::TextureType& type);
			VkImageViewType TextureTypeToVulkanImageViewType(const Texture::TextureType& type);
			VkFormat TextureFormatToVulkanFormat(const Texture::TextureFormat& format);
			Texture::TextureFormat VulkanFormatToTextureFormat(const VkFormat& format);
			VkSamplerAddressMode TextureWrapModeToVulkanWrapMode(const Texture::WrapMode& wrapMode);
			VkFilter TextureFilterModeToVulkanFilterMode(const Texture::FilterMode& filterMode);
			VkSamplerMipmapMode TextureMipMapModeToVulkanMipMapMode(const Texture::MipMapMode& mipMapMode);

			// set texture layout
			void SetImageLayout(VkCommandBuffer commandBufferHandle, VkImage imageHandle,
				VkImageLayout oldImageLayout, VkImageLayout newImageLayout,
				VkImageSubresourceRange subresourceRange,
				// NOTE! To account for all possible stages
				VkPipelineStageFlags srcStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
				VkPipelineStageFlags dstStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

			// shader
			VkShaderStageFlagBits ShaderStageToVulkanShaderStage(const Shader::ShaderStage& stage);

			// pipeline states
			VkCullModeFlagBits FaceCullModeToVulkanFaceCullMode(CullFaceState::CullMode cullMode);

			VkCompareOp CompareOpToVulkanCompareOp(DepthStencilState::CompareOp compareOp);
			VkStencilOp StencilOpToVulkanStencilOp(DepthStencilState::StencilOp stencilOp);

			VkBlendFactor BlendFactorToVulkanBlendFactor(ColorBlendState::BlendFactor blendFactor);
			VkBlendOp BlendOpToVulkanBlendOp(ColorBlendState::BlendOp blendOp);

			VkDynamicState DynamicStateToVulkanDynamicState(DynamicState::State state);
		}
	}
}

#endif // GRAPHICS_RENDERING_VBACKENDS_ULKAN_COMMON_VULKAN_UTILS_HPP