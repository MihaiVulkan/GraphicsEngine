#include "Graphics/Rendering/Resources/RenderFrameBuffer.hpp"
#include "Graphics/Rendering/Resources/RenderTarget.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

RenderFrameBuffer::RenderFrameBuffer()
	: mWidth(0)
	, mHeight(0)
	, mClearColor{}
	, mClearFlag(ClearFlag::GE_CF_COUNT)
{}

RenderFrameBuffer::RenderFrameBuffer(uint32_t width, uint32_t height)
	: mWidth(width)
	, mHeight(height)
	, mClearColor{}
	, mClearFlag(ClearFlag::GE_CF_COUNT)
{
	Create();
}


RenderFrameBuffer::~RenderFrameBuffer()
{
	Destroy();
}


void RenderFrameBuffer::Create()
{
	//TODO
}

void RenderFrameBuffer::Destroy()
{
	mWidth = 0;
	mHeight = 0;
	mClearColor = {};
	mClearFlag = ClearFlag::GE_CF_COUNT;

	mRenderTargetMap.clear();
}

const RenderFrameBuffer::RenderTargetMap& RenderFrameBuffer::GetRenderTargets() const
{
	return mRenderTargetMap;
}

uint32_t RenderFrameBuffer::GetWidth() const
{
	return mWidth;
}

uint32_t RenderFrameBuffer::GetHeight() const
{
	return mHeight;
}

const Color3f& RenderFrameBuffer::GetClearColor() const
{
	return mClearColor;
}

RenderFrameBuffer::ClearFlag RenderFrameBuffer::GetClearFlag() const
{
	return mClearFlag;
}

void RenderFrameBuffer::SetClearColor(const Color3f& color)
{
	mClearColor = color;
}

void RenderFrameBuffer::SetClearFlag(RenderFrameBuffer::ClearFlag flag)
{
	mClearFlag = flag;
}