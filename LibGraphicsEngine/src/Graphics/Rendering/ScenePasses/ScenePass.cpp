#include "Graphics/Rendering/ScenePasses/ScenePass.hpp"
#include "Foundation/Logger.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

ScenePass::ScenePass()
	: mPassType(PassType::GE_PT_COUNT)
{
	// Default data values
	mData.depth = 1.0f;
	mData.stencil = 0;
}

ScenePass::ScenePass(ScenePass::PassType type)
	: ScenePass()
{
	mPassType = type;
}

ScenePass::~ScenePass()
{
	mPassType = PassType::GE_PT_COUNT;
}

const ScenePass::PassType& ScenePass::GetPassType() const
{
	return mPassType;
}

const ScenePass::ScenePassMap& ScenePass::GetPasses() const
{
	return mPassMap;
}

ScenePass::ScenePassMap& ScenePass::GetPasses()
{
	return mPassMap;
}

const ScenePass::PassData& ScenePass::Data() const
{
	return mData;
}

ScenePass::PassData& ScenePass::Data()
{
	return mData;
}

void ScenePass::AddRenderTarget(RenderTarget* pRenderTarget)
{
	assert(pRenderTarget != nullptr);

	mRenderTargets[pRenderTarget->GetType()] = pRenderTarget;
}

RenderTarget* ScenePass::GetRenderTarget(RenderTarget::TargetType targetType)
{
	return mRenderTargets[targetType];
}

const ScenePass::RenderTargetMap& ScenePass::GetRenderTargets() const
{
	return mRenderTargets;
}

std::string ScenePass::PassTypeToStr(ScenePass::PassType type)
{
	std::string str;
	switch (type)
	{
	case PassType::GE_PT_OFFSCREEN:
		str = "Offscreen";
		break;
	case PassType::GE_PT_SHADOWS:
		str = "Shadows";
		break;
	case PassType::GE_PT_STANDARD:
		str = "Standaard";
		break;
	case PassType::GE_PT_COMPOSITE:
		str = "Composite";
		break;
	case PassType::GE_PT_COUNT:
	default:
		LOG_ERROR("Invalid scene pass type");
	}

	return str;
}

const ScenePass::VisualEffect& ScenePass::GetVisualEffect() const
{
	return mVisualEffect;
}

void ScenePass::SetVisualEffect(const ScenePass::VisualEffect& effect)
{
	 mVisualEffect = effect;
}