#ifndef GRAPHICS_RENDERING_RENDER_QUEUE_HPP
#define GRAPHICS_RENDERING_RENDER_QUEUE_HPP

#include "Foundation/Object.hpp"
#include "glm/mat4x4.hpp"
#include <list>
#include <unordered_map>
#include <functional>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Camera;
		class Node;
		class GeometryNode;
		class Material;

		class RenderQueue : public Object
		{
			GE_RTTI(GraphicsEngine::Graphics::RenderQueue)

		public:
			struct Renderable
			{
				GeometryNode* pGeometryNode;
				Material* pMaterial;
				//glm::mat4 modelTransform;
				//float64_t distanceFromCamera;
			};

			enum class RenderableType : uint8_t
			{
				GE_RT_BACKGROUND = 0,
				GE_RT_OCCLUDER,
				GE_RT_SHADOW_CASTER,
				GE_RT_OPAQUE,
				GE_RT_TRANSLUCENT,
				GE_RT_SCREEN,
				GE_RT_DEBUG,
				GE_RT_COUNT
			};

			typedef std::list<Renderable> RenderableList;

			RenderQueue();
			virtual ~RenderQueue();

			void Push(Node* pNode);

			void Push(GeometryNode* pGeoNode);
			//TODO - push other nodes

			RenderQueue::RenderableList* GetRenderables(const RenderQueue::RenderableType& type);

			void ForEach(RenderQueue::RenderableList* pRenderableList, std::function< void(RenderQueue::Renderable*) > callback);
			//void ForEach(std::function< void(Light*, int) > callback);



			Camera* GetCamera() const;
			void SetCamera(Camera* pCamera);

		private:
			void Destroy();

			Camera* mpCamera;

			std::unordered_map<RenderableType, RenderableList, std::hash<RenderableType>> mRenderables;
		};
	}
}

#endif // GRAPHICS_RENDERING_RENDER_QUEUE_HPP