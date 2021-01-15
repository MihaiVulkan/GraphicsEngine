#include "Graphics/Rendering/PipelineStates/DepthStencilState.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;


DepthStencilState::DepthStencilState()
	: PipelineState(false)
	, mIsDepthEnabled(false)
	, mIsDepthWritable(false)
	, mDepthCompareOp(CompareOp::GE_CO_COUNT)
	, mIsStencilEnabled(false)
	, mStencilFailOp(StencilOp::GE_SO_COUNT)
	, mStencilPassDepthPassOp(StencilOp::GE_SO_COUNT)
	, mStencilPassDepthFailOp(StencilOp::GE_SO_COUNT)
	, mStencilCompareOp(CompareOp::GE_CO_COUNT)
	, mStencilCompareMask(0)
	, mStencilWriteMask(0)
	, mStencilReference(0)
{}

DepthStencilState::DepthStencilState(bool_t isEnabled)
	: DepthStencilState()
{
	mIsEnabled = isEnabled;
}

DepthStencilState::~DepthStencilState()
{
	mIsDepthEnabled = false;
	mIsDepthWritable = false;
	mDepthCompareOp = CompareOp::GE_CO_COUNT;
	mIsStencilEnabled = false;
	mStencilFailOp = StencilOp::GE_SO_COUNT;
	mStencilPassDepthPassOp = StencilOp::GE_SO_COUNT;
	mStencilPassDepthFailOp = StencilOp::GE_SO_COUNT;
	mStencilCompareOp = CompareOp::GE_CO_COUNT;
	mStencilCompareMask = 0;
	mStencilWriteMask = 0;
	mStencilReference = 0;
}

const bool_t& DepthStencilState::GetIsDepthEnabled() const
{
	return mIsDepthEnabled;
}

void DepthStencilState::SetIsDepthEnabled(bool_t value)
{
	mIsDepthEnabled = value;
}

const bool_t& DepthStencilState::GetIsDepthWritable() const
{
	return mIsDepthWritable;
}

void DepthStencilState::SetIsDepthWritable(bool_t value)
{
	mIsDepthWritable = value;
}

const DepthStencilState::CompareOp& DepthStencilState::GetDepthCompareOp() const
{
	return mDepthCompareOp;
}

void DepthStencilState::SetDepthCompareOp(DepthStencilState::CompareOp compOp)
{
	mDepthCompareOp = compOp;
}

const bool_t& DepthStencilState::GetIsStencilEnabled() const
{
	return mIsStencilEnabled;
}

void DepthStencilState::SetIsStencilEnabled(bool_t value)
{
	mIsStencilEnabled = value;
}

const DepthStencilState::StencilOp& DepthStencilState::GetStencilFailOp() const
{
	return mStencilFailOp;
}

void DepthStencilState::SetStencilFailOp(DepthStencilState::StencilOp stencilOp)
{
	mStencilFailOp = stencilOp;
}

const DepthStencilState::StencilOp& DepthStencilState::GetStencilPassDepthPassOp() const
{
	return mStencilPassDepthPassOp;
}

void DepthStencilState::SetStencilPassDepthPassOp(DepthStencilState::StencilOp stencilOp)
{
	mStencilPassDepthPassOp = stencilOp;
}

const DepthStencilState::StencilOp& DepthStencilState::GetStencilPassDepthFailOp() const
{
	return mStencilPassDepthFailOp;
}
void DepthStencilState::SetStencilPassDepthFailOp(DepthStencilState::StencilOp stencilOp)
{
	mStencilPassDepthFailOp = stencilOp;
}

const DepthStencilState::CompareOp& DepthStencilState::GetStencilCompareOp() const
{
	return mStencilCompareOp;
}
void DepthStencilState::SetStencilCompareOp(DepthStencilState::CompareOp compOp)
{
	mStencilCompareOp = compOp;
}

const uint32_t& DepthStencilState::GetStencilCompareMask() const
{
	return mStencilCompareMask;
}

void DepthStencilState::SetStencilCompareMask(uint32_t mask)
{
	mStencilCompareMask = mask;
}

const uint32_t& DepthStencilState::GetStencilWriteMask() const
{
	return mStencilWriteMask;
}

void DepthStencilState::SetStencilWriteMask(uint32_t mask)
{
	mStencilWriteMask = mask;
}

const uint32_t& DepthStencilState::GetStencilReference() const
{
	return mStencilReference;
}

void DepthStencilState::SetStencilReference(uint32_t ref)
{
	mStencilReference = ref;
}