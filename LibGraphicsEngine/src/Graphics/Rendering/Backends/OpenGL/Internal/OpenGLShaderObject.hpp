#ifndef GRAPHICS_RENDERING_BACKENDS_OPENGL_INTERNAL_OPENGL_VERTEX_SHADER_OBJECT_HPP
#define GRAPHICS_RENDERING_BACKENDS_OPENGL_INTERNAL_OPENGL_VERTEX_SHADER_OBJECT_HPP

#include "Graphics/Rendering/Backends/OpenGL/Common/OpenGLObject.hpp"
#include <string>

namespace GraphicsEngine
{
	namespace Graphics
	{
		/*
			*Wrapper for GL Shader object*

			The source and/or binary code representing part or all of a shader program that is
			executed by one of the programmable stages defined by the GL (such as a vertex
			or fragment shader) is encapsulated in one or more shader objects.
			Shader objects may be shared.
		*/
		class OpenGLShaderObject : public OpenGLObject
		{
			GE_RTTI(GraphicsEngine::Graphics::OpenGLShaderObject)

		public:
			OpenGLShaderObject();
			explicit OpenGLShaderObject(GLenum type, const std::string& sourcePath);
			virtual ~OpenGLShaderObject();

			bool_t Compile();

			const GLuint& GetHandle() const;
			const GLenum& GetType() const;

		private:
			void Create();
			void Destroy();

			GLuint mHandle;
			GLenum mType;
			std::string mSourcePath;
			bool_t mIsSpecialized;
		};
	}
}

#endif // GRAPHICS_RENDERING_BACKENDS_OPENGL_INTERNAL_OPENGL_VERTEX_SHADER_OBJECT_HPP