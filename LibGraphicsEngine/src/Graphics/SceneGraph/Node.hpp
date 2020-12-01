#ifndef GRAPHICS_SCENE_GRAPH_NODE_HPP
#define GRAPHICS_SCENE_GRAPH_NODE_HPP

#include "Foundation/TypeDefs.hpp"
#include "Foundation/RTTI.hpp"
#include <string>
#include <list>
#include <unordered_map>
#include <functional>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class NodeComponent;

		class Node: public RTTI
		{
			GE_RTTI(GraphicsEngine::Graphics::Node)

		public:
			Node();
			explicit Node(const std::string& name);
			virtual ~Node();

			const std::string& GetName() const;

			bool_t HasParent() const;
			Node* GetParent() const;

			template< class NodeClass >
			NodeClass* GetParent(void)
			{
				return static_cast<NodeClass*>(mpParent);
			}

			void SetParent(Node* pParent);

			NodeComponent* GetComponentWithName(const std::string& name);

			template< class NODE_COMPONENT_CLASS >
			NODE_COMPONENT_CLASS* GetComponent(void)
			{
				return static_cast<NODE_COMPONENT_CLASS*>(GetComponentWithName(NODE_COMPONENT_CLASS::__CLASS_NAME));
			}

			bool_t HasComponent(NodeComponent* pComponent);

			void AttachComponent(NodeComponent* pComponent);
			Node* DettachFromParent();
			void DettachComponent(NodeComponent* pComponent);
			void DettachComponentWithName(const std::string& componentName);
			void DettachAllComponents();

			void UpdateComponents(bfloat32_t deltaTime);
			void ForEachComponent(std::function< void(NodeComponent*) > callback);

			bool_t GetIsEnabled() const;
			void SetIsEnabled(bool_t isEnabled);

			///////////

			virtual void Visit(std::function<void(Node*)> callback);

		private:
			std::string mName;

			Node* mpParent;

			std::unordered_map<std::string, NodeComponent*, std::hash<std::string>> mComponentMap;

			bool_t mIsEnabled;

			//TODO - add local/world transforms
		};
	}
}

#endif // GRAPHICS_SCENE_GRAPH_NODE_HPP