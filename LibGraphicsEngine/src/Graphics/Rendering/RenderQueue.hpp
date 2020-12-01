#ifndef GRAPHICS_RENDERING_RENDER_QUEUE_HPP
#define GRAPHICS_RENDERING_RENDER_QUEUE_HPP

#include "Foundation/TypeDefs.hpp"
#include "Foundation/RTTI.hpp"
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

		class RenderQueue : public RTTI
		{
			GE_RTTI(GraphicsEngine::Graphics::RenderQueue)

		public:
			struct Renderable
			{
				GeometryNode* pGeometryNode;
				Material* pMaterial;
				glm::mat4 modelTransform;
				bfloat64_t distanceFromCamera;
			};

			enum class RenderableType : uint8_t
			{
				RT_BACKGROUND = 0,
				RT_OCCLUDER,
				RT_SHADOW_CASTER,
				RT_OPAQUE,
				RT_TRANSLUCENT,
				RT_SCREEN,
				RT_DEBUG,
				RT_COUNT
			};

			typedef std::list<Renderable> RenderableList;

			RenderQueue();
			virtual ~RenderQueue();

			void Push(Node* pNode);

			RenderQueue::RenderableList* GetRenderables(const RenderQueue::RenderableType& type);

			void Each(RenderQueue::RenderableList* pRenderableList, std::function< void(RenderQueue::Renderable*) > callback);
			//void Each(std::function< void(Light*, int) > callback);



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