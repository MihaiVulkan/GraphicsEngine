#include "VulkanUtils.hpp"
#include "Foundation/Logger.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		namespace VulkanUtils
		{
			// accepted dimension per attribute
			constexpr const uint8_t FORMAT_TWO_DIM = 2;
			constexpr const uint8_t FORMAT_THREE_DIM = 3;
			constexpr const uint8_t FORMAT_FOUR_DIM = 4;

			VkFormat VertexFormatToVulkanVertexFormat(const VertexFormat::VertexAttribute& attribute, uint8_t dimension)
			{
				VkFormat format = VkFormat::VK_FORMAT_MAX_ENUM;

				//TODO - for now all attributes are of type float 32 !!!
				switch (dimension)
				{
				case FORMAT_TWO_DIM:
					format = VkFormat::VK_FORMAT_R32G32_SFLOAT;
					break;
				case FORMAT_THREE_DIM:
					format = VkFormat::VK_FORMAT_R32G32B32_SFLOAT;
					break;
				case FORMAT_FOUR_DIM:
					format = VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;
					break;
				default:
					LOG_ERROR("Invalid attribute dimension!");
				}

				switch (attribute)
				{
				case VertexFormat::VertexAttribute::GE_VA_POSITION:
				case VertexFormat::VertexAttribute::GE_VA_NORMAL:
				case VertexFormat::VertexAttribute::GE_VA_TANGENT:
				case VertexFormat::VertexAttribute::GE_VA_COLOR:
				case VertexFormat::VertexAttribute::GE_VA_TEXTURE_COORD:
					// format already processed for all!
					break;
				case VertexFormat::VertexAttribute::GE_VA_COUNT:
				default:
					LOG_ERROR("Invalid Vertex Format Attribute!");
				}

				return format;
			}


			VkPrimitiveTopology PrimitiveTopologyToVulkanTopolgy(GeometricPrimitive::PrimitiveTopology topology)
			{
				VkPrimitiveTopology vulkanTopology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;

				switch (topology)
				{
				case GeometricPrimitive::PrimitiveTopology::GE_PT_POINT_LIST:
					vulkanTopology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
					break;
				case GeometricPrimitive::PrimitiveTopology::GE_PT_LINE_LIST:
					vulkanTopology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
					break;
				case GeometricPrimitive::PrimitiveTopology::GE_PT_LINE_LOOP:
					LOG_ERROR("Line loop topology is not supported by Vulkan API!");
					break;
				case GeometricPrimitive::PrimitiveTopology::GE_PT_LINE_STRIP:
					vulkanTopology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
					break;
				case GeometricPrimitive::PrimitiveTopology::GE_PT_TRIANGLE_LIST:
					vulkanTopology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
					break;
				case GeometricPrimitive::PrimitiveTopology::GE_PT_TRIANGLE_STRIP:
					vulkanTopology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
					break;
				case GeometricPrimitive::PrimitiveTopology::GE_PT_TRIANGLE_FAN:
					vulkanTopology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
					break;
				case GeometricPrimitive::PrimitiveTopology::GE_PT_PATCH_LIST:
					vulkanTopology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
					break;

				default:
					LOG_ERROR("Invalid primitive topology!");
					;
				}

				return vulkanTopology;
			}

			VkFrontFace PrimitiveFaceWindingToVulkanFaceWinding(GeometricPrimitive::FaceWinding faceWinding)
			{
				VkFrontFace vulkanFaceWinding = VkFrontFace::VK_FRONT_FACE_MAX_ENUM;

				switch (faceWinding)
				{
				case GeometricPrimitive::FaceWinding::GE_FW_CLOCKWISE:
					vulkanFaceWinding = VkFrontFace::VK_FRONT_FACE_CLOCKWISE;
					break;
				case GeometricPrimitive::FaceWinding::GE_FW_COUNTER_CLOCKWISE:
					vulkanFaceWinding = VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;
					break;
				default:
					LOG_ERROR("Invalid primitive face winding!");
					break;
				}

				return vulkanFaceWinding;
			}

			VkPolygonMode PrimitivePolygonModeToVulkanPolygonMode(GeometricPrimitive::PolygonMode polygonMode)
			{
				VkPolygonMode vulkanPolygonMode = VkPolygonMode::VK_POLYGON_MODE_MAX_ENUM;

				switch (polygonMode)
				{
				case GeometricPrimitive::PolygonMode::GE_PM_FILL:
					vulkanPolygonMode = VkPolygonMode::VK_POLYGON_MODE_FILL;
					break;
				case GeometricPrimitive::PolygonMode::GE_PM_LINE:
					vulkanPolygonMode = VkPolygonMode::VK_POLYGON_MODE_LINE;
					break;
				case GeometricPrimitive::PolygonMode::GE_PM_POINT:
					vulkanPolygonMode = VkPolygonMode::VK_POLYGON_MODE_POINT;
					break;
				default:
					LOG_ERROR("Invalid primitive polygon mode!");
					break;
				}

				return vulkanPolygonMode;
			}

			VkShaderStageFlagBits ShaderStageToVulkanShaderStage(const Shader::ShaderStage& stage)
			{
				VkShaderStageFlagBits vulkanShaderStage = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;

				switch (stage)
				{
				case Shader::ShaderStage::GE_SS_VERTEX:
					vulkanShaderStage = VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
					break;
				case Shader::ShaderStage::GE_SS_TESSELATION_CONTROL:
					vulkanShaderStage = VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
					break;
				case Shader::ShaderStage::GE_SS_TESSELATION_EVALUATION:
					vulkanShaderStage = VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
					break;
				case Shader::ShaderStage::GE_SS_GEOMETRY:
					vulkanShaderStage = VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT;
					break;
				case Shader::ShaderStage::GE_SS_FRAGMENT:
					vulkanShaderStage = VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
					break;
				case Shader::ShaderStage::GE_SS_ALL_GRAPHICS:
					vulkanShaderStage = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL_GRAPHICS;
					break;
				case Shader::ShaderStage::GE_SS_COMPUTE:
					vulkanShaderStage = VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;
					break;
				case Shader::ShaderStage::GE_SS_COUNT:
				default:
					LOG_ERROR("Invalid Vulkan Shader Stage!");
				}

				return vulkanShaderStage;
			}

			VkCullModeFlagBits FaceCullModeToVulkanFaceCullMode(CullFaceState::CullMode cullMode)
			{
				VkCullModeFlagBits vulkanCullMode = VkCullModeFlagBits::VK_CULL_MODE_FLAG_BITS_MAX_ENUM;

				switch (cullMode)
				{
				case CullFaceState::CullMode::GE_CM_NONE:
					vulkanCullMode = VkCullModeFlagBits::VK_CULL_MODE_NONE;
					break;
				case CullFaceState::CullMode::GE_CM_BACK:
					vulkanCullMode = VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
					break;
				case CullFaceState::CullMode::GE_CM_FRONT:
					vulkanCullMode = VkCullModeFlagBits::VK_CULL_MODE_FRONT_BIT;
					break;
				case CullFaceState::CullMode::GE_CM_FRONT_AND_BACK:
					vulkanCullMode = VkCullModeFlagBits::VK_CULL_MODE_FRONT_AND_BACK;
					break;
				default:
					LOG_ERROR("Invalid face cull mode!");
					break;
				}

				return vulkanCullMode;
			}

			VkCompareOp CompareOpToVulkanCompareOp(DepthStencilState::CompareOp compareOp)
			{
				VkCompareOp vulkanCompareOp = VkCompareOp::VK_COMPARE_OP_MAX_ENUM;

				switch (compareOp)
				{
				case DepthStencilState::CompareOp::GE_CO_NEVER:
					vulkanCompareOp = VkCompareOp::VK_COMPARE_OP_NEVER;
					break;
				case DepthStencilState::CompareOp::GE_CO_LESS:
					vulkanCompareOp = VkCompareOp::VK_COMPARE_OP_LESS;
					break;
				case DepthStencilState::CompareOp::GE_CO_EQUAL:
					vulkanCompareOp = VkCompareOp::VK_COMPARE_OP_EQUAL;
					break;
				case DepthStencilState::CompareOp::GE_CO_LESS_OR_EQUAL:
					vulkanCompareOp = VkCompareOp::VK_COMPARE_OP_LESS_OR_EQUAL;
					break;
				case DepthStencilState::CompareOp::GE_CO_GREATER:
					vulkanCompareOp = VkCompareOp::VK_COMPARE_OP_GREATER;
					break;
				case DepthStencilState::CompareOp::GE_CO_NOT_EQUAL:
					vulkanCompareOp = VkCompareOp::VK_COMPARE_OP_NOT_EQUAL;
					break;
				case DepthStencilState::CompareOp::GE_CO_GREATER_OR_EQUAL:
					vulkanCompareOp = VkCompareOp::VK_COMPARE_OP_GREATER_OR_EQUAL;
					break;
				case DepthStencilState::CompareOp::GE_CO_ALWAYS:
					vulkanCompareOp = VkCompareOp::VK_COMPARE_OP_ALWAYS;
					break;
				default:
					LOG_ERROR("Invalid depth stencil compare op!");
					break;
				}

				return vulkanCompareOp;
			}

			VkStencilOp StencilOpToVulkanStencilOp(DepthStencilState::StencilOp stencilOp)
			{
				VkStencilOp vulkanStencilOp = VkStencilOp::VK_STENCIL_OP_MAX_ENUM;

				switch (stencilOp)
				{
				case DepthStencilState::StencilOp::GE_SO_KEEP:
					vulkanStencilOp = VkStencilOp::VK_STENCIL_OP_KEEP;
					break;
				case DepthStencilState::StencilOp::GE_SO_ZERO:
					vulkanStencilOp = VkStencilOp::VK_STENCIL_OP_ZERO;
					break;
				case DepthStencilState::StencilOp::GE_SO_REPLACE:
					vulkanStencilOp = VkStencilOp::VK_STENCIL_OP_REPLACE;
					break;
				case DepthStencilState::StencilOp::GE_SO_INCREMENT_AND_CLAMP:
					vulkanStencilOp = VkStencilOp::VK_STENCIL_OP_INCREMENT_AND_CLAMP;
					break;
				case DepthStencilState::StencilOp::GE_SO_DECREMENT_AND_CLAMP:
					vulkanStencilOp = VkStencilOp::VK_STENCIL_OP_DECREMENT_AND_CLAMP;
					break;
				case DepthStencilState::StencilOp::GE_SO_INVERT:
					vulkanStencilOp = VkStencilOp::VK_STENCIL_OP_INVERT;
					break;
				case DepthStencilState::StencilOp::GE_SO_INCREMENT_AND_WRAP:
					vulkanStencilOp = VkStencilOp::VK_STENCIL_OP_INCREMENT_AND_WRAP;
					break;
				case DepthStencilState::StencilOp::GE_SO_DECREMENT_AND_WRAP:
					vulkanStencilOp = VkStencilOp::VK_STENCIL_OP_DECREMENT_AND_WRAP;
					break;
				default:
					LOG_ERROR("Invalid depth stencil stencil op!");
					break;
				}

				return vulkanStencilOp;
			}

			VkBlendFactor BlendFactorToVulkanBlendFactor(ColorBlendState::BlendFactor blendFactor)
			{
				VkBlendFactor vulkanBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_MAX_ENUM;


				switch (blendFactor)
				{
				case ColorBlendState::BlendFactor::GE_BF_ZERO:
					vulkanBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ZERO;
					break;
				case ColorBlendState::BlendFactor::GE_BF_ONE:
					vulkanBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE;
					break;
				case ColorBlendState::BlendFactor::GE_BF_SRC_COLOR:
					vulkanBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_SRC_COLOR;
					break;
				case ColorBlendState::BlendFactor::GE_BF_ONE_MINUS_SRC_COLOR:
					vulkanBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
					break;
				case ColorBlendState::BlendFactor::GE_BF_DST_COLOR:
					vulkanBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_DST_COLOR;
					break;
				case ColorBlendState::BlendFactor::GE_BF_ONE_MINUS_DST_COLOR:
					vulkanBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
					break;
				case ColorBlendState::BlendFactor::GE_BF_SRC_ALPHA:
					vulkanBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_SRC_ALPHA;
					break;
				case ColorBlendState::BlendFactor::GE_BF_ONE_MINUS_SRC_ALPHA:
					vulkanBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
					break;
				case ColorBlendState::BlendFactor::GE_BF_DST_ALPHA:
					vulkanBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_DST_ALPHA;
					break;
				case ColorBlendState::BlendFactor::GE_BF_ONE_MINUS_DST_ALPHA:
					vulkanBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
					break;
				case ColorBlendState::BlendFactor::GE_BF_CONSTANT_COLOR:
					vulkanBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_CONSTANT_COLOR;
					break;
				case ColorBlendState::BlendFactor::GE_BF_ONE_MINUS_CONSTANT_COLOR:
					vulkanBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
					break;
				case ColorBlendState::BlendFactor::GE_BF_CONSTANT_ALPHA:
					vulkanBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_CONSTANT_ALPHA;
					break;
				case ColorBlendState::BlendFactor::GE_BF_ONE_MINUS_CONSTANT_ALPHA:
					vulkanBlendFactor = VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
					break;
				default:
					LOG_ERROR("Invalid blend factor!");
					break;
				}

				return vulkanBlendFactor;
			}

			VkBlendOp BlendOpToVulkanBlendOp(ColorBlendState::BlendOp blendOp)
			{
				VkBlendOp vulkanBlendOp = VkBlendOp::VK_BLEND_OP_MAX_ENUM;

				switch (blendOp)
				{
				case ColorBlendState::BlendOp::GE_BO_ADD:
					vulkanBlendOp = VkBlendOp::VK_BLEND_OP_ADD;
					break;
				case ColorBlendState::BlendOp::GE_BO_SUBTRAC:
					vulkanBlendOp = VkBlendOp::VK_BLEND_OP_SUBTRACT;
					break;
				case ColorBlendState::BlendOp::GE_BO_REVERSE_SUBTRACT:
					vulkanBlendOp = VkBlendOp::VK_BLEND_OP_REVERSE_SUBTRACT;
					break;
				case ColorBlendState::BlendOp::GE_BO_MIN:
					vulkanBlendOp = VkBlendOp::VK_BLEND_OP_MIN;
					break;
				default:
					LOG_ERROR("Invalid blend op!");
					break;
				}

				return vulkanBlendOp;
			}

			VkDynamicState DynamicStateToVulkanDynamicState(DynamicState::State state)
			{
				VkDynamicState vulkanDynamicState = VkDynamicState::VK_DYNAMIC_STATE_MAX_ENUM;

				switch (state)
				{
				case DynamicState::State::GE_DS_VIEWPORT:
					vulkanDynamicState = VkDynamicState::VK_DYNAMIC_STATE_VIEWPORT;
					break;
				case DynamicState::State::GE_DS_SCISSOR:
					vulkanDynamicState = VkDynamicState::VK_DYNAMIC_STATE_SCISSOR;
					break;
				case DynamicState::State::GE_DS_LINE_WIDTH:
					vulkanDynamicState = VkDynamicState::VK_DYNAMIC_STATE_LINE_WIDTH;
					break;
				case DynamicState::State::GE_DS_DEPTH_BIAS:
					vulkanDynamicState = VkDynamicState::VK_DYNAMIC_STATE_DEPTH_BIAS;
					break;
				case DynamicState::State::GE_DS_BLEND_CONSTANTS:
					vulkanDynamicState = VkDynamicState::VK_DYNAMIC_STATE_BLEND_CONSTANTS;
					break;
				case DynamicState::State::GE_DS_DEPTH_BOUNDS:
					vulkanDynamicState = VkDynamicState::VK_DYNAMIC_STATE_DEPTH_BOUNDS;
					break;
				case DynamicState::State::GE_DS_STENCIL_COMPARE_MASK:
					vulkanDynamicState = VkDynamicState::VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK;
					break;
				case DynamicState::State::GE_DS_STENCIL_WRITE_MASK:
					vulkanDynamicState = VkDynamicState::VK_DYNAMIC_STATE_STENCIL_WRITE_MASK;
					break;
				case DynamicState::State::GE_DS_STENCIL_REFERENCE:
					vulkanDynamicState = VkDynamicState::VK_DYNAMIC_STATE_STENCIL_REFERENCE;
					break;
				default:
					LOG_ERROR("Invalid dynamic state!");
					break;
				}

				return vulkanDynamicState;
			}
		}
	}
}