#include "Shader.hpp"
#include "Graphics/ShaderTools/GLSL/GLSLShaderParser.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;


Shader::Shader()
	: mShaderStage(ShaderStage::GE_SS_COUNT)
	, mSourcePath()
	, mpGLSLParser(nullptr)
{}

Shader::Shader(const std::string& sourcePath)
	: mShaderStage(ShaderStage::GE_SS_COUNT)
	, mSourcePath(sourcePath)
	, mpGLSLParser(nullptr)
{
	Create();
}

Shader::~Shader()
{
	Destroy();
}

void Shader::Create()
{
	mpGLSLParser = GE_ALLOC(GLSLShaderParser)(mSourcePath);
	assert(mpGLSLParser != nullptr);

	bool_t res = mpGLSLParser->Parse(mSourcePath);
	assert(res == true);

	mShaderStage = mpGLSLParser->GetStage();
}

void Shader::Destroy()
{
	mShaderStage = Shader::ShaderStage::GE_SS_COUNT;
	mSourcePath.clear();

	GE_FREE(mpGLSLParser);
}

const Shader::ShaderStage& Shader::GetShaderStage() const
{
	return mShaderStage;
}

const std::string& Shader::GetSourcePath() const
{
	return mSourcePath;
}

GLSLShaderParser* Shader::GetGLSLParser() const
{
	return mpGLSLParser;
}