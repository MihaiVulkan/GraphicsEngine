#include "CameraNode.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;


CameraNode::CameraNode()
{}

CameraNode::CameraNode(const std::string& name)
{}

CameraNode::~CameraNode()
{}

void CameraNode::Visit(std::function<void(Node*)> callback)
{
	if (callback)
		callback(this);

	//something else
}