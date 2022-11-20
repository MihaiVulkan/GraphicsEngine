#include "Graphics/Rendering/Backends/OpenGL/Internal/OpenGLShaderProgram.hpp"
#include "Foundation/Logger.hpp"
#include <vector>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

OpenGLShaderProgram::OpenGLShaderProgram()
	: mHandle(NULL)
{
	Create();
}

OpenGLShaderProgram::~OpenGLShaderProgram()
{
	Destroy();
}

void OpenGLShaderProgram::Create()
{
	mHandle = glCreateProgram();
}

void OpenGLShaderProgram::Destroy()
{
	if (mHandle)
	{
		glDeleteProgram(mHandle);
		mHandle = NULL;
	}
}


void OpenGLShaderProgram::AttachShader(GLuint shaderHandle)
{
	// NOTE! OpenGL’s operation is undefined if no shader is bound, but no error is generated!

	glAttachShader(mHandle, shaderHandle);
}

void OpenGLShaderProgram::DetachShader(GLuint shaderHandle)
{
	glDetachShader(mHandle, shaderHandle);
}

bool_t OpenGLShaderProgram::Link()
{
	glLinkProgram(mHandle);

	GLint isLinked = 0;
	glGetProgramiv(mHandle, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(mHandle, GL_INFO_LOG_LENGTH, &maxLength);
		assert(maxLength > 0);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(mHandle, maxLength, &maxLength, &infoLog[0]);

		// The program is useless now. So delete it.
		glDeleteProgram(mHandle);

		LOG_ERROR("Shader Program %d linking failed, error: %s!", mHandle, infoLog.data());

		return false;
	}

	return true;
}

void OpenGLShaderProgram::Bind()
{
	glUseProgram(mHandle);
}

void OpenGLShaderProgram::UnBind()
{
	glUseProgram(0);
}

const GLuint& OpenGLShaderProgram::GetHandle() const
{
	return mHandle;
}