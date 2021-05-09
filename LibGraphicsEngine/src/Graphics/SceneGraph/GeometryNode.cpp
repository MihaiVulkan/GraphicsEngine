#include "Graphics/SceneGraph/GeometryNode.hpp"
#include "Graphics/GeometricPrimitives/GeometricPrimitive.hpp"
#include "Graphics/Components/VisualComponent.hpp"
#include "Graphics/Rendering/Resources/UniformBuffer.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <algorithm> // std::remove()
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

GeometryNode::GeometryNode()
	: Node()
	, mpGeometry(nullptr)
	, mIsLit(false)
{
	Create();
}

GeometryNode::GeometryNode(const std::string& name)
	: Node(name)
	, mpGeometry(nullptr)
	, mIsLit(false)
{
	Create();
}

GeometryNode::~GeometryNode()
{
	Destroy();
}

void GeometryNode::Create()
{
	// TODO - object lifetime management
	AttachComponent(GE_ALLOC(VisualComponent)("VisualComponent"));
}

void GeometryNode::Destroy()
{
	//TODO - improve cleanup
	if (mpGeometry)
	{
		if (mpGeometry->IsModel())
		{
			//The renderer cleanups up as the model is covered by it
			mpGeometry = nullptr;
		}
		else
		{
			GE_FREE(mpGeometry);
		}
	}

	// TODO - object lifetime management
	DettachComponentWithName("VisualComponent");
}

GeometricPrimitive* GeometryNode::GetGeometry() const
{
	return mpGeometry;
}

void GeometryNode::SetGeometry(GeometricPrimitive* pGeometry)
{
	mpGeometry = pGeometry;
}

bool_t GeometryNode::IsLit() const
{
	return mIsLit;
}

void GeometryNode::SetIsLit(bool_t value)
{
	mIsLit = value;

	// if lit we add another uniform buffer for lights data
	// Uniform Buffer - fragment shader
	if (mIsLit)
	{
		auto* pUB = GE_ALLOC(UniformBuffer);
		assert(pUB != nullptr);

		GetComponent<VisualComponent>()->AddUniformBuffer(Shader::ShaderStage::GE_SS_FRAGMENT, pUB);
	}
}

void GeometryNode::Accept(NodeVisitor& visitor)
{
	visitor.Visit(this);
}