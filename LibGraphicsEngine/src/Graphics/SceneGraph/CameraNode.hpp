#ifndef GRAPHICS_SCENE_GRAPH_CAMERA_NODE_HPP
#define GRAPHICS_SCENE_GRAPH_CAMERA_NODE_HPP

#include "Node.hpp"
#include <list>
#include <string>
#include <functional>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Camera;

		class CameraNode : public Node
		{
			GE_RTTI(GraphicsEngine::Graphics::CameraNode)

		public:
			CameraNode();
			explicit CameraNode(const std::string& name);
			virtual ~CameraNode();

			virtual void Visit(std::function<void(Node*)> callback) override;

		private:
			void Create();
			void Destroy();

			Camera* mpCamera;

			bool_t mIsMainCamera;

			//TODO - add culling stuff
		};
	}
}

#endif // GRAPHICS_SCENE_GRAPH_CAMERA_NODE_HPP