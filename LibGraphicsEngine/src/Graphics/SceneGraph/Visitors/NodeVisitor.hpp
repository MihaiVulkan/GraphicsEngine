#ifndef GRAPHICS_SCENE_GRAPH_VISITORS_NODE_VISITOR_HPP
#define GRAPHICS_SCENE_GRAPH_VISITORS_NODE_VISITOR_HPP

#include "Foundation/Object.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Node;
		class GroupNode;
		class GeometryNode;
		class LightNode;
		class CameraNode;

		class NodeVisitor : public Object
		{
			GE_RTTI(GraphicsEngine::Graphics::NodeVisitor)

		public:
			NodeVisitor();
			virtual ~NodeVisitor();

			virtual void Traverse(Node* pNode);
			virtual void Visit(Node* pNode);
			virtual void Visit(GroupNode* pNode);
			virtual void Visit(GeometryNode* pNode);
			virtual void Visit(LightNode* pNode);
			virtual void Visit(CameraNode* pNode);

		private:
			NO_COPY_NO_MOVE_CLASS(NodeVisitor);

		};
	}
}

#endif // GRAPHICS_SCENE_GRAPH_VISITORS_NODE_VISITOR_HPP