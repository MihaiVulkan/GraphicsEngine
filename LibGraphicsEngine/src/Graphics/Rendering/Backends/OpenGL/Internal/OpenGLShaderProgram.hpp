#ifndef GRAPHICS_RENDERING_BACKENDS_OPENGL_INTERNAL_OPENGL_VERTEX_SHADER_PROGRAM_HPP
#define GRAPHICS_RENDERING_BACKENDS_OPENGL_INTERNAL_OPENGL_VERTEX_SHADER_PROGRAM_HPP

#include "Graphics/Rendering/Backends/OpenGL/Common/OpenGLObject.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		/*
			*Wrapper for GL Shader Program object*

			Shader objects that are to be used by one or more of the programmable stages of
			the GL are linked together to form a program object. The shader programs that
			are executed by these programmable stages are called executables. All information
			necessary for defining each executable is encapsulated in a program object.
		*/
		class OpenGLShaderProgram : public OpenGLObject
		{
			GE_RTTI(GraphicsEngine::Graphics::OpenGLShaderProgram)

		public:
			OpenGLShaderProgram();
			virtual ~OpenGLShaderProgram();

			void AttachShader(GLuint shaderHandle);
			void DetachShader(GLuint shaderHandle);

			bool_t Link();

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

#endif // GRAPHICS_RENDERING_BACKENDS_OPENGL_INTERNAL_OPENGL_VERTEX_SHADER_PROGRAM_HPP