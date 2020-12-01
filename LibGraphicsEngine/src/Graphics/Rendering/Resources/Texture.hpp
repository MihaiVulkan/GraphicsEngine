#ifndef GRAPHICS_RENDERING_RESOURCES_TEXTURE_HPP
#define GRAPHICS_RENDERING_RESOURCES_TEXTURE_HPP

#include "Resource.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		// Texture - used to store texture data to pass to a specific Graphics API
		class Texture : public Resource
		{
			GE_RTTI(GraphicsEngine::Graphics::Texture)

		public:
			enum class Type : uint8_t
			{
				TEX_1D = 0,
				TEX_1D_ARRAY,
				TEX_2D,
				TEX_2D_ARRAY,
				TEX_3D,
				TEX_3D_ARRAY,
				TEX_CUBEMAP,
				TEX_COUNT
			};

			// Pixel format + pixel type into one enum
			enum class Format : uint8_t
			{
				// 8-bit formats
				TF_R8_UNORM = 0,
				TF_R8_UINT,
				TF_R8_SINT,
				TF_R8G8_UNORM,
				TF_R8G8_UINT,
				TF_R8G8_SINT,
				TF_R8G8B8_UNORM,
				TF_R8G8B8_UINT,
				TF_R8G8B8_SINT,
				TF_R8G8B8A8_UNORM,
				TF_R8G8B8A8_UINT,
				TF_R8G8B8A8_SINT,

				// 16-bit formats
				TF_R16_UNORM,
				TF_R16_UINT,
				TF_R16_SINT,
				TF_R16_SFLOAT,
				TF_R16G16_UNORM,
				TF_R16G16_UINT,
				TF_R16G16_SINT,
				TF_R16G16_SFLOAT,
				TF_R16G16B16_UNORM,
				TF_R16G16B16_UINT,
				TF_R16G16B16_SINT,
				TF_R16G16B16_SFLOAT,
				TF_R16G16B16A16_UNORM,
				TF_R16G16B16A16_UINT,
				TF_R16G16B16A16_SINT,
				TF_R16G16B16A16_SFLOAT,

				// 32-bit formats
				TF_R32_UINT,
				TF_R32_SINT,
				TF_R32_SFLOAT,
				TF_R32G32_UINT,
				TF_R32G32_SINT,
				TF_R32G32_SFLOAT,
				TF_R32G32B32_UINT,
				TF_R32G32B32_SINT,
				TF_R32G32B32_SFLOAT,
				TF_R32G32B32A32_UINT,
				TF_R32G32B32A32_SINT,
				TF_R32G32B32A32_SFLOAT,

				// Depth-stencil formats
				TF_D16,
				TF_D24,
				TF_D32,
				TF_D16_S8,
				TF_D24_S8,
				TF_D32_S8,

				TF_COUNT
			};

			enum class WrapMode : uint8_t
			{
				WM_REPEAT = 0,
				WM_MIRROR_REPEAT,
				WM_CLAMP_TO_EDGE,
				WM_MIRROR_CLAMP_TO_EDGE,
				WM_CLAMP_TO_BORDER,
				WM_COUNT
			};

			enum class FilterMode : uint8_t
			{
				FM_NEAREST = 0,
				FM_LINEAR,
				FM_COUNT
			};

			enum class MipMapMode : uint8_t
			{
				MM_NONE = 0,
				MM_NEAREST,
				MM_LINEAR,
				MM_COUNT
			};

			typedef struct
			{
				Texture::Type type;
				Texture::Format format;
				Texture::WrapMode wrapMode;
				Texture::FilterMode filterMode;
				Texture::MipMapMode mipMapMode;

				uint32_t width, height, depth;
				uint32_t mipLevels;
				uint32_t layerCount;
			} Data;

			virtual ~Texture();

			const Texture::Data& GetData() const;

			bool_t HasMipMaps() const;


		protected:
			// abstract class
			// constructor to be called only by the derived class
			Texture();
			explicit Texture(Texture::Type type, Texture::Format format, Texture::WrapMode wrapMode, Texture::FilterMode filterMode, Texture::MipMapMode mipMapMode,
				uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevels, uint32_t layerCount);


		private:
			Texture::Data mTextureData;
			
			// in case we need to load a texture
			char_t* mpData; //we specify the data as char_t so we now its size (1 byte)
		};

		//////// SPECIALIZATIONS ////////

		class Texture1D : public Texture
		{
			GE_RTTI(GraphicsEngine::Graphics::Texture1D)

		public:
			Texture1D();
			explicit Texture1D(Texture::Type type, Texture::Format format, uint32_t width);

			//TODO - determine what texture formats we support loading from file e.g. ktx, dds, others?
			//bool_t LoadFromFile(const char* pPath);

			//char_t* GetData(uint32_t level) const;

		private:

		};

		class Texture1DArray : public Texture
		{
			GE_RTTI(GraphicsEngine::Graphics::Texture1DArray)

		public:
			Texture1DArray();
			explicit Texture1DArray(Texture::Type type, Texture::Format format, uint32_t width, uint32_t layerCount);

			//TODO - determine what texture formats we support loading from file e.g. ktx, dds, others?
			//bool_t LoadFromFile(const char* pPath);

			//char_t* GetData(uint32_t index, uint32_t level) const;

		private:

		};

		class Texture2D : public Texture
		{
			GE_RTTI(GraphicsEngine::Graphics::Texture2D)

		public:
			Texture2D();
			explicit Texture2D(Texture::Type type, Texture::Format format, Texture::WrapMode wrapMode, Texture::FilterMode filterMode,
				 Texture::MipMapMode mipMapMode, uint32_t width, uint32_t height, uint32_t mipLevels);

			//TODO - determine what texture formats we support loading from file e.g. ktx, dds, others?
			//bool_t LoadFromFile(const char* pPath);

			//char_t* GetData(uint32_t level) const;

		private:

		};

		class Texture2DArray : public Texture
		{
			GE_RTTI(GraphicsEngine::Graphics::Texture2DArray)

		public:
			Texture2DArray();
			explicit Texture2DArray(Texture::Type type, Texture::Format format, Texture::WrapMode wrapMode, Texture::FilterMode filterMode, Texture::MipMapMode mipMapMode,
				uint32_t width, uint32_t height, uint32_t mipLevels);

			//TODO - determine what texture formats we support loading from file e.g. ktx, dds, others?
			//bool_t LoadFromFile(const char* pPath);

			//char_t* GetData(uint32_t index, uint32_t level) const;

		private:

		};

		class Texture3D : public Texture
		{
			GE_RTTI(GraphicsEngine::Graphics::Texture3D)

		public:
			Texture3D();
			explicit Texture3D(Texture::Type type, Texture::Format format, Texture::WrapMode wrapMode, Texture::FilterMode filterMode, Texture::MipMapMode mipMapMode,
				uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevels);

			//TODO - determine what texture formats we support loading from file e.g. ktx, dds, others?
			//bool_t LoadFromFile(const char* pPath);

			//char_t* GetData(uint32_t level) const;

		private:

		};

		class Texture3DArray : public Texture
		{
			GE_RTTI(GraphicsEngine::Graphics::Texture3DArray)

		public:
			Texture3DArray();
			explicit Texture3DArray(Texture::Type type, Texture::Format format, Texture::WrapMode wrapMode, Texture::FilterMode filterMode, Texture::MipMapMode mipMapMode,
				uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevels, uint32_t layerCount);

			//TODO - determine what texture formats we support loading from file e.g. ktx, dds, others?
			//bool_t LoadFromFile(const char* pPath);

			//char_t* GetData(uint32_t index, uint32_t level) const;

		private:

		};

		class TextureCubeMap : public Texture
		{
			GE_RTTI(GraphicsEngine::Graphics::TextureCubeMap)

		public:
			TextureCubeMap();
			// 1 cubemap is composed of 6 2D textures
			explicit TextureCubeMap(Texture::Type type, Texture::Format format, Texture::WrapMode wrapMode, Texture::FilterMode filterMode, Texture::MipMapMode mipMapMode,
				uint32_t width, uint32_t height, uint32_t mipLevels);

			//TODO - determine what texture formats we support loading from file e.g. ktx, dds, others?
			//bool_t LoadFromFile(const char* pPath);

			//char_t* GetData(uint32_t face, uint32_t level) const;

		private:

		};
	}
}

#endif // GRAPHICS_RENDERING_RESOURCES_TEXTURE_HPP