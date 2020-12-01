#ifndef GRAPHICS_SCENE_GRAPH_GROUP_NODE_HPP
#define GRAPHICS_SCENE_GRAPH_GROUP_NODE_HPP

#include "Node.hpp"
#include <vector>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class GroupNode : public Node
		{
			GE_RTTI(GraphicsEngine::Graphics::GroupNode)

		public:
			GroupNode();
			explicit GroupNode(const std::string& name);
			virtual ~GroupNode();

			bool_t HasChildren() const;

			void AttachNode(Node* pNode);
			void DettachNode(Node* pNode);
			void DettachAllNodes();

			Node* GetNodeAt(uint32_t index);
			Node* GetNode(const std::string& name);

			virtual void Visit(std::function<void(Node*)> callback) override;

		private:
			std::vector<Node*> mChildren;
		};
	}
}

#endif // GRAPHICS_SCENE_GRAPH_GROUP_NODE_HPP