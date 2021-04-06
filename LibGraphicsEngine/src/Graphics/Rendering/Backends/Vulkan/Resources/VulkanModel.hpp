#ifndef GRAPHICS_RENDERING_BACKENDS_VULKAN_RESOURCES_VULKAN_MODEL_HPP
#define GRAPHICS_RENDERING_BACKENDS_VULKAN_RESOURCES_VULKAN_MODEL_HPP

#include "Graphics/Rendering/Backends/Vulkan/Resources/VulkanResource.hpp"

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

			void Draw(uint32_t currentBufferIdx);


		private:
			void Create(Renderer* pRenderer);
			void Destroy();

			VulkanRenderer* mpRenderer;

			Model* mpModel;
		};
	}
}

#endif // GRAPHICS_RENDERING_BACKENDS_VULKAN_RESOURCES_VULKAN_MODEL_HPP