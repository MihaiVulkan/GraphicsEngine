#ifndef GRAPHICS_RENDERING_RESOURCES_MATERIAL_HPP
#define GRAPHICS_RENDERING_RESOURCES_MATERIAL_HPP

#include "Graphics/Rendering/Resources/Resource.hpp"

#include <vector>
#include <array>

#define COLOR_MAP 0
#define NORMAL_MAP 1
#define SPECULAR_MAP 2
#define EMISSIVE_MAP 3
#define MAP_COUNT 4

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Texture;
		//class Shader;

		// Material - used to store material data to pass to a specific Graphics API
		class Material : public Resource
		{
			GE_RTTI(GraphicsEngine::Graphics::Material)

		public:
			struct MaterialProperties
			{
				Color3f ambient;
				Color3f diffuse;
				Color3f specular;
				float32_t shininess;
				float32_t emissive;
			};

			typedef std::array<Texture*, MAP_COUNT> TextureArray;

			Material();
			explicit Material(const Material::MaterialProperties& properties);
			virtual ~Material();


		//	void SetShaders(const std::vector<Shader*>& shaders);
		//	const std::vector<Shader*>& GetShaders() const;

			const Material::MaterialProperties& GetMaterialProperties() const;

			const Material::TextureArray& GetTextures() const;

			//TODO - add per texture getter

		private:
			void Create();
			void Destroy();

		//	std::vector<Shader*> mShaders;
			TextureArray mTextures;

			MaterialProperties mProperties;

		};

	}
}

#endif // GRAPHICS_RENDERING_RESOURCES_MATERIAL_HPP