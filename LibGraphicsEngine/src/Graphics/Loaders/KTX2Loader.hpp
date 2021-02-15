#ifndef GRAPHICS_LOADERS_KTX_LOADER_HPP
#define GRAPHICS_LOADERS_KTX_LOADER_HPP

#include "Foundation/Object.hpp"
#include "vulkan/vulkan.h"
#include <string>

namespace GraphicsEngine
{
	namespace Graphics
	{
		/*
			A simple KTX v2.0 loader. Just parses the header and the raw data.
			No support yet for: supercompression, streaming.
			More info: http://github.khronos.org/KTX-Specification/
		*/
		class KTX2Loader : public Object
		{
			GE_RTTI(GraphicsEngine::Graphics::KTX2Loader)

		public:
			typedef struct KTX2_Texture
			{
				VkFormat vkFormat; //Khronos decided to use the Vulkan format as it is more general and suits well many Graphics APIs
				bool_t   isArray;
				bool_t   isCubemap;
				bool_t   isCompressed;
				bool_t   generateMipmaps;
				uint32_t baseWidth;
				uint32_t baseHeight;
				uint32_t baseDepth;
				uint32_t numDimensions;
				uint32_t numLevels;
				uint32_t numLayers;
				uint32_t numFaces;
				size_t   dataSize;
				uint8_t* pData;
			} KTX2_Texture;

			KTX2Loader();
			explicit KTX2Loader(const std::string& filePath);
			virtual ~KTX2Loader();

			KTX2Loader::KTX2_Texture* GetMetaData();

			// Calculate the image offset as part of the KTX2 data, it can be a mipmap level, layer, face 
			size_t ComputeImageOffset(uint32_t level, uint32_t layer, uint32_t face);

			// Calculate the size of an array layer at the specified mip level.
			size_t ComputeLayerSize(uint32_t level);

			// Calculate the image size at the specified mip level
			size_t ComputeImageSize(uint32_t level);

		private:
			NO_COPY_NO_MOVE_CLASS(KTX2Loader)

			struct Impl;
			Impl* mpImpl;
		};
	}
}


#endif // GRAPHICS_LOADERS_KTX_LOADER_HPP