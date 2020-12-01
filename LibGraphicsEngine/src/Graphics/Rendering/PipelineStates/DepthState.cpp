#include "DepthState.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;


DepthState::DepthState()
	: PipelineState(true)
	, mIsWritable(true)
	, mCompare(CompareMode::CM_LEQUAL)
{

}
DepthState::~DepthState()
{}

bool_t DepthState::GetIsWritable() const
{
	return false;
}
void DepthState::SetIsWritable(bool_t value)
{}

const DepthState::CompareMode& DepthState::GetCompareMode() const
{
	return mCompare;
}
void DepthState::SetCompareMode(const CompareMode& compareMode)
{}