#include "PipelineState.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

PipelineState::PipelineState()
	: mIsEnabled(false)
{}

PipelineState::PipelineState(bool_t isEnabled)
	: mIsEnabled(isEnabled)
{}

PipelineState::~PipelineState()
{
	mIsEnabled = false;
}

void PipelineState::SetIsEnabled(bool_t isEnabled)
{
	mIsEnabled = isEnabled;
}

bool_t PipelineState::GetIsEnabled() const
{
	return mIsEnabled;
}