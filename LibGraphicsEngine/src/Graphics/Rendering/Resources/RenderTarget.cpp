#include "RenderTarget.hpp"
#include "Graphics/Rendering/Resources/Texture.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;


RenderTarget::RenderTarget()
	: mType(Type::T_COUNT)
	, mOutput(Output::O_COUNT)
	, mpTexture(nullptr)
	, mWidth(0)
	, mHeight(0)
	, mIsFloatData(false)
{}

RenderTarget::RenderTarget(Type type, Output output, uint32_t width, uint32_t height, bool_t isFloatData)
	: mType(type)
	, mOutput(output)
	, mpTexture(nullptr) 
	, mWidth(width)
	, mHeight(height)
	, mIsFloatData(false)
{
	Create();
}

RenderTarget::~RenderTarget()
{
	Destroy();
}

void RenderTarget::Create()
{
	Texture::Format format = Texture::Format::TF_COUNT;

	switch (mType)
	{
	case Type::T_COLOR_RGB:
		format = mIsFloatData ? Texture::Format::TF_R32G32B32_SFLOAT : Texture::Format::TF_R32G32B32_SINT;
		break;
	case Type::T_COLOR_RGBA:
		format = mIsFloatData ? Texture::Format::TF_R32G32B32A32_SFLOAT : Texture::Format::TF_R32G32B32A32_SINT;
		break;
	case Type::T_DEPTH_16:
		format = Texture::Format::TF_D16;
		break;
	case Type::T_DEPTH_24:
		format = Texture::Format::TF_D24;
		break;
	case Type::T_DEPTH_32:
		format = Texture::Format::TF_D32;
		break;
	}

	mpTexture = GE_ALLOC(Texture2D)(Texture::Type::TEX_2D, format, Texture::WrapMode::WM_REPEAT, Texture::FilterMode::FM_LINEAR, Texture::MipMapMode::MM_NONE,
								mWidth, mHeight, 1);
}

void RenderTarget::Destroy()
{
	mIsFloatData = false;
	mHeight = 0;
	mWidth = 0;
	mOutput = Output::O_COUNT;
	mType = Type::T_COUNT;
	
	if (mpTexture)
	{
		GE_FREE(mpTexture);
	}
}

const RenderTarget::Type& RenderTarget::GetType() const
{
	return mType;
}

const RenderTarget::Output& RenderTarget::GetOutput() const
{
	return mOutput;
}

uint32_t RenderTarget::GetWidth() const
{
	return mWidth;
}

uint32_t RenderTarget::GetHeight() const
{
	return mHeight;
}

Texture2D* RenderTarget::GetTexture() const
{
	return mpTexture;
}