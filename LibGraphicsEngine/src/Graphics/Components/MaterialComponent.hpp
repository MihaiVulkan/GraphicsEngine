#ifndef GRAPHICS_COMPONENTS_MATERIAL_COMPONENT_HPP
#define GRAPHICS_COMPONENTS_MATERIAL_COMPONENT_HPP

#include "NodeComponent.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Material;

		class MaterialComponent : public NodeComponent
		{
			GE_RTTI(GraphicsEngine::Graphics::MaterialComponent)

		public:
			MaterialComponent();
			virtual ~MaterialComponent();

			virtual void Start() override;
			virtual void Update(bfloat32_t deltaTime) override;

			const char_t* GetName() const override;

			Material* GetMaterial() const;
			void SetMaterial(Material* pMaterial);


		private:
			Material* mpMaterial;
		};
	}
}


#endif // GRAPHICS_COMPONENTS_MATERIAL_COMPONENT_HPP