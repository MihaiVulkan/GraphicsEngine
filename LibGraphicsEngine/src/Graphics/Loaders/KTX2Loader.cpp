
/*
	Implementation inspired from the Official Khronos Group KTX software repro:
	https://github.com/KhronosGroup/KTX-Software
*/

#include "Graphics/Loaders/KTX2Loader.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"
#include "glm/common.hpp" //glm::max(), glm::ceil()
#include "KHR/khr_df.h"
#include <vector>
#include <cstdio>
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

#define KTX2_IDENTIFIER { 0xAB, 0x4B, 0x54, 0x58, 0x20, 0x32, 0x30, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A }
#define KTX2_IDENTIFIER_SIZE 12 //bytes
#define KTX2_HEADER_SIZE 80 //bytes

#define CUBEMAP_FACE_COUNT 6

#define BYTE_BIT_COUNT 8

struct KTX2Loader::Impl
{
	typedef struct KTX2_IndexEntry32
	{
		uint32_t byteOffset; /*!< Offset of item from start of file. */
		uint32_t byteLength; /*!< Number of bytes of data in the item. */
	} KTX2_IndexEntry32;

	typedef struct KTX2_IndexEntry64
	{
		uint64_t byteOffset; /*!< Offset of item from start of file. */
		uint64_t byteLength; /*!< Number of bytes of data in the item. */
	} KTX2_IndexEntry64;

	typedef struct KTX2_LevelIndexEntry
	{
		uint64_t byteOffset; /*!< Offset of level from start of file. */
		uint64_t byteLength;
		/*!< Number of bytes of compressed image data in the level. */
		uint64_t uncompressedByteLength;
		/*!< Number of bytes of uncompressed image data in the level. */
	} KTX2_LevelIndexEntry;

	/**
	 * @brief Structure for holding size information for a texture format.
	 */
	typedef struct KTX2_FormatSize
	{
		uint32_t      blockSizeInBits;
		uint32_t      blockWidth;         // in texels
		uint32_t      blockHeight;        // in texels
		uint32_t      blockDepth;         // in texels
	} KTX2_FormatSize;

	typedef struct KTX2_Header
	{
		uint8_t identifier[KTX2_IDENTIFIER_SIZE];
		VkFormat vkFormat; //Khronos decided to use the Vulkan format as it is more general and suits well many Graphics APIs
		uint32_t typeSize;
		uint32_t pixelWidth;
		uint32_t pixelHeight;
		uint32_t pixelDepth;
		uint32_t layerCount;
		uint32_t faceCount;
		uint32_t levelCount;
		uint32_t supercompressionScheme;

		KTX2_IndexEntry32 dataFormatDescriptor;
		KTX2_IndexEntry32 keyValueData;
		KTX2_IndexEntry64 supercompressionGlobalData;
	} KTX2_Header;

	typedef enum KTX2_SuperCompressionScheme
	{
		GE_KTX2_SUPERCOMPRESSION_SCHEME_NONE = 0, //no supercompression
		GE_KTX2_SUPERCOMPRESSION_SCHEME_BASIS_LZ, // Basis LZ supercompression
		GE_KTX2_SUPERCOMPRESSION_SCHEME_ZSTD, // ZStd supercompression
		GE_KTX2_SUPERCOMPRESSION_SCHEME_COUNT
	} KTX2_SuperCompressionScheme;

	Impl();
	virtual ~Impl();

	NO_COPY_NO_MOVE_CLASS(Impl);

	bool_t LoadFromFile(const std::string& filePath);
	bool_t ParseData(std::FILE* pFile);

	KTX2_Texture* mpTexture;
	std::vector<KTX2_LevelIndexEntry> mLevelIndexes;
	KTX2_FormatSize mFormatSize;
};

KTX2Loader::Impl::Impl()
	: mpTexture(nullptr)
{}

KTX2Loader::Impl::~Impl()
{
	GE_FREE(mpTexture);
	mLevelIndexes.clear();
}

