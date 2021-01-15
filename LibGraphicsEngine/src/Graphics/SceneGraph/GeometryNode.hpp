#ifndef GRAPHICS_SCENE_GRAPH_GEOMETRY_NODE_HPP
#define GRAPHICS_SCENE_GRAPH_GEOMETRY_NODE_HPP

#include "Graphics/SceneGraph/Node.hpp"
#include <vector>
#include <string>
#include <functional>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class GeometricPrimitive;

		/* GeometryNoded is a visual/renderable node.
		   Other visual nodes are: LightNode, TextNode.
		   All these visual nodes have the VisualComponent. */
		class GeometryNode : public Node
		{
			GE_RTTI(GraphicsEngine::Graphics::GeometryNode)

		public:
			GeometryNode();
			explicit GeometryNode(const std::string& name);
			virtual ~GeometryNode();

			void AttachGeometry(GeometricPrimitive* pPrimitive);
			void DettachGeometry(GeometricPrimitive* pPrimitive);
			void DettachAllGeometry();

			void ForEachPrimitive(std::function< void(GeometricPrimitive*) > callback);

			const std::vector<GeometricPrimitive*>& GetGeometricPrimitives() const;

			///////// Visitor Pattern ///////
			virtual void Accept(NodeVisitor& visitor) override;

		private:
			void Create();
			void Destroy();

			std::vector<GeometricPrimitive*> mPrimitives;
		};
	}
}

#endif // GRAPHICS_SCENE_GRAPH_GEOMETRY_NODE_HPP