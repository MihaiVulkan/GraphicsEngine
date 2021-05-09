#ifndef GRAPHICS_SCENE_GRAPH_LIGHT_NODE_HPP
#define GRAPHICS_SCENE_GRAPH_LIGHT_NODE_HPP

#include "Graphics/SceneGraph/Node.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Light;

		class LightNode : public Node
		{
			GE_RTTI(GraphicsEngine::Graphics::LightNode)

		public:
			LightNode();
			explicit LightNode(const std::string& name);
			virtual ~LightNode();

			void SetLight(Light* pLight);
			Light* GetLight() const;

			///////// Visitor Pattern ///////
			virtual void Accept(NodeVisitor& visitor) override;

		private:
			void Create();
			void Destroy();

			Light* mpLight;
		};
	}
}

#endif // GRAPHICS_SCENE_GRAPH_LIGHT_NODE_HPP