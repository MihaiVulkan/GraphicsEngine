#include "Graphics/Rendering/Resources/RenderTarget.hpp"
#include "Graphics/Rendering/Resources/Texture.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"

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

RenderTarget::RenderTarget(RenderTarget::TargetType type, RenderTarget::TargetOutput output, uint32_t width, uint32_t height, bool_t isFloatData)
	: mType(type)
	, mOutput(output)
	, mpTexture(nullptr) 
	, mWidth(width)
	, mHeight(height)
	, mIsFloatData(isFloatData) //TODO - not currently used
{
	Create();
}

RenderTarget::~RenderTarget()
{
	Destroy();
}

void RenderTarget::Create()
{
	assert(mWidth > 0);
	assert(mHeight > 0);

	Texture::TextureFormat format = Texture::TextureFormat::GE_TF_COUNT;

	switch (mType)
	{
	case TargetType::GE_TT_COLOR:
		format = Texture::TextureFormat::GE_TF_R8G8B8A8_UNORM;
		break;
	case TargetType::GE_TT_DEPTH_STENCIL:
		format = Texture::TextureFormat::GE_TF_D32_S8;
		break;
	case TargetType::GE_TT_COUNT:
	default:
		LOG_ERROR("Invalid Texture Format!");
	}

	mpTexture = GE_ALLOC(Texture2D)(Texture::TextureType::GE_TT_2D, format, Texture::WrapMode::GE_WM_CLAMP_TO_EDGE, Texture::FilterMode::GE_FM_LINEAR, 
									Texture::MipMapMode::GE_MM_LINEAR, mWidth, mHeight, 1);
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