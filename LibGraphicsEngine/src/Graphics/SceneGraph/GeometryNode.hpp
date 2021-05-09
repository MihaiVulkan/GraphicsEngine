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
		   All these visual nodes have the VisualComponent. */
		class GeometryNode : public Node
		{
			GE_RTTI(GraphicsEngine::Graphics::GeometryNode)

		public:
			GeometryNode();
			explicit GeometryNode(const std::string& name);
			virtual ~GeometryNode();

			GeometricPrimitive* GetGeometry() const;
			void SetGeometry(GeometricPrimitive* pPrimitive);

			bool_t IsLit() const;
			void SetIsLit(bool_t value);

			///////// Visitor Pattern ///////
			virtual void Accept(NodeVisitor& visitor) override;

		private:
			void Create();
			void Destroy();

			GeometricPrimitive* mpGeometry;

			bool_t mIsLit;
		};
	}
}

#endif // GRAPHICS_SCENE_GRAPH_GEOMETRY_NODE_HPP