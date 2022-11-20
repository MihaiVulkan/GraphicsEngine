#if defined(OPENGL_RENDERER)
#include "Graphics/Rendering/Backends/OpenGL/Resources/OpenGLShader.hpp"
#include "Graphics/Rendering/Backends/OpenGL/Common/OpenGLUtils.hpp"
#include "Graphics/Rendering/Backends/OpenGL/Internal/OpenGLShaderObject.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

GADRShader::GADRShader()
	: mpOpenGLShaderObject(nullptr)
	, mpShader(nullptr)
{}

GADRShader::GADRShader(Renderer* pRenderer, Shader* pShader)
	: mpOpenGLShaderObject(nullptr)
	, mpShader(pShader)
{
	Create(pRenderer);
}

GADRShader::~GADRShader()
{
	Destroy();
}

void GADRShader::Create(Renderer* pRenderer)
{
	assert(pRenderer != nullptr);
	assert(mpShader != nullptr);

	GLenum glShaderType = OpenGLUtils::ShaderStageToOpenGLShaderType(mpShader->GetShaderStage());

	mpOpenGLShaderObject = GE_ALLOC(OpenGLShaderObject)(glShaderType, mpShader->GetSourcePath());
	assert(mpOpenGLShaderObject != nullptr);
}

void GADRShader::Destroy()
{
	if (mpShader)
	{
		mpShader = nullptr;
	}

	GE_FREE(mpOpenGLShaderObject);
}

Shader* GADRShader::GetShader() const
{
	return mpShader;
}

OpenGLShaderObject* GADRShader::GetGLShaderObject() const
{
	return mpOpenGLShaderObject;
}
#endif // OPENGL_RENDERER