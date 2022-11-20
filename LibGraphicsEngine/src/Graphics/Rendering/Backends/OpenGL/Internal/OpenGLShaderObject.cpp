#include "Graphics/Rendering/Backends/OpenGL/Internal/OpenGLShaderObject.hpp"
#include "Graphics/Rendering/Backends/OpenGL/Common/OpenGLCommon.hpp"
#include "Foundation/FileUtils.hpp"
#include "Foundation/Logger.hpp"
#include <vector>

#define SPIRV_EXT ".spv"
#define SHADER_ENTRY_POINT "main"

//#define LOAD_SHADER_SOURCE

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

OpenGLShaderObject::OpenGLShaderObject()
	: mHandle(NULL)
	, mType(0)
	, mIsSpecialized(false)
{}

OpenGLShaderObject::OpenGLShaderObject(GLenum type, const std::string& sourcePath)
	: mHandle(NULL)
	, mType(type)
	, mSourcePath(sourcePath)
	, mIsSpecialized(false)
{
	Create();
}

OpenGLShaderObject::~OpenGLShaderObject()
{
	Destroy();
}

void OpenGLShaderObject::Create()
{
	assert(false == mSourcePath.empty());

	mHandle = glCreateShader(mType);

#ifdef LOAD_SHADER_SOURCE
	// load shader source from source path
	std::string shaderSource;
	FileUtils::ReadTextFile(mSourcePath, shaderSource);
	assert(shaderSource.empty() == false);

	std::string extensions("#version 450\n#extension GL_KHR_vulkan_glsl : require\n");

	constexpr short kSize = 2;
	const char* shaderContent[kSize] = { extensions.c_str(), shaderSource.c_str() };
	int shaderContentLengths[kSize] = { extensions.length(), shaderSource.length() };
	glShaderSource(mHandle, kSize, shaderContent, shaderContentLengths);
#else
	// load SPIR-V shder compiled source (*.spv) binary file
	std::string sourcePathTmp(mSourcePath);

	//check if file path is a SPIR-V binary file
	if (sourcePathTmp.find(SPIRV_EXT) == std::string::npos) //if not found
	{
		//add it
		sourcePathTmp += SPIRV_EXT;
	}

	std::vector<char_t> shaderBinary;
	FileUtils::ReadBinaryFile(sourcePathTmp, shaderBinary);
	assert(shaderBinary.empty() == false);

	// load shader binary in SPIR-V format
	glShaderBinary(1, &mHandle, GL_SHADER_BINARY_FORMAT_SPIR_V, shaderBinary.data(), shaderBinary.size());
#endif // LOAD_SHADER_SOURCE
}

void OpenGLShaderObject::Destroy()
{
	if (mHandle)
	{
		// will delete this shader object only if it's detached from a shader program
		// if not it will be deleted later
		glDeleteShader(mHandle);
		mHandle = NULL;
	}
}

bool_t OpenGLShaderObject::Compile()
{
#ifdef LOAD_SHADER_SOURCE
	glCompileShader(mHandle);
#else
	// NOTE! In case of useing OpenGL binary SPIR-V specialized shaders
	// OpenGL doesn't allow to specialiaze the binary format of the same shader
	// more then once to we use the below flag to avoid another specialization
	// if already done
	if (mIsSpecialized)
		return true;

	// Specialization is equivalent to compilation.
	glSpecializeShader(mHandle, SHADER_ENTRY_POINT, 0, nullptr, nullptr);
#endif // LOAD_SHADER_SOURCE

	GLint isCompiled = 0;
	glGetShaderiv(mHandle, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(mHandle, GL_INFO_LOG_LENGTH, &maxLength);
		assert(maxLength > 0);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(mHandle, maxLength, &maxLength, &errorLog[0]);

		// Provide the infolog in whatever manor you deem best.
		glDeleteShader(mHandle); // Don't leak the shader.

		LOG_ERROR("Shader %s compilation failed, error: %s!", mSourcePath.c_str(), errorLog.data());

		mIsSpecialized = false;
		return false;
	}

	mIsSpecialized = true;

	return true;
}

const GLuint& OpenGLShaderObject::GetHandle() const
{
	return mHandle;
}

const GLenum& OpenGLShaderObject::GetType() const
{
	return mType;
}