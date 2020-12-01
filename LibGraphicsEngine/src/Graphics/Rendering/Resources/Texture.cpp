#include "Texture.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;


Texture::Texture()
	: mTextureData{}
	, mpData(nullptr)
{}

Texture::Texture(Texture::Type type, Texture::Format format, Texture::WrapMode wrapMode, Texture::FilterMode filterMode, Texture::MipMapMode mipMapMode,
				uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevels, uint32_t layerCount)
	: mTextureData{}
	, mpData(nullptr)
{
	mTextureData.type = type;
	mTextureData.format = format;
	mTextureData.wrapMode = wrapMode;
	mTextureData.filterMode = filterMode;
	mTextureData.mipMapMode = mipMapMode;
	mTextureData.width = width;
	mTextureData.height = height;
	mTextureData.depth = depth;
	mTextureData.mipLevels = mipLevels;
	mTextureData.layerCount = layerCount;
}

Texture::~Texture()
{
	mTextureData = {};
}

const Texture::Data& Texture::GetData() const
{
	return mTextureData;
}

bool_t Texture::HasMipMaps() const
{
	return mTextureData.mipLevels > 1;
}

///////////////////

Texture1D::Texture1D()
{}

Texture1D::Texture1D(Texture::Type type, Texture::Format format, uint32_t width)
{
	//TODO
}

Texture1DArray::Texture1DArray()
{}

Texture1DArray::Texture1DArray(Texture::Type type, Texture::Format format, uint32_t width, uint32_t layerCount)
{
	//TODO
}

Texture2D::Texture2D()
{}

Texture2D::Texture2D(Texture::Type type, Texture::Format format, Texture::WrapMode wrapMode, Texture::FilterMode filterMode,
	Texture::MipMapMode mipMapMode, uint32_t width, uint32_t height, uint32_t mipLevels)
{
	//TODO
}

Texture2DArray::Texture2DArray()
{}

Texture2DArray::Texture2DArray(Texture::Type type, Texture::Format format, Texture::WrapMode wrapMode, Texture::FilterMode filterMode, Texture::MipMapMode mipMapMode,
	uint32_t width, uint32_t height, uint32_t mipLevels)
{
	//TODO
}

Texture3D::Texture3D()
{}

Texture3D::Texture3D(Texture::Type type, Texture::Format format, Texture::WrapMode wrapMode, Texture::FilterMode filterMode, Texture::MipMapMode mipMapMode,
	uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevels)
{
	//TODO
}

Texture3DArray::Texture3DArray()
{}

Texture3DArray::Texture3DArray(Texture::Type type, Texture::Format format, Texture::WrapMode wrapMode, Texture::FilterMode filterMode, Texture::MipMapMode mipMapMode,
	uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevels, uint32_t layerCount)
{
	//TODO
}

TextureCubeMap::TextureCubeMap()
{}

TextureCubeMap::TextureCubeMap(Texture::Type type, Texture::Format format, Texture::WrapMode wrapMode, Texture::FilterMode filterMode, Texture::MipMapMode mipMapMode,
	uint32_t width, uint32_t height, uint32_t mipLevels)
{
	//TODO
}