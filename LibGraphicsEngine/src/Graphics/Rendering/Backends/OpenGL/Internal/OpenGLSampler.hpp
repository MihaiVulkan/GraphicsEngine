#ifndef GRAPHICS_RENDERING_BACKENDS_OPENGL_INTERNAL_OPENGL_SAMPLER_HPP
#define GRAPHICS_RENDERING_BACKENDS_OPENGL_INTERNAL_OPENGL_SAMPLER_HPP

#include "Graphics/Rendering/Backends/OpenGL/Common/OpenGLObject.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		/*
			*Wrapper for GL Texture object*

			Sampler objects contain the subset of texture object state controlling how sampling
			is performed when a shader accesses a texture. Sampler and texture objects may be
			bound together so that the sampler object state is used by shaders when sampling
			the texture, overriding equivalent state in the texture object. Separating texture
			image data from the method of sampling that data allows reuse of the same sampler
			state with many different textures without needing to set the sampler state in each
			texture.

		*/
		class OpenGLSampler : OpenGLObject
		{
			GE_RTTI(GraphicsEngine::Graphics::OpenGLSampler)

		public:
			typedef struct Data
			{
				GLenum wrapU;
				GLenum wrapV;
				GLenum wrapW;
				GLenum minFilter;
				GLenum magFilter;
				uint32_t minLod;
				uint32_t maxLod;
				uint32_t biasLod;
			} Data;

			OpenGLSampler();
			explicit OpenGLSampler(GLenum wrapU, GLenum wrapV, GLenum wrapW, GLenum minFilter, GLenum magFilter,
									uint32_t minLod, uint32_t maxLod, uint32_t biasLod);
			virtual ~OpenGLSampler();

			void Bind(GLuint unit = 0);
			void UnBind();

			const OpenGLSampler::Data& GetData() const;

			const GLuint& GetHandle() const;

		private:
			void Create();
			void Destroy();

			GLuint mHandle;
			OpenGLSampler::Data mData;
		};
	}
}

#endif // GRAPHICS_RENDERING_BACKENDS_OPENGL_INTERNAL_OPENGL_SAMPLER_HPP