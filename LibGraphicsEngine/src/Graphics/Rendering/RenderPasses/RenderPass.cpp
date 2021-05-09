#include "Graphics/Rendering/RenderPasses/RenderPass.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

RenderPass::RenderPass()
	: mPassType(PassType::GE_PT_COUNT)
{}

RenderPass::RenderPass(RenderPass::PassType type)
	: mPassType(type)
{}

RenderPass::~RenderPass()
{
	mPassType = PassType::GE_PT_COUNT;
}

const RenderPass::PassType& RenderPass::GetPassType() const
{
	return mPassType;
}