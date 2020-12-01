#ifndef GRAPHICS_COMPONENTS_NODE_COMPONENT_HPP
#define GRAPHICS_COMPONENTS_NODE_COMPONENT_HPP

#include "Foundation/TypeDefs.hpp"
#include "Foundation/RTTI.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Node;

		class NodeComponent : public RTTI
		{
			GE_RTTI(GraphicsEngine::Graphics::NodeComponent)

		public:
			NodeComponent();
			virtual ~NodeComponent();

			virtual void Start() {};
			virtual void Update(bfloat32_t deltaTime) {};

			// to be overriden by derived classes
			virtual const char_t* GetName() const;

			Node* GetNode() const;
			void SetNode(Node* pNode);

			bool_t GetIsEnabled() const;
			void SetIsEnabled(bool_t isEnabled);



		private:
			Node* mpNode;
			bool_t mIsEnabled;
		};
	}
}

#endif // GRAPHICS_COMPONENTS_NODE_COMPONENT_HPP