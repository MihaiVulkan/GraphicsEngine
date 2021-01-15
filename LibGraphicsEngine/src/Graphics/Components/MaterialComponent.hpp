#ifndef GRAPHICS_COMPONENTS_MATERIAL_COMPONENT_HPP
#define GRAPHICS_COMPONENTS_MATERIAL_COMPONENT_HPP

#include "Graphics/Components/NodeComponent.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Material;

		/* MaterialComponent used for the nodes who want to have materials */
		class MaterialComponent : public NodeComponent
		{
			GE_RTTI(GraphicsEngine::Graphics::MaterialComponent)

		public:
			MaterialComponent();
			explicit MaterialComponent(const std::string& name);
			virtual ~MaterialComponent();

			virtual void Start() override;
			virtual void Update(float32_t deltaTime) override;

			Material* GetMaterial() const;
			void SetMaterial(Material* pMaterial);


		private:
			void Create();
			void Destroy();

			Material* mpMaterial;
		};
	}
}


#endif // GRAPHICS_COMPONENTS_MATERIAL_COMPONENT_HPP