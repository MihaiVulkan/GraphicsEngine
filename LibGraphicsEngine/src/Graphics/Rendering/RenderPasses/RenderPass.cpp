#include "RenderPass.hpp"
#include "Graphics/Rendering/Renderer.hpp"
#include "Graphics/Rendering/RenderQueue.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

RenderPass::RenderPass()
	: mPassType(PassType::GE_PT_COUNT)
{}

RenderPass::~RenderPass()
{
	mPassType = PassType::GE_PT_COUNT;
}

const RenderPass::PassType& RenderPass::GetPassType() const
{
	return mPassType;
}