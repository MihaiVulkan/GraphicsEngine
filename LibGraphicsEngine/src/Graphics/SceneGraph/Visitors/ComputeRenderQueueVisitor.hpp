#ifndef GRAPHICS_SCENE_GRAPH_GROUP_NODE_HPP
#define GRAPHICS_SCENE_GRAPH_GROUP_NODE_HPP

#include "Graphics/SceneGraph/Visitors/NodeVisitor.hpp"
#include <vector>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class RenderQueue;

		class ComputeRenderQueueVisitor : public NodeVisitor
		{
			GE_RTTI(GraphicsEngine::Graphics::ComputeRenderQueueVisitor)

		public:
			ComputeRenderQueueVisitor();
			explicit ComputeRenderQueueVisitor(RenderQueue* pRenderQueue);
			virtual ~ComputeRenderQueueVisitor();

			virtual void Traverse(Node* pNode) override;
			virtual void Visit(Node* pNode) override;
			virtual void Visit(GroupNode* pNode) override;
			virtual void Visit(GeometryNode* pNode) override;
			virtual void Visit(CameraNode* pNode) override;

		private:
			NO_COPY_NO_MOVE(ComputeRenderQueueVisitor);

			RenderQueue* mpRenderQueue;
		};
	}
}

#endif // GRAPHICS_SCENE_GRAPH_GROUP_NODE_HPP