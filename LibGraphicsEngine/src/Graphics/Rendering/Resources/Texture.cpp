#include "Graphics/Rendering/Resources/Texture.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Graphics/Rendering/Backends/Vulkan/Common/VulkanUtils.hpp"
#include "Graphics/Loaders/KTX2Loader.hpp"
#include "Foundation/Logger.hpp"
#include "glm/common.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;


Texture::Texture()
	: mTextureMetaData{}
	, mUsageType(UsageType::GE_UT_RENDER) // default to render usage
	, mSamplingType(SamplingType::GE_ST_COUNT)
{}

Texture::Texture(Texture::TextureType type, Texture::TextureFormat format, Texture::WrapMode wrapMode, Texture::FilterMode filterMode, Texture::MipMapMode mipMapMode,
				uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevels, uint32_t layerCount)
	: Texture()
{
	mTextureMetaData.type = type;
	mTextureMetaData.format = format;
	mTextureMetaData.wrapMode = wrapMode;
	mTextureMetaData.filterMode = filterMode;
	mTextureMetaData.mipMapMode = mipMapMode;
	mTextureMetaData.width = width;
	mTextureMetaData.height = height;
	mTextureMetaData.depth = depth;
	mTextureMetaData.mipLevels = mipLevels;
	mTextureMetaData.layerCount = layerCount;
}

Texture::~Texture()
{
	GE_FREE_ARRAY(mTextureMetaData.mpData);
	mTextureMetaData = {};
}

bool_t Texture::LoadFromFile(const std::string& texturePath)
{
	//NOTE! We shall use KTX2 textures
	// Info: http://github.khronos.org/KTX-Specification/

	// KTX2 Cubemap faces are stored in the order: +X, -X, +Y, -Y, +Z, -Z

	//TODO - what if we want several textures at once
	// shpuld I have a ktxloder for each texture or 
	// one loader for all of them
	{
		KTX2Loader ktxLoader(texturePath);

		auto* pKtxTexture = ktxLoader.GetMetaData();
		assert(pKtxTexture != nullptr);


		// check is desired texture type is matched with loaded image data
		switch (mTextureMetaData.type)
		{
		case TextureType::GE_TT_1D:
			assert(pKtxTexture->numDimensions == 1 && pKtxTexture->isArray == false);
			break;
		case TextureType::GE_TT_1D_ARRAY:
			assert(pKtxTexture->numDimensions == 1 && pKtxTexture->isArray);
			break;
		case TextureType::GE_TT_2D:
			assert(pKtxTexture->numDimensions == 2 && pKtxTexture->isArray == false && pKtxTexture->isCubemap == false);
			break;
		case TextureType::GE_TT_2D_ARRAY:
			assert(pKtxTexture->numDimensions == 2 && pKtxTexture->isArray && pKtxTexture->isCubemap == false);
			break;
		case TextureType::GE_TT_3D:
			assert(pKtxTexture->numDimensions == 3 && pKtxTexture->isArray == false && pKtxTexture->isCubemap == false);
			break;
			//case TextureType::GE_TT_3D_ARRAY: //NOTE! No 3d array textures yet
		case TextureType::GE_TT_CUBEMAP:
			assert(pKtxTexture->numDimensions == 2 && pKtxTexture->isArray == false && pKtxTexture->isCubemap);
			break;
		case TextureType::GE_TT_CUBEMAP_ARRAY:
			assert(pKtxTexture->numDimensions == 2 && pKtxTexture->isArray && pKtxTexture->isCubemap);
			break;
		case TextureType::GE_TT_COUNT:
		default:
			LOG_ERROR("Invalid texture type!");
			return false;
		}

		// NOTE! We don't want to depend on utils of a specific graphics api
		// still, the ktx texture format depends on vulkan types
		mTextureMetaData.format = VulkanUtils::VulkanFormatToTextureFormat(pKtxTexture->vkFormat);
		//

		mTextureMetaData.width = pKtxTexture->baseWidth;
		mTextureMetaData.height = pKtxTexture->baseHeight;
		mTextureMetaData.depth = pKtxTexture->baseDepth;
		mTextureMetaData.mipLevels = pKtxTexture->numLevels;
		mTextureMetaData.layerCount = pKtxTexture->numLayers;
		mTextureMetaData.faceCount = pKtxTexture->numFaces;

		mTextureMetaData.dataSize = pKtxTexture->dataSize;
		assert(mTextureMetaData.dataSize > 0);

		mTextureMetaData.mpData = GE_ALLOC_ARRAY(uint8_t, mTextureMetaData.dataSize);
		assert(mTextureMetaData.mpData != nullptr);

		assert(pKtxTexture->pData != nullptr);
		::memcpy(mTextureMetaData.mpData, pKtxTexture->pData, mTextureMetaData.dataSize);

		size_t imageCount = mTextureMetaData.mipLevels * mTextureMetaData.faceCount * mTextureMetaData.layerCount;
		mTextureMetaData.mipmaps.resize(imageCount);

		// NOTE! These loops are specific to the KTX2 image data internal layout
		for (uint32_t layer = 0; layer < mTextureMetaData.layerCount; ++layer)
		{
			for (uint32_t face = 0; face < mTextureMetaData.faceCount; ++face)
			{
				for (uint32_t level = 0; level < mTextureMetaData.mipLevels; ++level)
				{
					uint8_t index = mTextureMetaData.faceCount * layer +
						mTextureMetaData.mipLevels * face + level;
					auto& mipmapRef = mTextureMetaData.mipmaps[index];

					size_t imageOffset = ktxLoader.ComputeImageOffset(level, layer, face);

					mipmapRef.width = glm::max(1u, mTextureMetaData.width >> level); // 'dim >> level' == dim / 2^level
					mipmapRef.height = glm::max(1u, mTextureMetaData.height >> level);
					mipmapRef.offset = imageOffset;
				}
			}
		}

		mTextureMetaData.wrapMode = WrapMode::GE_WM_REPEAT;
		mTextureMetaData.filterMode = FilterMode::GE_FM_LINEAR;
		mTextureMetaData.mipMapMode = MipMapMode::GE_MM_LINEAR;
	}

	// NOTE! by loading a texture we always want to sample from it in shader
	mSamplingType = SamplingType::GE_ST_SAMPLING;

	return true;
}

