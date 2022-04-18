#include "Graphics/Rendering/VisualEffects/VisualEffect.hpp"
#include "Graphics/Rendering/Renderer.hpp"
#include "Graphics/Rendering/Resources/VertexFormat.hpp"
#include "Graphics/Cameras/Camera.hpp"
#include "Graphics/SceneGraph/GeometryNode.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

VisualEffect::VisualEffect()
	: mEffectType(EffectType::GE_PT_COUNT)
	, mpTargetNode(nullptr)
	, mPrimitiveTopology(VisualPass::PrimitiveTopology::GE_PT_COUNT)
	, mFaceWinding(VisualPass::FaceWinding::GE_FW_COUNT)
	, mPolygonMode(VisualPass::PolygonMode::GE_PM_COUNT)
{}

VisualEffect::VisualEffect(VisualEffect::EffectType effectType)
	: VisualEffect()
{
	mEffectType = effectType;

	mEffectName = GetClassName_();
}

VisualEffect::~VisualEffect()
{
	for (auto& it : mPassMap)
	{
		for (auto& pass : it.second)
		{
			GE_FREE(pass);
		}
	}
	mPassMap.clear();
}

void VisualEffect::Init()
{
	InitCustomEffect();
}

void VisualEffect::Reset() // screen resize
{
	//TODO
}

void VisualEffect::InitPasses(Renderer* pRenderer)
{
	assert(pRenderer != nullptr);
	assert(mPassMap.size() > 0);

	for (auto& it : mPassMap)
	{
		auto& passes = it.second;
		for (auto* pPass : passes)
		{
			if (pPass)
			{
				//TODO - improve this
				pPass->SetPrimitiveTopology(mPrimitiveTopology);
				pPass->SetFaceWinding(mFaceWinding);
				pPass->SetPolygonMode(mPolygonMode);

				pPass->Init(pRenderer, this);
			}
		}
	}
}

const VisualEffect::PassMap& VisualEffect::GetPasses() const
{
	return mPassMap;
}

void VisualEffect::SetPrimitiveTopology(VisualPass::PrimitiveTopology topology)
{
	mPrimitiveTopology = topology;
}

void VisualEffect::SetFaceWinding(VisualPass::FaceWinding winding)
{
	mFaceWinding = winding;
}

void VisualEffect::SetPolygonMode(VisualPass::PolygonMode mode)
{
	mPolygonMode = mode;
}

void VisualEffect::SetTargetNode(GeometryNode* pGeoNode)
{
	mpTargetNode = pGeoNode;
}

GeometryNode* VisualEffect::GetTargetNode()
{
	return mpTargetNode;
}

void VisualEffect::AddDependencyNode(GeometryNode* pGeoNode)
{
	mDependencyNodes.push_back(pGeoNode);
}

std::vector<GeometryNode*> VisualEffect::GetDependencyNodes()
{
	return mDependencyNodes;
}

VisualEffect::EffectType VisualEffect::GetEffectType() const
{
	return mEffectType;
}

std::string VisualEffect::EffectTypeToStr(VisualEffect::EffectType type)
{
	std::string str;
	switch (type)
	{
	case EffectType::GE_ET_UNLIT:
		str = "Unlit";
		break;
	case EffectType::GE_ET_LIT:
		str = "Lit";
		break;
	case EffectType::GE_ET_LIT_SHADOWS:
		str = "Lit with Shadows";
		break;
	case EffectType::GE_ET_CUSTOM:
		str = "Custom";
		break;
	case EffectType::GE_PT_COUNT:
	default:
		LOG_ERROR("Invalid visual effect type!");
	}

	return str;
}