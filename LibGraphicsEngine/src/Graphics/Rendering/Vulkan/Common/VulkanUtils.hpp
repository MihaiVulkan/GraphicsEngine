#ifndef GRAPHICS_RENDERING_VULKAN_COMMON_VULKAN_UTILS_HPP
#define GRAPHICS_RENDERING_VULKAN_COMMON_VULKAN_UTILS_HPP

#include "Foundation/TypeDefs.hpp"
#include "vulkan/vulkan.h"

#include "Graphics/Rendering/Resources/VertexFormat.hpp"
#include "Graphics/Rendering/Resources/VertexBuffer.hpp"
#include "Graphics/Rendering/Resources/IndexBuffer.hpp"
#include "Graphics/Rendering/Resources/Shader.hpp"

#include "Graphics/GeometricPrimitives/GeometricPrimitive.hpp"
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
			VkVertexInputRate VertexInputRateToVulkanVertexInputRate(const VertexBuffer::VertexInputRate& vertexInputRate);

			// index type
			VkIndexType IndexTypeToVulkanIndexType(const IndexBuffer::IndexType& indexType);

			// geometric primitive
			VkPrimitiveTopology PrimitiveTopologyToVulkanTopolgy(GeometricPrimitive::PrimitiveTopology topology);
			VkFrontFace PrimitiveFaceWindingToVulkanFaceWinding(GeometricPrimitive::FaceWinding faceWinding);
			VkPolygonMode PrimitivePolygonModeToVulkanPolygonMode(GeometricPrimitive::PolygonMode polygonMode);

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

#endif // GRAPHICS_RENDERING_VULKAN_COMMON_VULKAN_UTILS_HPP