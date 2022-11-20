#ifndef GRAPHICS_RENDERING_BACKENDS_OPENGL_INTERNAL_OPENGL_TEXTURE_OBJECT_HPP
#define GRAPHICS_RENDERING_BACKENDS_OPENGL_INTERNAL_OPENGL_TEXTURE_OBJECT_HPP

#include "Graphics/Rendering/Backends/OpenGL/Common/OpenGLObject.hpp"
#include "Graphics/Rendering/Resources/Texture.hpp"
#include <vector>

namespace GraphicsEngine
{
	namespace Graphics
	{
		/*
			*Wrapper for GL Texture object*

			Texture objects or textures include a data store containing a collection of texture
			images built from arrays of image elements. The image elements are referred to
			as texels. There are many types of texture objects varying by dimensionality and
			structure;

			Texture objects also include state describing the image parameters of the texture images,
			and state describing how sampling is performed when a shader accesses a texture.
			Shaders may sample a texture at a location indicated by specified texture coordinates, 
			with details of sampling determined by the sampler state of the texture.
			The resulting texture samples are typically used to modify a fragment’s color, in
			order to map an image onto a geometric primitive being drawn, but may be used
			for any purpose in a shader.

			Textures are composed of texels, which will often contain color values.

			When multitexturing, as part of an overall multiple pass application of texture images,
			controls one processing step for a single texture image. A texture unit maintains the texturing state
			for one texturing pass, including the texture image, filter, environment, coordinate generation, and
			matrix stack. Multitexturing consists of a chain of texture units.
		*/
		class OpenGLTextureObject : OpenGLObject
		{
			GE_RTTI(GraphicsEngine::Graphics::OpenGLTextureObject)

		public:
			typedef struct Data
			{
				typedef struct Extent
				{
					uint32_t width;
					uint32_t height;
					uint32_t depth;
				} Extent;

				GLenum type;
				GLenum format;
				GLenum sizedFormat;
				GLenum dataType;
				Extent extent;
				uint32_t mipLevels;
				uint32_t faces;
				uint32_t arrayLayers;
				uint8_t samples;
			} Data;

			OpenGLTextureObject();
			explicit OpenGLTextureObject(GLenum type, GLenum format, GLenum sizedFormat, GLenum dataType, uint32_t width, uint32_t height, uint32_t depth,
										 uint32_t mipLevels, uint32_t faces, uint32_t arrayLayers, const std::vector<Texture::MipMapMetaData>& mipmapsMetadata,
										 uint8_t samples, uint64_t dataSize = 0, uint8_t* pData = nullptr);
			virtual ~OpenGLTextureObject();

			void Bind(GLuint unit = 0);
			void UnBind();

			const OpenGLTextureObject::Data& GetData() const;

			const GLuint& GetHandle() const;

		private:
			void Create(const std::vector<Texture::MipMapMetaData>& mipmapsMetadata, uint64_t dataSize, uint8_t* pData);
			void Destroy();

			GLuint mHandle;
			OpenGLTextureObject::Data mData;
		};
	}
}

#endif // GRAPHICS_RENDERING_BACKENDS_OPENGL_INTERNAL_OPENGL_TEXTURE_OBJECT_HPP