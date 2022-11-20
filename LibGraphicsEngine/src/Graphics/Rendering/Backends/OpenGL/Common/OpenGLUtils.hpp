#ifndef GRAPHICS_RENDERING_BACKENDS_OPENGL_COMMON_OPENGL_UTILS_HPP
#define GRAPHICS_RENDERING_BACKENDS_OPENGL_COMMON_OPENGL_UTILS_HPP

#include "Graphics/Rendering/Backends/OpenGL/Common/OpenGLCommon.hpp"

#include "Foundation/TypeDefines.hpp"
#include "Foundation/Platform/GLLoader/GLLoader.hpp"

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
		namespace OpenGLUtils
		{
			// index type
			GLenum IndexTypeToOpenGLIndexType(const IndexBuffer::IndexType& indexType);
			uint32_t IndexTypeToSizeofBytes(const IndexBuffer::IndexType& indexType);

			// geometric primitive
			GLenum PrimitiveTopologyToOpenGLTopolgy(VisualPass::PrimitiveTopology topology);
			GLenum PrimitiveFaceWindingToOpenGLFaceWinding(VisualPass::FaceWinding faceWinding);
			GLenum PrimitivePolygonModeToOpenGLPolygonMode(VisualPass::PolygonMode polygonMode);
			GLenum PrimitivePolygonModeToOpenGLPolygonOffset(VisualPass::PolygonMode polygonMode);

			// pipeline states
			GLenum FaceCullModeToOpenGLFaceCullMode(CullFaceState::CullMode cullMode);

			// texture
			GLenum TextureTypeToOpenGLTextureType(const Texture::TextureType& type);
			GLenum TextureFormatToOpenGLFormat(const Texture::TextureFormat& format);
			GLenum TextureFormatToOpenGLSizedFormat(const Texture::TextureFormat& format);
			GLenum TextureFormatToOpenGLTextureDataType(const Texture::TextureFormat& format);
			GLenum TextureWrapModeToOpenGLWrapMode(const Texture::WrapMode& wrapMode);
			GLenum TextureFilterModeToOpenGLFilterMode(const Texture::FilterMode& filterMode);
			GLenum TextureMipMapModeToOpenGLMipMapMode(const Texture::MipMapMode& mipMapMode);

			// shader
			GLenum ShaderStageToOpenGLShaderType(const Shader::ShaderStage& stage);


			//
			GLenum CompareOpToOpenGLCompareOp(DepthStencilState::CompareOp compareOp);
			GLenum StencilOpToOpenGLStencilOp(DepthStencilState::StencilOp stencilOp);

			GLenum BlendFactorToOpenGLBlendFactor(ColorBlendState::BlendFactor blendFactor);
			GLenum BlendOpToOpenGLBlendOp(ColorBlendState::BlendOp blendOp);
		}
	}
}

#endif // GRAPHICS_RENDERING_BACKENDS_OPENGL_COMMON_OPENGL_UTILS_HPP