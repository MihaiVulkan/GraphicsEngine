#ifndef GRAPHICS_RENDERING_RESOURCES_TEXTURE_HPP
#define GRAPHICS_RENDERING_RESOURCES_TEXTURE_HPP

#include "Graphics/Rendering/Resources/Resource.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		// Texture - used to store texture data to pass to a specific Graphics API
		class Texture : public Resource
		{
			GE_RTTI(GraphicsEngine::Graphics::Texture)

		public:
			enum class TextureType : uint8_t
			{
				GE_TT_1D = 0,
				GE_TT_1D_ARRAY,
				GE_TT_2D,
				GE_TT_2D_ARRAY,
				GE_TT_3D,
				GE_TT_3D_ARRAY,
				GE_TT_CUBEMAP,
				GE_TT_COUNT
			};

			// Pixel format + pixel type into one enum
			enum class Format : uint8_t
			{
				// 8-bit formats
				GE_TF_R8_UNORM = 0,
				GE_TF_R8_UINT,
				GE_TF_R8_SINT,
				GE_TF_R8G8_UNORM,
				GE_TF_R8G8_UINT,
				GE_TF_R8G8_SINT,
				GE_TF_R8G8B8_UNORM,
				GE_TF_R8G8B8_UINT,
				GE_TF_R8G8B8_SINT,
				GE_TF_R8G8B8A8_UNORM,
				GE_TF_R8G8B8A8_UINT,
				GE_TF_R8G8B8A8_SINT,

				// 16-bit formats
				GE_TF_R16_UNORM,
				GE_TF_R16_UINT,
				GE_TF_R16_SINT,
				GE_TF_R16_SFLOAT,
				GE_TF_R16G16_UNORM,
				GE_TF_R16G16_UINT,
				GE_TF_R16G16_SINT,
				GE_TF_R16G16_SFLOAT,
				GE_TF_R16G16B16_UNORM,
				GE_TF_R16G16B16_UINT,
				GE_TF_R16G16B16_SINT,
				GE_TF_R16G16B16_SFLOAT,
				GE_TF_R16G16B16A16_UNORM,
				GE_TF_R16G16B16A16_UINT,
				GE_TF_R16G16B16A16_SINT,
				GE_TF_R16G16B16A16_SFLOAT,

				// 32-bit formats
				GE_TF_R32_UINT,
				GE_TF_R32_SINT,
				GE_TF_R32_SFLOAT,
				GE_TF_R32G32_UINT,
				GE_TF_R32G32_SINT,
				GE_TF_R32G32_SFLOAT,
				GE_TF_R32G32B32_UINT,
				GE_TF_R32G32B32_SINT,
				GE_TF_R32G32B32_SFLOAT,
				GE_TF_R32G32B32A32_UINT,
				GE_TF_R32G32B32A32_SINT,
				GE_TF_R32G32B32A32_SFLOAT,

				// Depth-stencil formats
				GE_TF_D16,
				GE_TF_D24,
				GE_TF_D32,
				GE_TF_D16_S8,
				GE_TF_D24_S8,
				GE_TF_D32_S8,

				GE_TF_COUNT
			};

			enum class WrapMode : uint8_t
			{
				GE_WM_REPEAT = 0,
				GE_WM_MIRROR_REPEAT,
				GE_WM_CLAMP_TO_EDGE,
				GE_WM_MIRROR_CLAMP_TO_EDGE,
				GE_WM_CLAMP_TO_BORDER,
				GE_WM_COUNT
			};

			enum class FilterMode : uint8_t
			{
				GE_FM_NEAREST = 0,
				GE_FM_LINEAR,
				GE_FM_COUNT
			};

			enum class MipMapMode : uint8_t
			{
				GE_MM_NONE = 0,
				GE_MM_NEAREST,
				GE_MM_LINEAR,
				GE_MM_COUNT
			};

			typedef struct
			{
				Texture::TextureType type;
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
			explicit Texture(Texture::TextureType type, Texture::Format format, Texture::WrapMode wrapMode, Texture::FilterMode filterMode, Texture::MipMapMode mipMapMode,
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
			explicit Texture1D(Texture::TextureType type, Texture::Format format, uint32_t width);

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
			explicit Texture1DArray(Texture::TextureType type, Texture::Format format, uint32_t width, uint32_t layerCount);

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
			explicit Texture2D(Texture::TextureType type, Texture::Format format, Texture::WrapMode wrapMode, Texture::FilterMode filterMode,
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
			explicit Texture2DArray(Texture::TextureType type, Texture::Format format, Texture::WrapMode wrapMode, Texture::FilterMode filterMode, Texture::MipMapMode mipMapMode,
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
			explicit Texture3D(Texture::TextureType type, Texture::Format format, Texture::WrapMode wrapMode, Texture::FilterMode filterMode, Texture::MipMapMode mipMapMode,
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
			explicit Texture3DArray(Texture::TextureType type, Texture::Format format, Texture::WrapMode wrapMode, Texture::FilterMode filterMode, Texture::MipMapMode mipMapMode,
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
			explicit TextureCubeMap(Texture::TextureType type, Texture::Format format, Texture::WrapMode wrapMode, Texture::FilterMode filterMode, Texture::MipMapMode mipMapMode,
				uint32_t width, uint32_t height, uint32_t mipLevels);

			//TODO - determine what texture formats we support loading from file e.g. ktx, dds, others?
			//bool_t LoadFromFile(const char* pPath);

			//char_t* GetData(uint32_t face, uint32_t level) const;

		private:

		};
	}
}

#endif // GRAPHICS_RENDERING_RESOURCES_TEXTURE_HPP