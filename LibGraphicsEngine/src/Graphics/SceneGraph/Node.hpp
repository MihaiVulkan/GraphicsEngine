#ifndef GRAPHICS_SCENE_GRAPH_NODE_HPP
#define GRAPHICS_SCENE_GRAPH_NODE_HPP

#include "Foundation/Object.hpp"
#include <string>
#include <list>
#include <unordered_map>
#include <functional>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class NodeComponent;

		/* Node is the vase class of every node of the scene graph.
		   By defauly the node is not a renderable/visual element.
		*/
		class Node: public Object
		{
			GE_RTTI(GraphicsEngine::Graphics::Node)

		public:
			Node();
			explicit Node(const std::string& name);
			virtual ~Node();

			const std::string& GetName() const;
			void SetName(const std::string& name);

			bool_t HasParent() const;
			Node* GetParent() const;

			template< class NodeClass >
			NodeClass* GetParent(void)
			{
				return static_cast<NodeClass*>(mpParent);
			}

			void SetParent(Node* pParent);

			NodeComponent* GetComponentWithName(const std::string& componentName);

			template< class NODE_COMPONENT_CLASS >
			NODE_COMPONENT_CLASS* GetComponent(void)
			{
				return static_cast<NODE_COMPONENT_CLASS*>(GetComponentWithName(NODE_COMPONENT_CLASS::__CLASS_NAME));
			}

			bool_t HasComponentWithName(const std::string& componentName);
			bool_t HasComponent(NodeComponent* pComponent);

			void AttachComponent(NodeComponent* pComponent);
			Node* DettachFromParent();
			void DettachComponent(NodeComponent* pComponent);
			void DettachComponentWithName(const std::string& componentName);
			void DettachAllComponents();

			void StartComponents();
			void UpdateComponents(float32_t deltaTime);
			void ForEachComponent(std::function< void(NodeComponent*) > callback);

			bool_t GetIsEnabled() const;
			void SetIsEnabled(bool_t isEnabled);

			///////////

			virtual void Visit(std::function<void(Node*)> callback);

		private:
			void Create();
			void Destroy();

			std::string mName;
			Node* mpParent;
			bool_t mIsEnabled;

			std::unordered_map<std::string, NodeComponent*, std::hash<std::string>> mComponentMap;

			//TODO - add local/world transforms
		};
	}
}

#endif // GRAPHICS_SCENE_GRAPH_NODE_HPP