Texture::UsageType Texture::GetUsageType() const
{
	return mUsageType;
}

void Texture::SetUsageType(Texture::UsageType usageType)
{
	mUsageType = usageType;
}

Texture::SamplingType Texture::GetSamplingType() const
{
	return mSamplingType;
}

void Texture::SetSamplingType(Texture::SamplingType samplingType)
{
	mSamplingType = samplingType;
}

const Texture::MetaData& Texture::GetMetaData() const
{
	return mTextureMetaData;
}

bool_t Texture::HasMipMaps() const
{
	return mTextureMetaData.mipLevels > 1;
}

bool_t Texture::IsColorFormat()
{
	bool_t ret = (mTextureMetaData.format >= TextureFormat::GE_TF_R8_UNORM && mTextureMetaData.format <= TextureFormat::GE_TF_R32G32B32A32_SFLOAT);
	return ret;
}

bool_t Texture::IsDepthFormat()
{
	bool_t ret = (mTextureMetaData.format >= TextureFormat::GE_TF_D16 && mTextureMetaData.format <= TextureFormat::GE_TF_D32_S8);
	return ret;
}

///////////////////

Texture1D::Texture1D()
{}

Texture1D::Texture1D(const std::string& texturePath)
{
	mTextureMetaData.type = Texture::TextureType::GE_TT_1D;

	bool_t res = Texture::LoadFromFile(texturePath);
	assert(res == true);
}

Texture1D::Texture1D(Texture::TextureType type, Texture::TextureFormat format, uint32_t width)
{
	//TODO
}

Texture1D::~Texture1D()
{}

Texture1DArray::Texture1DArray()
{}

