#ifndef GRAPHICS_SCENE_GRAPH_GEOMETRY_NODE_HPP
#define GRAPHICS_SCENE_GRAPH_GEOMETRY_NODE_HPP

#include "Node.hpp"
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

			virtual void Visit(std::function<void(Node*)> callback) override;

			const std::vector<GeometricPrimitive*>& GetGeometricPrimitives() const;

		private:
			void Create();
			void Destroy();

			std::vector<GeometricPrimitive*> mPrimitives;
		};
	}
}

#endif // GRAPHICS_SCENE_GRAPH_GEOMETRY_NODE_HPP