bool_t KTX2Loader::Impl::LoadFromFile(const std::string& filePath)
{
	if (filePath.empty() == true)
	{
		LOG_ERROR("Invalid file path!");
		return false;
	}

	std::FILE* pFile = std::fopen(filePath.c_str(), "rb");
	if (pFile == nullptr)
	{
		LOG_ERROR("Invalid file stream!");
		return false;
	}

	int64_t Beg = std::ftell(pFile);
	std::fseek(pFile, 0, SEEK_END);
	int64_t End = std::ftell(pFile);
	std::fseek(pFile, 0, SEEK_SET);

	int64_t fileSize = End - Beg;
	if (fileSize == 0)
	{
		LOG_ERROR("Invalid file size!");
		return false;
	}

	if (ParseData(pFile) == false)
	{
		std::fclose(pFile);
		return false;
	}

	std::fclose(pFile);

	return true;
}

bool_t KTX2Loader::Impl::ParseData(std::FILE* pFile)
{
	if (pFile == nullptr)
	{
		LOG_ERROR("Invalid file stream!");
		return false;
	}

	//header
	KTX2_Header header;
	std::fread(&header, 1, sizeof(header), pFile);

	//check header
	uint8_t identifier[KTX2_IDENTIFIER_SIZE] = KTX2_IDENTIFIER;

	if (::memcmp(header.identifier, identifier, KTX2_IDENTIFIER_SIZE) != 0)
	{
		LOG_ERROR("This is not a KTX2 file!");
		return false;
	}

	if ((header.pixelWidth == 0) || (header.pixelDepth > 0 && (header.pixelHeight == 0)))
	{
		LOG_ERROR("Invalid texture width or height!");
		return false;
	}

	uint32_t textureDim = 0;
	if (header.pixelDepth > 0)
	{
		if (header.layerCount > 0)
		{
			/* No 3D array textures yet. */
			LOG_ERROR("3d array textures are not supported yet!");
			return false;
		}
		textureDim = 3;
	}
	else if (header.pixelHeight > 0)
	{
		textureDim = 2;
	}
	else
	{
		textureDim = 1;
	}

	if (header.faceCount == CUBEMAP_FACE_COUNT)
	{
		if (textureDim != 2)
		{
			/* cube map needs 2D faces */
			LOG_ERROR("Invalid Cubemap texture, it needs 2D faces!");
			return false;
		}
	}
	else if (header.faceCount != 1)
	{
		/* numberOfFaces must be either 1 or 6 */
		LOG_ERROR("NumberOfFaces must be either 1 or 6!");
		return false;
	}

	// Check number of mipmap levels
	bool_t shouldGenerateMipmaps = false;
	if (header.levelCount == 0)
	{
		shouldGenerateMipmaps = true;
		header.levelCount = 1;
	}
	else
	{
		shouldGenerateMipmaps = false;
	}

	// This test works for arrays too because height or depth will be 0.
	uint32_t maxDim = glm::max(glm::max(header.pixelWidth, header.pixelHeight), header.pixelDepth);
	if (maxDim < ((uint32_t)1 << (header.levelCount - 1)))
	{
		// Can't have more mip levels than 1 + log2(max(width, height, depth))
		LOG_ERROR("Can't have more mip levels than 1 + log2(max(width, height, depth)) !");
		return false;
	}

	// header is valid so we now extract the meta info from it
	// and create our internal structure of metadata
	mpTexture = GE_ALLOC(KTX2_Texture);
	assert(mpTexture != nullptr);

	mpTexture->vkFormat = header.vkFormat;
	//TODO - improve isCompressed recognition as this is not correct
	mpTexture->isCompressed = header.supercompressionScheme != KTX2_SuperCompressionScheme::GE_KTX2_SUPERCOMPRESSION_SCHEME_NONE;
	mpTexture->numDimensions = textureDim;
	mpTexture->baseWidth = header.pixelWidth;

	switch (mpTexture->numDimensions)
	{
	case 1:
		mpTexture->baseHeight = mpTexture->baseDepth = 1;
		break;
	case 2:
		mpTexture->baseHeight = header.pixelHeight;
		mpTexture->baseDepth = 1;
		break;
	case 3:
		mpTexture->baseHeight = header.pixelHeight;
		mpTexture->baseDepth = header.pixelDepth;
		break;
	}

	if (header.layerCount > 0)
	{
		mpTexture->numLayers = header.layerCount;
		mpTexture->isArray = true;
	}
	else
	{
		//NOTE! Even if the spec says that layerCOunt must be 0 for non array textures, we cache it as 1 to denote the base image
		mpTexture->numLayers = 1;
		mpTexture->isArray = false;
	}
	mpTexture->numFaces = header.faceCount;
	mpTexture->isCubemap = (header.faceCount == CUBEMAP_FACE_COUNT);

	mpTexture->numLevels = header.levelCount;
	mpTexture->generateMipmaps = shouldGenerateMipmaps;

	// read level index
	size_t levelIndexSize = sizeof(KTX2_LevelIndexEntry) * mpTexture->numLevels;
	mLevelIndexes.resize(mpTexture->numLevels);

	std::fread(mLevelIndexes.data(), sizeof(KTX2_LevelIndexEntry), mpTexture->numLevels, pFile);

	// reset index so taht the base mipmap level is the first in the vector, not the last.
	uint64_t firstLevelFileOffset = mLevelIndexes[mpTexture->numLevels - 1].byteOffset;
	for (size_t level = 0; level < mpTexture->numLevels; ++level)
	{
		mLevelIndexes[level].byteOffset -= firstLevelFileOffset;
	}

	size_t fileOffset = std::ftell(pFile);

	// read some of the DataFormatDescriptor data (only format size, no format type)
	if (header.dataFormatDescriptor.byteLength > 0)
	{
		uint32_t* pDfd = GE_ALLOC_ARRAY(uint32_t, header.dataFormatDescriptor.byteLength);
		assert(pDfd != nullptr);

		std::fread(pDfd, sizeof(uint32_t), header.dataFormatDescriptor.byteLength, pFile);

		uint32_t* pBdb = pDfd + 1;
		assert(pBdb != nullptr);

		// Check the DFD is of the expected type and version.
		if (*pBdb != 0)
		{
			// Either decriptorType or vendorId is not 0
			LOG_ERROR("Either decriptorType or vendorId is not 0!");
			return false;
		}
		if (KHR_DFDVAL(pBdb, VERSIONNUMBER) != KHR_DF_VERSIONNUMBER_1_3)
		{
			LOG_ERROR("DFD version should be 1.3 !");
			return false;
		}

		mFormatSize.blockWidth = KHR_DFDVAL(pBdb, TEXELBLOCKDIMENSION0) + 1;
		mFormatSize.blockHeight = KHR_DFDVAL(pBdb, TEXELBLOCKDIMENSION1) + 1;
		mFormatSize.blockDepth = KHR_DFDVAL(pBdb, TEXELBLOCKDIMENSION2) + 1;
		mFormatSize.blockSizeInBits = KHR_DFDVAL(pBdb, BYTESPLANE0) * BYTE_BIT_COUNT;

		GE_FREE_ARRAY(pDfd);
		pBdb = nullptr;
	}

	//// skip KeyValue Data
	//if (header.keyValueData.byteLength > 0)
	//{
	//	fileOffset += header.keyValueData.byteLength;
	//}

	//// skip Supercompressed Global Data
	//if (header.supercompressionGlobalData.byteLength > 0)
	//{
	//	fileOffset += header.supercompressionGlobalData.byteLength;
	//}

	// OR set directly the base mipmap level offset (in this way we skip all the previous data)
	fileOffset = firstLevelFileOffset;

	std::fseek(pFile, fileOffset, SEEK_SET);
	// calculate size of the image data.Level 0 is the last level in the data.
	mpTexture->dataSize = mLevelIndexes[0].byteOffset + mLevelIndexes[0].byteLength;

	// read mipmap level data
	mpTexture->pData = GE_ALLOC_ARRAY(uint8_t, mpTexture->dataSize);
	assert(mpTexture->pData != nullptr);

	std::fread(mpTexture->pData, 1, mpTexture->dataSize, pFile);

	return true;
}

