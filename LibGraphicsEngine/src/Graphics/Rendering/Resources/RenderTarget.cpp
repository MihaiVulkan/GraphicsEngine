#include "Graphics/Rendering/Resources/RenderTarget.hpp"
#include "Graphics/Rendering/Resources/Texture.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;


RenderTarget::RenderTarget()
	: mType(TargetType::GE_TT_COUNT)
	, mOutput(TargetOutput::GE_TO_COUNT)
	, mpTexture(nullptr)
	, mWidth(0)
	, mHeight(0)
	, mIsFloatData(false)
{}

RenderTarget::RenderTarget(TargetType type, TargetOutput output, uint32_t width, uint32_t height, bool_t isFloatData)
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
	Texture::Format format = Texture::Format::GE_TF_COUNT;

	switch (mType)
	{
	case TargetType::GE_TT_COLOR_RGB:
		format = mIsFloatData ? Texture::Format::GE_TF_R32G32B32_SFLOAT : Texture::Format::GE_TF_R32G32B32_SINT;
		break;
	case TargetType::GE_TT_COLOR_RGBA:
		format = mIsFloatData ? Texture::Format::GE_TF_R32G32B32A32_SFLOAT : Texture::Format::GE_TF_R32G32B32A32_SINT;
		break;
	case TargetType::GE_TT_DEPTH_16:
		format = Texture::Format::GE_TF_D16;
		break;
	case TargetType::GE_TT_DEPTH_24:
		format = Texture::Format::GE_TF_D24;
		break;
	case TargetType::GE_TT_DEPTH_32:
		format = Texture::Format::GE_TF_D32;
		break;
	}

	mpTexture = GE_ALLOC(Texture2D)(Texture::TextureType::GE_TT_2D, format, Texture::WrapMode::GE_WM_REPEAT, Texture::FilterMode::GE_FM_LINEAR, Texture::MipMapMode::GE_MM_NONE,
								mWidth, mHeight, 1);
	assert(mpTexture != nullptr);
}

void RenderTarget::Destroy()
{
	mIsFloatData = false;
	mHeight = 0;
	mWidth = 0;
	mOutput = TargetOutput::GE_TO_COUNT;
	mType = TargetType::GE_TT_COUNT;
	
	if (mpTexture)
	{
		GE_FREE(mpTexture);
	}
}

const RenderTarget::TargetType& RenderTarget::GetType() const
{
	return mType;
}

const RenderTarget::TargetOutput& RenderTarget::GetOutput() const
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