Texture1DArray::Texture1DArray(const std::string& texturePath)
{
	mTextureMetaData.type = Texture::TextureType::GE_TT_1D_ARRAY;

	bool_t res = Texture::LoadFromFile(texturePath);
	assert(res == true);
}

Texture1DArray::Texture1DArray(Texture::TextureType type, Texture::TextureFormat format, uint32_t width, uint32_t layerCount)
{
	//TODO
}

Texture1DArray::~Texture1DArray()
{}

Texture2D::Texture2D()
{}

Texture2D::Texture2D(const std::string& texturePath)
{
	mTextureMetaData.type = Texture::TextureType::GE_TT_2D;

	bool_t res = Texture::LoadFromFile(texturePath);
	assert(res == true);
}

Texture2D::Texture2D(Texture::TextureType type, Texture::TextureFormat format, Texture::WrapMode wrapMode, Texture::FilterMode filterMode,
	Texture::MipMapMode mipMapMode, uint32_t width, uint32_t height, uint32_t mipLevels)
	: Texture(type, format, wrapMode, filterMode, mipMapMode, width, height, 1, mipLevels, 1)
{
	mTextureMetaData.faceCount = 1;
}

Texture2D::~Texture2D()
{}

Texture2DArray::Texture2DArray()
{}

Texture2DArray::Texture2DArray(const std::string& texturePath)
{
	mTextureMetaData.type = Texture::TextureType::GE_TT_2D_ARRAY;

	bool_t res = Texture::LoadFromFile(texturePath);
	assert(res == true);
}

Texture2DArray::Texture2DArray(Texture::TextureType type, Texture::TextureFormat format, Texture::WrapMode wrapMode, Texture::FilterMode filterMode, Texture::MipMapMode mipMapMode,
	uint32_t width, uint32_t height, uint32_t mipLevels)
{
	//TODO
}

Texture2DArray::~Texture2DArray()
{}


Texture3D::Texture3D()
{}

Texture3D::Texture3D(const std::string& texturePath)
{
	mTextureMetaData.type = Texture::TextureType::GE_TT_3D;

	bool_t res = Texture::LoadFromFile(texturePath);
	assert(res == true);
}

Texture3D::Texture3D(Texture::TextureType type, Texture::TextureFormat format, Texture::WrapMode wrapMode, Texture::FilterMode filterMode, Texture::MipMapMode mipMapMode,
	uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevels)
{
	//TODO
}

Texture3D::~Texture3D()
{}

//NOTE! Texture3DArray not yet supported bu 3d Graphics APIs

TextureCubeMap::TextureCubeMap()
{}

TextureCubeMap::TextureCubeMap(const std::string& texturePath)
{
	mTextureMetaData.type = Texture::TextureType::GE_TT_CUBEMAP;

	bool_t res = Texture::LoadFromFile(texturePath);
	assert(res == true);
}

TextureCubeMap::TextureCubeMap(Texture::TextureType type, Texture::TextureFormat format, Texture::WrapMode wrapMode, Texture::FilterMode filterMode, Texture::MipMapMode mipMapMode,
	uint32_t width, uint32_t height, uint32_t mipLevels)
{
	//TODO
}

TextureCubeMap::~TextureCubeMap()
{}


TextureCubeMapArray::TextureCubeMapArray()
{}

TextureCubeMapArray::TextureCubeMapArray(const std::string& texturePath)
{
	mTextureMetaData.type = Texture::TextureType::GE_TT_CUBEMAP_ARRAY;

	bool_t res = Texture::LoadFromFile(texturePath);
	assert(res == true);
}

TextureCubeMapArray::TextureCubeMapArray(Texture::TextureType type, Texture::TextureFormat format, Texture::WrapMode wrapMode, Texture::FilterMode filterMode, Texture::MipMapMode mipMapMode,
	uint32_t width, uint32_t height, uint32_t mipLevels)
{
	//TODO
}

TextureCubeMapArray::~TextureCubeMapArray()
{}