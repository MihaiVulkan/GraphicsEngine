#ifndef GRAPHICS_COMPONENTS_VISUAL_COMPONENT_HPP
#define GRAPHICS_COMPONENTS_VISUAL_COMPONENT_HPP

#include "Graphics/Components/NodeComponent.hpp"
#include <unordered_map>
#include <vector>

namespace GraphicsEngine
{
	namespace Graphics
	{		
		class VisualEffect;

		/* VisualComponent is the base class for all visual node components 
		   All redenderable/visual elements must have this component present. */
		class VisualComponent : public NodeComponent
		{
			GE_RTTI(GraphicsEngine::Graphics::VisualComponent)

		public:
			VisualComponent();
			explicit VisualComponent(const std::string& name);
			virtual ~VisualComponent();

			virtual void Start() override;
			virtual void Update(float32_t deltaTime) override;

			virtual void OnAttach() override;
			virtual void OnDettach() override;

			VisualEffect* GetVisualEffect();
			void SetVisualEffect(VisualEffect* pEffect);

		private:
			NO_COPY_NO_MOVE_CLASS(VisualComponent)

			void Create();
			void Destroy();

			VisualEffect* mpVisualEffect;
		};
	}
}


#endif // GRAPHICS_COMPONENTS_VISUAL_COMPONENT_HPP