KTX2Loader::KTX2Loader()
	: mpImpl(GE_ALLOC(Impl))
{}

KTX2Loader::KTX2Loader(const std::string& filePath)
	: KTX2Loader()
{
	assert(mpImpl != nullptr);

	mpImpl->LoadFromFile(filePath);
}

KTX2Loader::~KTX2Loader()
{
	GE_FREE(mpImpl);
}

KTX2Loader::KTX2_Texture* KTX2Loader::GetMetaData()
{
	assert(mpImpl != nullptr);

	return mpImpl->mpTexture;
}

size_t KTX2Loader::ComputeImageOffset(uint32_t level, uint32_t layer, uint32_t face)
{
	assert(mpImpl != nullptr);
	auto* pTexture = mpImpl->mpTexture;
	assert(pTexture != nullptr);

	assert(level < pTexture->numLevels);
	assert(layer < pTexture->numLayers);
	assert(face < pTexture->numFaces);

	if (pTexture->isCompressed)
	{
		LOG_ERROR("No image offset support for compressed textures!");
		return 0;
	}

	// level offset
	size_t imageOffset = mpImpl->mLevelIndexes[level].byteOffset;

	// layer offset
	if (layer > 0)
	{
		imageOffset += layer * ComputeLayerSize(level);
	}

	// face offset - level image size
	if (face > 0)
	{
		imageOffset += face * ComputeImageSize(level);
	}

	return imageOffset;
}

