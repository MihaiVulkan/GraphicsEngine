#ifndef GRAPHICS_COMPONENTS_NODE_COMPONENT_HPP
#define GRAPHICS_COMPONENTS_NODE_COMPONENT_HPP

#include "Foundation/Object.hpp"
#include <string>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Node;

		/* NodeComponent is the base class for every component a scene graph node wants to have */
		class NodeComponent : public Object
		{
			GE_RTTI(GraphicsEngine::Graphics::NodeComponent)

		public:
			NodeComponent();
			explicit NodeComponent(const std::string& name);
			virtual ~NodeComponent();

			virtual void Start();
			virtual void Update(float32_t deltaTime);

			virtual void OnAttach();
			virtual void OnDettach();

			virtual const std::string& GetName() const;
			void SetName(const std::string& name);

			Node* GetNode() const;
			void SetNode(Node* pNode);

			bool_t GetIsEnabled() const;
			void SetIsEnabled(bool_t isEnabled);

		private:
			NO_COPY_NO_MOVE_CLASS(NodeComponent)

			void Create();
			void Destroy();

			std::string mName;
			Node* mpNode;
			bool_t mIsEnabled;
		};
	}
}

#endif // GRAPHICS_COMPONENTS_NODE_COMPONENT_HPP