#include "Graphics/SceneGraph/LightNode.hpp"
#include "Graphics/Lights/Light.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

#include <cassert>

LightNode::LightNode()
	: Node()
{
	Create();
}

LightNode::LightNode(const std::string& name)
	: Node(name)
{
	Create();
}

LightNode::~LightNode()
{
	Destroy();
}

void LightNode::Create()
{

}

void LightNode::Destroy()
{
	GE_FREE(mpLight);
}

void LightNode::SetLight(Light* pLight)
{
	mpLight = pLight;
}

Light* LightNode::GetLight() const
{
	return mpLight;
}

void LightNode::Accept(NodeVisitor& visitor)
{
	visitor.Visit(this);
}