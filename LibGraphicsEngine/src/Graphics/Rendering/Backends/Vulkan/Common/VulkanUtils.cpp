#include "Graphics/Rendering/Backends/Vulkan/Common/VulkanUtils.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Internal/VulkanInitializers.hpp"
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

			VkVertexInputRate VertexInputRateToVulkanVertexInputRate(const VertexBuffer::VertexInputRate& vertecInputRate)
			{
				VkVertexInputRate vulkanVertexInputRate = VkVertexInputRate::VK_VERTEX_INPUT_RATE_MAX_ENUM;

				switch (vertecInputRate)
				{
				case VertexBuffer::VertexInputRate::GE_VIR_VERTEX:
					vulkanVertexInputRate = VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;
					break;
				case VertexBuffer::VertexInputRate::GE_VIR_INSTANCE:
					vulkanVertexInputRate = VkVertexInputRate::VK_VERTEX_INPUT_RATE_INSTANCE;
					break;
				case VertexBuffer::VertexInputRate::GE_VIR_COUNT:
				default:
					LOG_ERROR("Invalid Vulkan Vertex Input Rate!");
				}

				return vulkanVertexInputRate;
			}

			VkIndexType IndexTypeToVulkanIndexType(const IndexBuffer::IndexType& indexType)
			{
				VkIndexType vulkanIndexType = VkIndexType::VK_INDEX_TYPE_MAX_ENUM;

				switch (indexType)
				{
				case IndexBuffer::IndexType::GE_IT_UINT32:
					vulkanIndexType = VkIndexType::VK_INDEX_TYPE_UINT32;
					break;
				case IndexBuffer::IndexType::GE_IT_UINT16:
					vulkanIndexType = VkIndexType::VK_INDEX_TYPE_UINT16;
					break;
				case IndexBuffer::IndexType::GE_IT_UINT8:
					vulkanIndexType = VkIndexType::VK_INDEX_TYPE_UINT8_EXT; //TOOD - check for extension support - VkPhysicalDeviceIndexTypeUint8FeaturesEXT 
					break;
				case IndexBuffer::IndexType::GE_IT_COUNT:
				default:
					LOG_ERROR("Invalid Vulkan Index Buffer Index Type!");
				}

				return vulkanIndexType;
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

			VkImageType TextureTypeToVulkanImageType(const Texture::TextureType& type)
			{
				VkImageType vulkanImageType = VkImageType::VK_IMAGE_TYPE_MAX_ENUM;

				switch (type)
				{
				case Texture::TextureType::GE_TT_1D:
				case Texture::TextureType::GE_TT_1D_ARRAY:
					vulkanImageType = VkImageType::VK_IMAGE_TYPE_1D;
					break;
				case Texture::TextureType::GE_TT_2D:
				case Texture::TextureType::GE_TT_2D_ARRAY:
				case Texture::TextureType::GE_TT_CUBEMAP: // a cubemap is formed from 6 faces, each face a 2d map
					vulkanImageType = VkImageType::VK_IMAGE_TYPE_2D;
					break;
				case Texture::TextureType::GE_TT_3D:
				//case Texture::TextureType::GE_TT_3D_ARRAY: //NOTE! No 3d array textures yet
					vulkanImageType = VkImageType::VK_IMAGE_TYPE_3D;
					break;
				case Texture::TextureType::GE_TT_COUNT:
				default:
					LOG_ERROR("Invalid Vulkan Image Type!");
				}

				return vulkanImageType;
			}

			VkImageViewType TextureTypeToVulkanImageViewType(const Texture::TextureType& type)
			{
				VkImageViewType vulkanImageViewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_MAX_ENUM;

				switch (type)
				{
				case Texture::TextureType::GE_TT_1D:
					vulkanImageViewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_1D;
					break;
				case Texture::TextureType::GE_TT_1D_ARRAY:
					vulkanImageViewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_1D_ARRAY;
					break;
				case Texture::TextureType::GE_TT_2D:
					vulkanImageViewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
					break;
				case Texture::TextureType::GE_TT_2D_ARRAY:
					vulkanImageViewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D_ARRAY;
					break;
				case Texture::TextureType::GE_TT_3D:
					vulkanImageViewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_3D;
					break;
				//case Texture::TextureType::GE_TT_3D_ARRAY: //NOTE! No 3d array textures yet
				case Texture::TextureType::GE_TT_CUBEMAP: // a cubemap is formed from 6 faces, each face a 2d map
					vulkanImageViewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE;
					break;
				case Texture::TextureType::GE_TT_CUBEMAP_ARRAY: // a cubemap is formed from 6 faces, each face a 2d map
					vulkanImageViewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
					break;
				case Texture::TextureType::GE_TT_COUNT:
				default:
					LOG_ERROR("Invalid Vulkan Image View Type!");
				}

				return vulkanImageViewType;
			}

			VkFormat TextureFormatToVulkanFormat(const Texture::TextureFormat& format)
			{
				VkFormat vulkanFormat = VkFormat::VK_FORMAT_MAX_ENUM;

				//TODO - add check for format support!
				switch (format)
				{
				case Texture::TextureFormat::GE_TF_R8G8B8_UNORM:
					vulkanFormat = VkFormat::VK_FORMAT_R8G8B8_UNORM;
					break;
				case Texture::TextureFormat::GE_TF_R8G8B8A8_UNORM:
					vulkanFormat = VkFormat::VK_FORMAT_R8G8B8A8_UNORM;
					break;
				case Texture::TextureFormat::GE_TF_D32_S8:
					vulkanFormat = VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT;
					break;
					// TODO - other formats
				case Texture::TextureFormat::GE_TF_COUNT:
				default:
					LOG_ERROR("Invalid Vulkan Format!");
				}

				return vulkanFormat;
			}

			Texture::TextureFormat VulkanFormatToTextureFormat(const VkFormat& format)
			{
				Texture::TextureFormat textureFormat = Texture::TextureFormat::GE_TF_COUNT;

				//TODO - add check for format support!
				switch (format)
				{
				case VkFormat::VK_FORMAT_R8G8B8_UNORM:
					textureFormat = Texture::TextureFormat::GE_TF_R8G8B8_UNORM;
					break;
				case VkFormat::VK_FORMAT_R8G8B8A8_UNORM:
					textureFormat = Texture::TextureFormat::GE_TF_R8G8B8A8_UNORM;
					break;
				case VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT:
					textureFormat = Texture::TextureFormat::GE_TF_D32_S8;
					break;
					// TODO - other formats
				case VkFormat::VK_FORMAT_MAX_ENUM:
				default:
					LOG_ERROR("Invalid Texture Format!");
				}

				return textureFormat;
			}

			VkSamplerAddressMode TextureWrapModeToVulkanWrapMode(const Texture::WrapMode& wrapMode)
			{
				VkSamplerAddressMode vulkanWrapMode = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;

				switch (wrapMode)
				{
				case Texture::WrapMode::GE_WM_REPEAT:
					vulkanWrapMode = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
					break;
				case Texture::WrapMode::GE_WM_MIRROR_REPEAT:
					vulkanWrapMode = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
					break;
				case Texture::WrapMode::GE_WM_CLAMP_TO_EDGE:
					vulkanWrapMode = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
					break;
				case Texture::WrapMode::GE_WM_MIRROR_CLAMP_TO_EDGE:
					vulkanWrapMode = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
					break;
				case Texture::WrapMode::GE_WM_CLAMP_TO_BORDER:
					vulkanWrapMode = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
					break;
				case Texture::WrapMode::GE_WM_COUNT:
				default:
					LOG_ERROR("Invalid Vulkan Wrap Mode!");
				}

				return vulkanWrapMode;
			}

			VkFilter TextureFilterModeToVulkanFilterMode(const Texture::FilterMode& filterMode)
			{
				VkFilter vulkanFilterMode = VkFilter::VK_FILTER_MAX_ENUM;
				switch (filterMode)
				{
				case Texture::FilterMode::GE_FM_NEAREST:
					vulkanFilterMode = VkFilter::VK_FILTER_NEAREST;
					break;
				case Texture::FilterMode::GE_FM_LINEAR:
					vulkanFilterMode = VkFilter::VK_FILTER_LINEAR;
					break;
				case Texture::FilterMode::GE_FM_COUNT:
				default:
					LOG_ERROR("Invalid Vulkan Filter Mode!");
				}

				return vulkanFilterMode;
			}

			VkSamplerMipmapMode TextureMipMapModeToVulkanMipMapMode(const Texture::MipMapMode& mipMapMode)
			{
				VkSamplerMipmapMode vulkanMipMapMode = VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_MAX_ENUM;
				switch (mipMapMode)
				{
				case Texture::MipMapMode::GE_MM_NEAREST:
					vulkanMipMapMode = VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST;
					break;
				case Texture::MipMapMode::GE_MM_LINEAR:
					vulkanMipMapMode = VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_LINEAR;
					break;
				case Texture::MipMapMode::GE_MM_COUNT:
				default:
					LOG_ERROR("Invalid Vulkan MipMap Mode!");
				}

				return vulkanMipMapMode;
			}

			void SetImageLayout(VkCommandBuffer commandBufferHandle, VkImage imageHandle,
				VkImageLayout oldImageLayout, VkImageLayout newImageLayout,
				VkImageSubresourceRange subresourceRange,
				VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask)
			{
				VkAccessFlags srcAccessMask, dstAccessMask;

				// Source layouts (old)
				// Source access mask controls actions that have to be finished on the old layout
				// before it will be transitioned to the new layout
				switch (oldImageLayout)
				{
				case VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED:
					// Image layout is undefined (or does not matter)
					// Only valid as initial layout
					// No flags required, listed only for completeness
					srcAccessMask = 0;
					break;

				case VkImageLayout::VK_IMAGE_LAYOUT_PREINITIALIZED:
					// Image is preinitialized
					// Only valid as initial layout for linear images, preserves memory contents
					// Make sure host writes have been finished
					srcAccessMask = VkAccessFlagBits::VK_ACCESS_HOST_WRITE_BIT;
					break;

				case VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
					// Image is a color attachment
					// Make sure any writes to the color buffer have been finished
					srcAccessMask = VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
					break;

				case VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
					// Image is a depth/stencil attachment
					// Make sure any writes to the depth/stencil buffer have been finished
					srcAccessMask = VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
					break;

				case VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
					// Image is a transfer source
					// Make sure any reads from the image have been finished
					srcAccessMask = VkAccessFlagBits::VK_ACCESS_TRANSFER_READ_BIT;
					break;

				case VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
					// Image is a transfer destination
					// Make sure any writes to the image have been finished
					srcAccessMask = VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT;
					break;

				case VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
					// Image is read by a shader
					// Make sure any shader reads from the image have been finished
					srcAccessMask = VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT;
					break;
				default:
					// Other source layouts aren't handled (yet)
					break;
				}

				// Target layouts (new)
				// Destination access mask controls the dependency for the new image layout
				switch (newImageLayout)
				{
				case VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
					// Image will be used as a transfer destination
					// Make sure any writes to the image have been finished
					dstAccessMask = VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT;
					break;

				case VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
					// Image will be used as a transfer source
					// Make sure any reads from the image have been finished
					dstAccessMask = VkAccessFlagBits::VK_ACCESS_TRANSFER_READ_BIT;
					break;

				case VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
					// Image will be used as a color attachment
					// Make sure any writes to the color buffer have been finished
					dstAccessMask = VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
					break;

				case VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
					// Image layout will be used as a depth/stencil attachment
					// Make sure any writes to depth/stencil buffer have been finished
					dstAccessMask = dstAccessMask | VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
					break;

				case VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
					// Image will be read in a shader (sampler, input attachment)
					// Make sure any writes to the image have been finished
					if (srcAccessMask == 0)
					{
						srcAccessMask = VkAccessFlagBits::VK_ACCESS_HOST_WRITE_BIT | VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT;
					}
					dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
					break;
				default:
					// Other source layouts aren't handled (yet)
					break;
				}

				// Create an image barrier object
				VkImageMemoryBarrier imageMemoryBarrier = VulkanInitializers::ImageMemoryBarrier(srcAccessMask, dstAccessMask, oldImageLayout, newImageLayout, 
										VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, imageHandle, subresourceRange);

				// Insert a memory dependency at the proper pipeline stages that will execute the image layout transition
				vkCmdPipelineBarrier(
					commandBufferHandle,
					srcStageMask,
					dstStageMask,
					0,
					0, nullptr,
					0, nullptr,
					1, &imageMemoryBarrier);
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