#include "Graphics/Rendering/Backends/OpenGL/Internal/OpenGLSampler.hpp"
#include "Foundation/Logger.hpp"
#include <string>
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

OpenGLSampler::OpenGLSampler()
	: mHandle(NULL)
	, mData{}
{}

OpenGLSampler::OpenGLSampler(GLenum wrapU, GLenum wrapV, GLenum wrapW, GLenum minFilter, GLenum magFilter,
							 uint32_t minLod, uint32_t maxLod, uint32_t biasLod)
	: mHandle(NULL)
{
	mData.wrapU = wrapU;
	mData.wrapV = wrapV;
	mData.wrapW = wrapW;
	mData.minFilter = minFilter;
	mData.magFilter = magFilter;
	mData.minLod = minLod;
	mData.maxLod = maxLod;
	mData.biasLod = biasLod;

	Create();
}

OpenGLSampler::~OpenGLSampler()
{
	Destroy();
}

void OpenGLSampler::Create()
{
	glCreateSamplers(1, &mHandle);

	glSamplerParameteri(mHandle, GL_TEXTURE_WRAP_S, mData.wrapU);
	glSamplerParameteri(mHandle, GL_TEXTURE_WRAP_T, mData.wrapV);
	glSamplerParameteri(mHandle, GL_TEXTURE_WRAP_R, mData.wrapW);
	glSamplerParameteri(mHandle, GL_TEXTURE_MIN_FILTER, mData.minFilter);
	glSamplerParameteri(mHandle, GL_TEXTURE_MAG_FILTER, mData.magFilter);
	glSamplerParameterf(mHandle, GL_TEXTURE_MIN_LOD, mData.minLod);
	glSamplerParameterf(mHandle, GL_TEXTURE_MAX_LOD, mData.maxLod);
	glSamplerParameterf(mHandle, GL_TEXTURE_LOD_BIAS, mData.biasLod);
}

void OpenGLSampler::Destroy()
{
	if (mHandle)
	{
		glDeleteSamplers(1, &mHandle);
		mHandle = NULL;
	}
}

void OpenGLSampler::Bind(GLuint unit)
{
	glBindSampler(unit, mHandle);
}

void OpenGLSampler::UnBind()
{
	glBindSampler(0, 0);
}

const GLuint& OpenGLSampler::GetHandle() const
{
	return mHandle;
}

const OpenGLSampler::Data& OpenGLSampler::GetData() const
{
	return mData;
}