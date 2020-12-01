#include "AlphaState.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

AlphaState::AlphaState()
	: PipelineState(false)
	//, mIsBlendEnabled(false)
	, mSrcBlendMode(SrcBlendMode::SBM_SRC_ALPHA)
	, mDstBlendMode(DstBlendMode::DBM_ONE_MINUS_SRC_ALPHA)
	, mIsCompareEnabled(false)
	, mCompareMode(CompareMode::CM_ALWAYS)
	, mReference(0.0f)
	, mConstantColor(0.0f)
{

}

AlphaState::~AlphaState()
{}

const AlphaState::SrcBlendMode& AlphaState::GetSrcBlendMode() const
{
	return mSrcBlendMode;
}

void AlphaState::SetSrcBlendMode(const SrcBlendMode& srcBlendMode)
{

}

const AlphaState::DstBlendMode& AlphaState::GetDstBlendMode() const
{
	return mDstBlendMode;
}

void AlphaState::SetDstBlendMode(const DstBlendMode& dstBlendMode)
{

}

bool_t AlphaState::IsComparedEnabled() const
{
	return false;
}

void AlphaState::SetIsComparedEnabled(bool_t value)
{

}

const AlphaState::CompareMode& AlphaState::GetCompareMode() const
{
	return mCompareMode;
}

void AlphaState::SetCompareMode(const CompareMode& compareMode)
{}

bfloat32_t AlphaState::GetReference() const
{
	return 0.0f;
}

void AlphaState::SetReference(bfloat32_t ref)
{}

const Color4f& AlphaState::GetConstantColor() const
{
	return mConstantColor;
}

void AlphaState::SetConstantColor(const Color4f& color)
{}