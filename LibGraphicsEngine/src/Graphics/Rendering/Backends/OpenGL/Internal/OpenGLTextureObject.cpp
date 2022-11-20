#include "Graphics/Rendering/Backends/OpenGL/Internal/OpenGLTextureObject.hpp"
#include "Foundation/Logger.hpp"
#include <string>
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

OpenGLTextureObject::OpenGLTextureObject()
	: mHandle(NULL)
{}

OpenGLTextureObject::OpenGLTextureObject(GLenum type, GLenum format, GLenum sizedFormat, GLenum dataType, uint32_t width, uint32_t height, uint32_t depth,
										 uint32_t mipLevels, uint32_t faces, uint32_t arrayLayers, const std::vector<Texture::MipMapMetaData>& mipmapsMetadata,
										 uint8_t samples, uint64_t dataSize, uint8_t* pData)
	: mHandle(NULL)
{
	mData.type = type;
	mData.format = format;
	mData.sizedFormat = sizedFormat;
	mData.dataType = dataType;
	mData.extent = { width, height, depth };
	mData.mipLevels = mipLevels;
	mData.faces = faces;
	mData.arrayLayers = arrayLayers;
	mData.samples = samples;

	Create(mipmapsMetadata, dataSize, pData);
}

OpenGLTextureObject::~OpenGLTextureObject()
{
	Destroy();
}

void OpenGLTextureObject::Create(const std::vector<Texture::MipMapMetaData>& mipmapsMetadata, uint64_t dataSize, uint8_t* pData)
{
	// crete texture obj
	glCreateTextures(mData.type, 1, &mHandle);

	// allocate texture memory
	switch (mData.type)
	{
	case GL_TEXTURE_1D:
		glTextureStorage1D(mHandle, mData.mipLevels, mData.sizedFormat, mData.extent.width);
		break;
	case GL_TEXTURE_2D:
	case GL_TEXTURE_1D_ARRAY:
	case GL_TEXTURE_CUBE_MAP:
		glTextureStorage2D(mHandle, mData.mipLevels, mData.sizedFormat, mData.extent.width, mData.arrayLayers * mData.extent.height);
		break;
	case GL_TEXTURE_3D:
	case GL_TEXTURE_2D_ARRAY:
	case GL_TEXTURE_CUBE_MAP_ARRAY:
		glTextureStorage3D(mHandle, mData.mipLevels, mData.sizedFormat, mData.extent.width, mData.extent.height, mData.arrayLayers * mData.extent.depth);
		break;
	}

	if (mData.samples > 1)
	{
		switch (mData.type)
		{
		case GL_TEXTURE_2D_MULTISAMPLE:
			glTextureStorage2DMultisample(mHandle, mData.samples, mData.sizedFormat, mData.extent.width, mData.extent.height, GL_FALSE);
			break;
		case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
			glTextureStorage3DMultisample(mHandle, mData.samples, mData.sizedFormat, mData.extent.width, mData.extent.height, mData.extent.depth, GL_FALSE);
			break;
		}
	}

	// load texture data if available
	if (pData && dataSize > 0)
	{
		assert(mData.mipLevels * mData.faces * mData.arrayLayers == mipmapsMetadata.size());

		for (uint32_t layer = 0; layer < mData.arrayLayers; ++layer)
		{
			for (uint32_t face = 0; face < mData.faces; ++face)
			{
				for (uint32_t level = 0; level < mData.mipLevels; ++level)
				{
					uint32_t index = mData.faces * layer + mData.mipLevels * face + level;

					auto& mipLevel = mipmapsMetadata[index];

					switch (mData.type)
					{
					case GL_TEXTURE_1D:
						glTextureSubImage1D(mHandle, level, 0, mData.extent.width, mData.format, mData.dataType, pData + mipLevel.offset);
						break;
					case GL_TEXTURE_2D:
						glTextureSubImage2D(mHandle, level, 0, 0, mipLevel.width, mipLevel.height, mData.format, mData.dataType, pData + mipLevel.offset);
						break;
					case GL_TEXTURE_1D_ARRAY:
						glTextureSubImage2D(mHandle, level, 0, layer, mData.extent.width, mData.extent.depth, mData.format, mData.dataType, pData + mipLevel.offset);
						break;
					case GL_TEXTURE_2D_ARRAY:
						glTextureSubImage3D(mHandle, level, 0, 0, layer, mipLevel.width, mipLevel.height, mData.extent.depth, mData.format, mData.dataType, pData + mipLevel.offset);
						break;
					case GL_TEXTURE_CUBE_MAP:
						glTextureSubImage3D(mHandle, level, 0, 0, face, mipLevel.width, mipLevel.height, mData.extent.depth, mData.format, mData.dataType, pData + mipLevel.offset);
						break;
					case GL_TEXTURE_3D:
						glTextureSubImage3D(mHandle, level, 0, 0, 0, mData.extent.width, mData.extent.height, mData.extent.depth, mData.format, mData.dataType, pData);
						break;

						//TODO - Multisample !!!
					}
				}
			}
		}
	}

	// mipmaps
	if (mData.mipLevels >= 1)
	{
		// texture specific parameters (not configurable via a sampler object)
	//	glTextureParameterf(mHandle, GL_TEXTURE_BASE_LEVEL, 0);
	//	glTextureParameterf(mHandle, GL_TEXTURE_MAX_LEVEL, mData.mipLevels);

	//	glGenerateTextureMipmap(mHandle);
	}

	glBindTexture(mData.type, 0);

}

void OpenGLTextureObject::Destroy()
{
	if (mHandle)
	{
		glDeleteTextures(1, &mHandle);
		mHandle = NULL;
	}
}

void OpenGLTextureObject::Bind(GLuint unit)
{
	glBindTextureUnit(unit, mHandle);
}

void OpenGLTextureObject::UnBind()
{
	glBindTextureUnit(0, 0);
}

const GLuint& OpenGLTextureObject::GetHandle() const
{
	return mHandle;
}

const OpenGLTextureObject::Data& OpenGLTextureObject::GetData() const
{
	return mData;
}