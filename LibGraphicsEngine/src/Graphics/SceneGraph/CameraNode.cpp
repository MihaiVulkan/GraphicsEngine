#include "CameraNode.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;


CameraNode::CameraNode()
	: Node()
{
	Create();
}

CameraNode::CameraNode(const std::string& name)
	: Node(name)
{
	Create();
}

CameraNode::~CameraNode()
{
	Destroy();
}

void CameraNode::Create()
{

}

void CameraNode::Destroy()
{

}

void CameraNode::Visit(std::function<void(Node*)> callback)
{
	if (callback)
		callback(this);

	//something else
}