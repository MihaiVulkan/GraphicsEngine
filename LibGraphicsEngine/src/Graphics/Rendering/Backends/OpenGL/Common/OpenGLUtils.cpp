#include "Graphics/Rendering/Backends/OpenGL/Common/OpenGLUtils.hpp"
#include "Foundation/Logger.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		namespace OpenGLUtils
		{
			GLenum IndexTypeToOpenGLIndexType(const IndexBuffer::IndexType& indexType)
			{
				GLenum glIndexType = 0;

				switch (indexType)
				{
				case IndexBuffer::IndexType::GE_IT_UINT32:
					glIndexType = GL_UNSIGNED_INT;
					break;
				case IndexBuffer::IndexType::GE_IT_UINT16:
					glIndexType = GL_UNSIGNED_SHORT;
					break;
				case IndexBuffer::IndexType::GE_IT_UINT8:
					glIndexType = GL_UNSIGNED_BYTE;
					break;
				case IndexBuffer::IndexType::GE_IT_COUNT:
				default:
					LOG_ERROR("Invalid OpenGL Index Buffer Index Type!");
				}

				return glIndexType;
			}

			uint32_t IndexTypeToSizeofBytes(const IndexBuffer::IndexType& indexType)
			{
				uint32_t sizeofBytes = 0;

				switch (indexType)
				{
				case IndexBuffer::IndexType::GE_IT_UINT32:
					sizeofBytes = sizeof(uint32_t);
					break;
				case IndexBuffer::IndexType::GE_IT_UINT16:
					sizeofBytes = sizeof(uint16_t);
					break;
				case IndexBuffer::IndexType::GE_IT_UINT8:
					sizeofBytes = sizeof(uint8_t);
					break;
				case IndexBuffer::IndexType::GE_IT_COUNT:
				default:
					LOG_ERROR("Invalid OpenGL Index Buffer Index Type!");
				}

				return sizeofBytes;
			}

			GLenum PrimitiveTopologyToOpenGLTopolgy(VisualPass::PrimitiveTopology topology)
			{
				GLenum glTopology = 0;

				switch (topology)
				{
				case VisualPass::PrimitiveTopology::GE_PT_POINT_LIST:
					glTopology = GL_POINTS;
					break;
				case VisualPass::PrimitiveTopology::GE_PT_LINE_LIST:
					glTopology = GL_LINES;
					break;
				case VisualPass::PrimitiveTopology::GE_PT_LINE_LOOP:
					glTopology = GL_LINE_LOOP;
					break;
				case VisualPass::PrimitiveTopology::GE_PT_LINE_STRIP:
					glTopology = GL_LINE_STRIP;
					break;
				case VisualPass::PrimitiveTopology::GE_PT_TRIANGLE_LIST:
					glTopology = GL_TRIANGLES;
					break;
				case VisualPass::PrimitiveTopology::GE_PT_TRIANGLE_STRIP:
					glTopology = GL_TRIANGLE_STRIP;
					break;
				case VisualPass::PrimitiveTopology::GE_PT_TRIANGLE_FAN:
					glTopology = GL_TRIANGLE_FAN;
					break;
				case VisualPass::PrimitiveTopology::GE_PT_PATCH_LIST:
					glTopology = GL_PATCHES;
					break;

				default:
					LOG_ERROR("Invalid primitive topology!");
				}

				return glTopology;
			}

			GLenum PrimitiveFaceWindingToOpenGLFaceWinding(VisualPass::FaceWinding faceWinding)
			{
				GLenum glFaceWinding = 0;

				switch (faceWinding)
				{
				case VisualPass::FaceWinding::GE_FW_CLOCKWISE:
					glFaceWinding = GL_CW;
					break;
				case VisualPass::FaceWinding::GE_FW_COUNTER_CLOCKWISE:
					glFaceWinding = GL_CCW;
					break;
				default:
					LOG_ERROR("Invalid primitive face winding!");
					break;
				}

				return glFaceWinding;
			}

			GLenum PrimitivePolygonModeToOpenGLPolygonMode(VisualPass::PolygonMode polygonMode)
			{
				GLenum glPolygonMode = 0;

				switch (polygonMode)
				{
				case VisualPass::PolygonMode::GE_PM_FILL:
					glPolygonMode = GL_FILL;
					break;
				case VisualPass::PolygonMode::GE_PM_LINE:
					glPolygonMode = GL_LINE;
					break;
				case VisualPass::PolygonMode::GE_PM_POINT:
					glPolygonMode = GL_POINT;
					break;
				default:
					LOG_ERROR("Invalid primitive polygon mode!");
					break;
				}

				return glPolygonMode;
			}

			GLenum PrimitivePolygonModeToOpenGLPolygonOffset(VisualPass::PolygonMode polygonMode)
			{
				GLenum glPolygonOffset = 0;

				switch (polygonMode)
				{
				case VisualPass::PolygonMode::GE_PM_FILL:
					glPolygonOffset = GL_POLYGON_OFFSET_FILL;
					break;
				case VisualPass::PolygonMode::GE_PM_LINE:
					glPolygonOffset = GL_POLYGON_OFFSET_LINE;
					break;
				case VisualPass::PolygonMode::GE_PM_POINT:
					glPolygonOffset = GL_POLYGON_OFFSET_POINT;
					break;
				default:
					LOG_ERROR("Invalid primitive polygon offset!");
					break;
				}

				return glPolygonOffset;
			}

			GLenum FaceCullModeToOpenGLFaceCullMode(CullFaceState::CullMode cullMode)
			{
				GLenum glCullMode = 0;

				switch (cullMode)
				{
				case CullFaceState::CullMode::GE_CM_NONE:
					glCullMode = GL_NONE;
					break;
				case CullFaceState::CullMode::GE_CM_BACK:
					glCullMode = GL_BACK;
					break;
				case CullFaceState::CullMode::GE_CM_FRONT:
					glCullMode = GL_FRONT;
					break;
				case CullFaceState::CullMode::GE_CM_FRONT_AND_BACK:
					glCullMode = GL_FRONT_AND_BACK; // NOTE! no polygons drawn, but points and lines are!
					break;
				default:
					LOG_ERROR("Invalid face cull mode!");
					break;
				}

				return glCullMode;
			}

			GLenum TextureTypeToOpenGLTextureType(const Texture::TextureType& type)
			{

				GLenum  glTexType = 0;

				switch (type)
				{
				case Texture::TextureType::GE_TT_1D:
					glTexType = GL_TEXTURE_1D;
					break;
				case Texture::TextureType::GE_TT_1D_ARRAY:
					glTexType = GL_TEXTURE_1D_ARRAY;
					break;
				case Texture::TextureType::GE_TT_2D:
					glTexType = GL_TEXTURE_2D;
					break;
				case Texture::TextureType::GE_TT_2D_ARRAY:
					glTexType = GL_TEXTURE_2D_ARRAY;
					break;
				case Texture::TextureType::GE_TT_CUBEMAP: // a cubemap is formed from 6 faces, each face a 2d map
					glTexType = GL_TEXTURE_CUBE_MAP;
					break;
				case Texture::TextureType::GE_TT_CUBEMAP_ARRAY:
					glTexType = GL_TEXTURE_CUBE_MAP_ARRAY;
					break;
				case Texture::TextureType::GE_TT_3D:
					glTexType = GL_TEXTURE_3D;
					break;
				//case Texture::TextureType::GE_TT_3D_ARRAY: //NOTE! No 3d array textures yet
				case Texture::TextureType::GE_TT_COUNT:
				default:
					LOG_ERROR("Invalid OpenGL Image Type!");
				}

				return glTexType;
			}

			GLenum TextureFormatToOpenGLFormat(const Texture::TextureFormat& format)
			{
				GLenum glTexFormat = 0;

				//TODO - add check for format support!
				switch (format)
				{
				case Texture::TextureFormat::GE_TF_R8G8B8_UNORM:
					glTexFormat = GL_RGB;
					break;
				case Texture::TextureFormat::GE_TF_R8G8B8A8_UNORM:
					glTexFormat = GL_RGBA;
					break;
				case Texture::TextureFormat::GE_TF_D32_S8:
					glTexFormat = GL_DEPTH_STENCIL;
					break;
				case Texture::TextureFormat::GE_TF_D32:
					glTexFormat = GL_DEPTH_COMPONENT;
					break;
					// TODO - other formats
				case Texture::TextureFormat::GE_TF_COUNT:
				default:
					LOG_ERROR("Invalid OpenGL Format!");
				}

				return glTexFormat;
			}

			GLenum TextureFormatToOpenGLSizedFormat(const Texture::TextureFormat& format)
			{
				GLenum glTexSizedFormat = 0;

				//TODO - add check for format support!
				switch (format)
				{
				case Texture::TextureFormat::GE_TF_R8G8B8_UNORM:
					glTexSizedFormat = GL_RGB8;
					break;
				case Texture::TextureFormat::GE_TF_R8G8B8A8_UNORM:
					glTexSizedFormat = GL_RGBA8;
					break;
				case Texture::TextureFormat::GE_TF_D32_S8:
					glTexSizedFormat = GL_DEPTH32F_STENCIL8;
					break;
				case Texture::TextureFormat::GE_TF_D32:
					glTexSizedFormat = GL_DEPTH_COMPONENT32F;
					break;
					// TODO - other formats
				case Texture::TextureFormat::GE_TF_COUNT:
				default:
					LOG_ERROR("Invalid OpenGL Format!");
				}

				return glTexSizedFormat;
			}

			GLenum TextureFormatToOpenGLTextureDataType(const Texture::TextureFormat& format)
			{
				GLenum glTexDataType = 0;

				//TODO - add check for format support!
				switch (format)
				{
				case Texture::TextureFormat::GE_TF_R8G8B8_UNORM:
					glTexDataType = GL_UNSIGNED_BYTE;
					break;
				case Texture::TextureFormat::GE_TF_R8G8B8A8_UNORM:
					glTexDataType = GL_UNSIGNED_BYTE;
					break;
				case Texture::TextureFormat::GE_TF_D32_S8:
					glTexDataType = GL_FLOAT;
					break;
				case Texture::TextureFormat::GE_TF_D32:
					glTexDataType = GL_FLOAT;
					break;
					// TODO - other formats
				case Texture::TextureFormat::GE_TF_COUNT:
				default:
					LOG_ERROR("Invalid OpenGL Format!");
				}

				return glTexDataType;
			}

			GLenum TextureWrapModeToOpenGLWrapMode(const Texture::WrapMode& wrapMode)
			{
				GLenum glWrapMode = 0;

				switch (wrapMode)
				{
				case Texture::WrapMode::GE_WM_REPEAT:
					glWrapMode = GL_REPEAT;
					break;
				case Texture::WrapMode::GE_WM_MIRROR_REPEAT:
					glWrapMode = GL_MIRRORED_REPEAT;
					break;
				case Texture::WrapMode::GE_WM_CLAMP_TO_EDGE:
					glWrapMode = GL_CLAMP_TO_EDGE;
					break;
				case Texture::WrapMode::GE_WM_MIRROR_CLAMP_TO_EDGE:
					glWrapMode = GL_MIRROR_CLAMP_TO_EDGE;
					break;
				case Texture::WrapMode::GE_WM_CLAMP_TO_BORDER:
					glWrapMode = GL_CLAMP_TO_BORDER;
					break;
				case Texture::WrapMode::GE_WM_COUNT:
				default:
					LOG_ERROR("Invalid OpenGL Wrap Mode!");
				}

				return glWrapMode;
			}

			GLenum TextureFilterModeToOpenGLFilterMode(const Texture::FilterMode& filterMode)
			{
				GLenum glFilterMode = 0;
				switch (filterMode)
				{
				case Texture::FilterMode::GE_FM_NEAREST:
					glFilterMode = GL_NEAREST;
					break;
				case Texture::FilterMode::GE_FM_LINEAR:
					glFilterMode = GL_LINEAR;
					break;
				case Texture::FilterMode::GE_FM_COUNT:
				default:
					LOG_ERROR("Invalid OpenGL Filter Mode!");
				}

				return glFilterMode;
			}
			GLenum TextureMipMapModeToOpenGLMipMapMode(const Texture::MipMapMode& mipMapMode)
			{
				GLenum glMipMapMode = 0;
				switch (mipMapMode)
				{
				case Texture::MipMapMode::GE_MM_NEAREST:
					glMipMapMode = GL_NEAREST_MIPMAP_NEAREST;
					break;
				case Texture::MipMapMode::GE_MM_LINEAR:
					glMipMapMode = GL_LINEAR_MIPMAP_LINEAR;
					break;
				// NOTE! GL_NEAREST_MIPMAP_LINEAR and GL_LINEAR_MIPMAP_NEAREST are not used currently!
				case Texture::MipMapMode::GE_MM_COUNT:
				default:
					LOG_ERROR("Invalid OpenGL MipMap Mode!");
				}

				return glMipMapMode;
			}

			GLenum ShaderStageToOpenGLShaderType(const Shader::ShaderStage& stage)
			{
				GLenum glShaderType = 0;

				switch (stage)
				{
				case Shader::ShaderStage::GE_SS_VERTEX:
					glShaderType = GL_VERTEX_SHADER;
					break;
				case Shader::ShaderStage::GE_SS_TESSELATION_CONTROL:
					glShaderType = GL_TESS_CONTROL_SHADER;
					break;
				case Shader::ShaderStage::GE_SS_TESSELATION_EVALUATION:
					glShaderType = GL_TESS_EVALUATION_SHADER;
					break;
				case Shader::ShaderStage::GE_SS_GEOMETRY:
					glShaderType = GL_GEOMETRY_SHADER;
					break;
				case Shader::ShaderStage::GE_SS_FRAGMENT:
					glShaderType = GL_FRAGMENT_SHADER;
					break;
				case Shader::ShaderStage::GE_SS_ALL_GRAPHICS:
					LOG_ERROR("All Grpahics shader stage is not supported for the OpenGL API!");
					break;
				case Shader::ShaderStage::GE_SS_COMPUTE:
					glShaderType = GL_COMPUTE_SHADER;
					break;
				case Shader::ShaderStage::GE_SS_COUNT:
				default:
					LOG_ERROR("Invalid OpenGL Shader Stage!");
				}

				return glShaderType;
			}

			GLenum CompareOpToOpenGLCompareOp(DepthStencilState::CompareOp compareOp)
			{
				GLenum glCompareOp = 0;

				switch (compareOp)
				{
				case DepthStencilState::CompareOp::GE_CO_NEVER:
					glCompareOp = GL_NEVER;
					break;
				case DepthStencilState::CompareOp::GE_CO_LESS:
					glCompareOp = GL_LESS;
					break;
				case DepthStencilState::CompareOp::GE_CO_EQUAL:
					glCompareOp = GL_EQUAL;
					break;
				case DepthStencilState::CompareOp::GE_CO_LESS_OR_EQUAL:
					glCompareOp = GL_LEQUAL;
					break;
				case DepthStencilState::CompareOp::GE_CO_GREATER:
					glCompareOp = GL_GREATER;
					break;
				case DepthStencilState::CompareOp::GE_CO_NOT_EQUAL:
					glCompareOp = GL_NOTEQUAL;
					break;
				case DepthStencilState::CompareOp::GE_CO_GREATER_OR_EQUAL:
					glCompareOp = GL_GEQUAL;
					break;
				case DepthStencilState::CompareOp::GE_CO_ALWAYS:
					glCompareOp = GL_ALWAYS;
					break;
				default:
					LOG_ERROR("Invalid depth stencil compare op!");
					break;
				}

				return glCompareOp;
			}

			GLenum StencilOpToOpenGLStencilOp(DepthStencilState::StencilOp stencilOp)
			{
				GLenum glStencilOp = 0;

				switch (stencilOp)
				{
				case DepthStencilState::StencilOp::GE_SO_KEEP:
					glStencilOp = GL_KEEP;
					break;
				case DepthStencilState::StencilOp::GE_SO_ZERO:
					glStencilOp = GL_ZERO;
					break;
				case DepthStencilState::StencilOp::GE_SO_REPLACE:
					glStencilOp = GL_REPLACE;
					break;
				case DepthStencilState::StencilOp::GE_SO_INCREMENT_AND_CLAMP:
					glStencilOp = GL_INCR;
					break;
				case DepthStencilState::StencilOp::GE_SO_DECREMENT_AND_CLAMP:
					glStencilOp = GL_DECR;
					break;
				case DepthStencilState::StencilOp::GE_SO_INVERT:
					glStencilOp = GL_INVERT;
					break;
				case DepthStencilState::StencilOp::GE_SO_INCREMENT_AND_WRAP:
					glStencilOp = GL_INCR_WRAP;
					break;
				case DepthStencilState::StencilOp::GE_SO_DECREMENT_AND_WRAP:
					glStencilOp = GL_DECR_WRAP;
					break;
				default:
					LOG_ERROR("Invalid depth stencil stencil op!");
					break;
				}

				return glStencilOp;
			}

			GLenum BlendFactorToOpenGLBlendFactor(ColorBlendState::BlendFactor blendFactor)
			{
				GLenum glBlendFactor = 0;

				switch (blendFactor)
				{
				case ColorBlendState::BlendFactor::GE_BF_ZERO:
					glBlendFactor = GL_ZERO;
					break;
				case ColorBlendState::BlendFactor::GE_BF_ONE:
					glBlendFactor = GL_ONE;
					break;
				case ColorBlendState::BlendFactor::GE_BF_SRC_COLOR:
					glBlendFactor = GL_SRC_COLOR;
					break;
				case ColorBlendState::BlendFactor::GE_BF_ONE_MINUS_SRC_COLOR:
					glBlendFactor = GL_ONE_MINUS_SRC_COLOR;
					break;
				case ColorBlendState::BlendFactor::GE_BF_DST_COLOR:
					glBlendFactor = GL_DST_COLOR;
					break;
				case ColorBlendState::BlendFactor::GE_BF_ONE_MINUS_DST_COLOR:
					glBlendFactor = GL_ONE_MINUS_DST_COLOR;
					break;
				case ColorBlendState::BlendFactor::GE_BF_SRC_ALPHA:
					glBlendFactor = GL_SRC_ALPHA;
					break;
				case ColorBlendState::BlendFactor::GE_BF_ONE_MINUS_SRC_ALPHA:
					glBlendFactor = GL_ONE_MINUS_SRC_ALPHA;
					break;
				case ColorBlendState::BlendFactor::GE_BF_DST_ALPHA:
					glBlendFactor = GL_DST_ALPHA;
					break;
				case ColorBlendState::BlendFactor::GE_BF_ONE_MINUS_DST_ALPHA:
					glBlendFactor = GL_ONE_MINUS_DST_ALPHA;
					break;
				case ColorBlendState::BlendFactor::GE_BF_CONSTANT_COLOR:
					glBlendFactor = GL_CONSTANT_COLOR;
					break;
				case ColorBlendState::BlendFactor::GE_BF_ONE_MINUS_CONSTANT_COLOR:
					glBlendFactor = GL_ONE_MINUS_CONSTANT_COLOR;
					break;
				case ColorBlendState::BlendFactor::GE_BF_CONSTANT_ALPHA:
					glBlendFactor = GL_CONSTANT_ALPHA;
					break;
				case ColorBlendState::BlendFactor::GE_BF_ONE_MINUS_CONSTANT_ALPHA:
					glBlendFactor = GL_ONE_MINUS_CONSTANT_ALPHA;
					break;
				default:
					LOG_ERROR("Invalid blend factor!");
					break;
				}

				return glBlendFactor;
			}

			GLenum BlendOpToOpenGLBlendOp(ColorBlendState::BlendOp blendOp)
			{
				GLenum glBlendOp = 0;

				switch (blendOp)
				{
				case ColorBlendState::BlendOp::GE_BO_ADD:
					glBlendOp = GL_FUNC_ADD;
					break;
				case ColorBlendState::BlendOp::GE_BO_SUBTRAC:
					glBlendOp = GL_FUNC_SUBTRACT;
					break;
				case ColorBlendState::BlendOp::GE_BO_REVERSE_SUBTRACT:
					glBlendOp = GL_FUNC_REVERSE_SUBTRACT;
					break;
				case ColorBlendState::BlendOp::GE_BO_MIN:
					glBlendOp = GL_MIN;
					break;
				default:
					LOG_ERROR("Invalid blend op!");
					break;
				}

				return glBlendOp;
			}
		}
	}
}