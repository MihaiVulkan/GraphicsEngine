#ifndef GRAPHICS_RENDERING_RENDER_QUEUE_HPP
#define GRAPHICS_RENDERING_RENDER_QUEUE_HPP

#include "Foundation/Object.hpp"
#include "glm/mat4x4.hpp"
#include <vector>
#include <unordered_map>
#include <functional>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Camera;
		class Node;
		class GeometryNode;
		class LightNode;
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

			typedef std::vector<Renderable> RenderableCollection;

			RenderQueue();
			virtual ~RenderQueue();

			void Push(GeometryNode* pGeoNode);
			void Push(LightNode* pLightNode);

			const RenderQueue::RenderableCollection& GetRenderables(const RenderQueue::RenderableType& type) const;

			void ForEach(const RenderQueue::RenderableCollection& renderables, std::function< void(const RenderQueue::Renderable*) > callback);
			void ForEach(std::function< void(const LightNode*) > callback);



			Camera* GetCamera() const;
			void SetCamera(Camera* pCamera);

		private:
			void Destroy();

			Camera* mpCamera;

			std::unordered_map<RenderableType, RenderableCollection, std::hash<RenderableType>> mRenderables;
			std::vector<LightNode*> mLights;

		};
	}
}

#endif // GRAPHICS_RENDERING_RENDER_QUEUE_HPP