size_t KTX2Loader::ComputeLayerSize(uint32_t level)
{
	assert(mpImpl != nullptr);
	auto* pTexture = mpImpl->mpTexture;
	assert(pTexture != nullptr);

	assert(level < pTexture->numLevels);

	/*
	* As there are no 3D cubemaps, the image's z block count will always be
	* 1 for cubemaps and numFaces will always be 1 for 3D textures so the
	* multiply is safe. 3D cubemaps, if they existed, would require
	* imageSize * (blockCount.z + numFaces);
	*/
	uint32_t blockCountZ = 0;
	size_t imageSize = 0, layerSize = 0;

	blockCountZ = glm::max(1u, (pTexture->baseDepth / mpImpl->mFormatSize.blockDepth) >> level);
	imageSize = ComputeImageSize(level);

	layerSize = imageSize * blockCountZ * pTexture->numFaces;
	
	return layerSize;
}

size_t KTX2Loader::ComputeImageSize(uint32_t level)
{
	//TODO - add support for compressed textures

	assert(mpImpl != nullptr);
	auto* pTexture = mpImpl->mpTexture;
	assert(pTexture != nullptr);

	assert(level < pTexture->numLevels);

	struct blockCount
	{
		uint32_t x, y;
	} blockCount;

	uint32_t blockSizeInBytes = 0;
	uint32_t rowBytes = 0;

	float32_t levelWidth = (float32_t)(mpImpl->mpTexture->baseWidth >> level);
	float32_t levelHeight = (float32_t)(mpImpl->mpTexture->baseHeight >> level);
	// Round up to next whole block. We can't use KTX_PADN because some of
	// the block sizes are not powers of 2.
	blockCount.x
		= (uint32_t)glm::ceil(levelWidth / mpImpl->mFormatSize.blockWidth);
	blockCount.y
		= (uint32_t)glm::ceil(levelHeight / mpImpl->mFormatSize.blockHeight);
	blockCount.x = glm::max(1u, blockCount.x);
	blockCount.y = glm::max(1u, blockCount.y);

	blockSizeInBytes = mpImpl->mFormatSize.blockSizeInBits / BYTE_BIT_COUNT;

	assert(mpImpl->mFormatSize.blockWidth == mpImpl->mFormatSize.blockHeight == mpImpl->mFormatSize.blockDepth == 1);
	rowBytes = blockCount.x * blockSizeInBytes;
		
	size_t size = rowBytes * blockCount.y;

	return size;
}