#ifndef GRAPHICS_RENDERING_BACKENDS_OPENGL_INTERNAL_OPENGL_VERTEX_ARRAY_OBJECT_HPP
#define GRAPHICS_RENDERING_BACKENDS_OPENGL_INTERNAL_OPENGL_VERTEX_ARRAY_OBJECT_HPP

#include "Graphics/Rendering/Backends/OpenGL/Common/OpenGLObject.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		/*
			*Wrapper for GL Vertex Array object*

			Vertex array objects represent a collection of sets of vertex attributes. Each set
			is stored as an array in a buffer object data store, with each element of the array
			having a specified format and component count. The attributes of the currently
			bound vertex array object are used as inputs to the vertex shader when executing
			drawing commands.
			Vertex array objects are container objects including references to buffer objects.

			An object representing the state of a set of vertex arrays.
		*/
		class OpenGLVertexArrayObject : public OpenGLObject
		{
			GE_RTTI(GraphicsEngine::Graphics::OpenGLVertexArrayObject)

		public:
			OpenGLVertexArrayObject();
			virtual ~OpenGLVertexArrayObject();

			void Bind();
			void UnBind();

			const GLuint& GetHandle() const;

		private:
			void Create();
			void Destroy();

			GLuint mHandle;
		};
	}
}

#endif // GRAPHICS_RENDERING_BACKENDS_OPENGL_INTERNAL_OPENGL_VERTEX_ARRAY_OBJECT_HPP