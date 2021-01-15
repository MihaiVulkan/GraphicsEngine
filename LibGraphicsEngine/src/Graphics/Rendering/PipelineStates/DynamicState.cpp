#include "Graphics/Rendering/PipelineStates/DynamicState.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;


DynamicState::DynamicState()
{}

DynamicState::DynamicState(bool_t isEnabled)
{
	mIsEnabled = isEnabled;
}

DynamicState::~DynamicState()
{
	mStates.clear();
}


void DynamicState::Add(DynamicState::State state)
{
	mStates.push_back(state);
}

bool_t DynamicState::HasStates() const
{
	return (false == mStates.empty());
}

const std::vector<DynamicState::State>& DynamicState::GetStates() const
{
	return mStates;
}