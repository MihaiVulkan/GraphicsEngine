#ifndef GRAPHICS_RENDERING_BACKENDS_OPENGL_INTERNAL_OPENGL_BUFFER_HPP
#define GRAPHICS_RENDERING_BACKENDS_OPENGL_INTERNAL_OPENGL_BUFFER_HPP

#include "Graphics/Rendering/Backends/OpenGL/Common/OpenGLObject.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		/*
			*Wrapper for GL Buffer object*

			Buffer objects contain a data store holding a fixed-sized allocation of server memory. 
		*/
		class OpenGLBuffer : public OpenGLObject
		{
			GE_RTTI(GraphicsEngine::Graphics::OpenGLBuffer)

		public:
			OpenGLBuffer();
			//TODO - settle on useful default flags !
			explicit OpenGLBuffer(GLenum type, GLsizeiptr size, void* pData = nullptr, GLbitfield flags = 0);
			virtual ~OpenGLBuffer();

			void Map(GLsizeiptr size);
			void UnMap();

			void SetData(void* pData, GLsizeiptr size);

			void Bind();
			void UnBind();

			void CopyTo(OpenGLBuffer* pDestBuffer);

			GLenum GetType() const;
			const GLuint& GetHandle() const;
			GLsizeiptr GetSize() const;
			void* GetData() const;

		private:
			void Create(void* pData);
			void Destroy();

			GLuint mHandle;
			GLsizeiptr mSize;
			uint8_t* mpMappedData;
			GLenum mType; // aka target
			GLbitfield mAcessFlags;
		};
	}
}

#endif // GRAPHICS_RENDERING_BACKENDS_OPENGL_INTERNAL_OPENGL_BUFFER_HPP