#ifndef GRAPHICS_RENDERING_BACKENDS_OPENGL_RESOURCES_OPENGL_MODEL_HPP
#define GRAPHICS_RENDERING_BACKENDS_OPENGL_RESOURCES_OPENGL_MODEL_HPP

#if defined(OPENGL_RENDERER)
#include "Graphics/Rendering/Backends/OpenGL/Resources/OpenGLResource.hpp"
#include <functional>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Renderer;
		class Model;

		// OpenGL implementation of the Graphics API Dependent Resource
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
			void Create();
			void Destroy();

			Model* mpModel;
		};
	}
}
#endif // OPENGL_RENDERER

#endif // GRAPHICS_RENDERING_BACKENDS_OPENGL_RESOURCES_OPENGL_MODEL_HPP