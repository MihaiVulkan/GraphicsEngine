#ifndef GRAPHICS_RENDERING_BACKENDS_VULKAN_RESOURCES_VULKAN_MODEL_HPP
#define GRAPHICS_RENDERING_BACKENDS_VULKAN_RESOURCES_VULKAN_MODEL_HPP

#if defined(VULKAN_RENDERER)
#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanResource.hpp"
#include <functional>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Renderer;
		class VulkanRenderer;

		class Model;

		// Vulkan implementation of the Graphics API Dependent Resource
		// INFO : basic model
		class GADRModel : public GADRResource
		{
			GE_RTTI(GraphicsEngine::Graphics::GADRModel)

		public:
			GADRModel();
			explicit GADRModel(Renderer* pRenderer, Model* pModel);
			virtual ~GADRModel();

			void Draw(std::function<void(uint32_t indexCount, uint32_t firstIndex)> onDrawCB);


		private:
			void Create(Renderer* pRenderer);
			void Destroy();

			Model* mpModel;
		};
	}
}
#endif // VULKAN_RENDERER

#endif // GRAPHICS_RENDERING_BACKENDS_VULKAN_RESOURCES_VULKAN_MODEL_HPP