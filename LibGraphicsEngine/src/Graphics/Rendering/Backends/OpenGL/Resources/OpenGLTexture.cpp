#if defined(OPENGL_RENDERER)
#include "Graphics/Rendering/Backends/OpenGL/Resources/OpenGLTexture.hpp"
#include "Graphics/Rendering/Backends/OpenGL/Internal/OpenGLTextureObject.hpp"
#include "Graphics/Rendering/Backends/OpenGL/Internal/OpenGLSampler.hpp"
#include "Graphics/Rendering/Backends/OpenGL/Common/OpenGLCommon.hpp"
#include "Graphics/Rendering/Backends/OpenGL/Common/OpenGLUtils.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"
#include <vector>
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

GADRTexture::GADRTexture()
	: mpOpenGLTextureObject(nullptr)
	, mpOpenGLSampler(nullptr)
	, mpTexture(nullptr)
{}

GADRTexture::GADRTexture(Renderer* pRenderer, Texture* pTexture)
	: mpOpenGLTextureObject(nullptr)
	, mpOpenGLSampler(nullptr)
	, mpTexture(pTexture)
{
	Create();
}

GADRTexture::~GADRTexture()
{
	Destroy();
}

void GADRTexture::Create()
{
	assert(mpTexture != nullptr);

	Texture::UsageType textureUsageType = mpTexture->GetUsageType();

	GLenum textureType = OpenGLUtils::TextureTypeToOpenGLTextureType(mpTexture->GetMetaData().type);
	GLenum textureFormat = OpenGLUtils::TextureFormatToOpenGLFormat(mpTexture->GetMetaData().format);
	GLenum textureSizedFormat = OpenGLUtils::TextureFormatToOpenGLSizedFormat(mpTexture->GetMetaData().format);
	GLenum textureDataType = OpenGLUtils::TextureFormatToOpenGLTextureDataType(mpTexture->GetMetaData().format);

	mpOpenGLTextureObject = GE_ALLOC(OpenGLTextureObject)
									(
										textureType,
										textureFormat,
										textureSizedFormat,
										textureDataType,
										mpTexture->GetMetaData().width,
										mpTexture->GetMetaData().height,
										mpTexture->GetMetaData().depth,
										mpTexture->GetMetaData().mipLevels,
										mpTexture->GetMetaData().faceCount,
										mpTexture->GetMetaData().layerCount,
										mpTexture->GetMetaData().mipmaps,
										GL_SAMPLE_COUNT_1, // see GL_MAX_SAMPLES
										mpTexture->GetMetaData().dataSize,
										mpTexture->GetMetaData().mpData
									);
	assert(mpOpenGLTextureObject != nullptr);

	// add a sampler only if we need to sample from the texture in shader
	if (mpTexture->GetSamplingType() == Texture::SamplingType::GE_ST_SAMPLING)
	{
		//sampler
		GLenum filterMode = OpenGLUtils::TextureFilterModeToOpenGLFilterMode(mpTexture->GetMetaData().filterMode);
		GLenum mipmapMode = OpenGLUtils::TextureMipMapModeToOpenGLMipMapMode(mpTexture->GetMetaData().mipMapMode);
		GLenum addressMode = OpenGLUtils::TextureWrapModeToOpenGLWrapMode(mpTexture->GetMetaData().wrapMode);

		mpOpenGLSampler = GE_ALLOC(OpenGLSampler)
			(
				addressMode, addressMode, addressMode,
				//filterMode, filterMode,
				mipmapMode, filterMode,
				0, 10, //TODO
				0 // see GL_MAX_TEXTURE_LOD_BIAS
			);
		assert(mpOpenGLSampler != nullptr);
	}
}

void GADRTexture::Destroy()
{
	GE_FREE(mpOpenGLSampler);
	GE_FREE(mpOpenGLTextureObject);

	if (mpTexture)
	{
		mpTexture = nullptr;
	}
}

void GADRTexture::Bind(GLuint unit)
{
	assert(mpOpenGLTextureObject != nullptr);
	mpOpenGLTextureObject->Bind(unit);
	
	if (mpOpenGLSampler)
	{
		mpOpenGLSampler->Bind(unit);
	}
}

Texture* GADRTexture::GetTexture()
{
	return mpTexture;
}

OpenGLTextureObject* GADRTexture::GetGLTextureObject()
{
	return mpOpenGLTextureObject;
}

OpenGLSampler* GADRTexture::GetGLSampler()
{
	return mpOpenGLSampler;
}
#endif // OPENGL_RENDERER