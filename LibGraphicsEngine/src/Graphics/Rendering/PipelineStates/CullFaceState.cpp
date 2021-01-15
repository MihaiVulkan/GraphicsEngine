#include "Graphics/Rendering/PipelineStates/CullFaceState.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

CullFaceState::CullFaceState()
	: mCullMode(CullMode::GE_CM_COUNT)
{}

CullFaceState::CullFaceState(bool_t isEnabled)
	: CullFaceState()
{
	mIsEnabled = isEnabled;
}

CullFaceState::~CullFaceState()
{
	mCullMode = CullMode::GE_CM_COUNT;
}

void CullFaceState::SetCullMode(CullFaceState::CullMode cullMode)
{
	mCullMode = cullMode;
}

const CullFaceState::CullMode& CullFaceState::GetCullMode() const
{
	return mCullMode;
}