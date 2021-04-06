#ifndef GRAPHICS_RENDERING_RESOURCES_MODEL_HPP
#define GRAPHICS_RENDERING_RESOURCES_MODEL_HPP

#include "Graphics/Rendering/Resources/Resource.hpp"
#include "Graphics/GeometricPrimitives/GeometricPrimitive.hpp"
#include "Graphics/Loaders/glTF2Loader.hpp"
#include <string>
#include <functional>

namespace GraphicsEngine
{
	namespace Graphics
	{
		//class glTF2Loader;

		// Model - used to store model data to pass to a specific Graphics API
		class Model: public Resource, public GeometricPrimitive
		{
			GE_RTTI(GraphicsEngine::Graphics::Model)

		public:
			Model();
			explicit Model(const std::string& filePath, glTF2Loader::FileLoadingFlags loadingFlags = glTF2Loader::FileLoadingFlags::None);
			virtual ~Model();

			void Draw(std::function<void(uint32_t indexCount, uint32_t firstIndex)> onDrawCB);

		private:
			NO_COPY_NO_MOVE_CLASS(Model)

			void Create();
			void Destroy();

			glTF2Loader* mpLoader;
		};

	}
}

#endif // GRAPHICS_RENDERING_RESOURCES_MODEL_HPP