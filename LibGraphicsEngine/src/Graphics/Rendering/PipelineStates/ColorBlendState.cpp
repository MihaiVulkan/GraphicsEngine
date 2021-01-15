#include "Graphics/Rendering/PipelineStates/ColorBlendState.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;


ColorBlendState::ColorBlendState()
	: mIsBlendEnabled(false)
	, mSrcColorBlendFactor(BlendFactor::GE_BF_COUNT)
	, mDstColorBlendFactor(BlendFactor::GE_BF_COUNT)
	, mColorBlendOp(BlendOp::GE_BO_COUNT)
	, mSrcAlphaBlendFactor(BlendFactor::GE_BF_COUNT)
	, mDstAlphaBlendFactor(BlendFactor::GE_BF_COUNT)
	, mAlphaBlendOp(BlendOp::GE_BO_COUNT)
	, mColorWriteMask(0)
	, mConstantColor{}
{}

ColorBlendState::ColorBlendState(bool_t isEnabled)
	: ColorBlendState()
{
	mIsEnabled = isEnabled;
}

ColorBlendState::~ColorBlendState()
{
	mIsBlendEnabled = false;
	mSrcColorBlendFactor = BlendFactor::GE_BF_COUNT;
	mDstColorBlendFactor = BlendFactor::GE_BF_COUNT;
	mColorBlendOp = BlendOp::GE_BO_COUNT;
	mSrcAlphaBlendFactor = BlendFactor::GE_BF_COUNT;
	mDstAlphaBlendFactor= BlendFactor::GE_BF_COUNT;
	mAlphaBlendOp = BlendOp::GE_BO_COUNT;
	mColorWriteMask = 0;
	mConstantColor = {};
}

const bool_t& ColorBlendState::GetIsBlendEnabled() const
{
	return mIsBlendEnabled;
}

void ColorBlendState::SetIsBlendEnabled(bool_t value)
{
	mIsBlendEnabled = value;
}

const ColorBlendState::BlendFactor& ColorBlendState::GetSrcColorBlendFactor() const
{
	return mSrcColorBlendFactor;
}

void ColorBlendState::SetSrcColorBlendFactor(const ColorBlendState::BlendFactor& factor)
{
	mSrcColorBlendFactor = factor;
}

const ColorBlendState::BlendFactor& ColorBlendState::GetDstColorBlendFactor() const
{
	return mDstColorBlendFactor;
}

void ColorBlendState::SetDstColorBlendFactor(const ColorBlendState::BlendFactor& factor)
{
	mDstColorBlendFactor = factor;
}

const ColorBlendState::BlendOp& ColorBlendState::GetColorBlendOp() const
{
	return mColorBlendOp;
}

void ColorBlendState::SetColorBlendOp(const ColorBlendState::BlendOp& blendOp)
{
	mColorBlendOp = blendOp;
}

const ColorBlendState::BlendFactor& ColorBlendState::GetSrcAlphaBlendFactor() const
{
	return mSrcAlphaBlendFactor;
}

void ColorBlendState::SetSrcAlphaBlendFactor(const ColorBlendState::BlendFactor& factor)
{
	mSrcAlphaBlendFactor = factor;
}

const ColorBlendState::BlendFactor& ColorBlendState::GetDstAlphaBlendFactor() const
{
	return mDstAlphaBlendFactor;
}

void ColorBlendState::SetDstAlphaBlendFactor(const ColorBlendState::BlendFactor& factor)
{
	mDstAlphaBlendFactor = factor;
}

const ColorBlendState::BlendOp& ColorBlendState::GetAlphaBlendOp() const
{
	return mAlphaBlendOp;
}

void ColorBlendState::SetAlphaBlendOp(const ColorBlendState::BlendOp& blendOp)
{
	mAlphaBlendOp = blendOp;
}

const uint32_t& ColorBlendState::GetColorWriteMask() const
{
	return mColorWriteMask;
}

void  ColorBlendState::SetColorWriteMask(uint32_t mask)
{
	mColorWriteMask = mask;
}

const Color4f& ColorBlendState::GetConstantColor() const
{
	return mConstantColor;
}

void ColorBlendState::SetConstantColor(const Color4f& color)
{
	mConstantColor = color;
}