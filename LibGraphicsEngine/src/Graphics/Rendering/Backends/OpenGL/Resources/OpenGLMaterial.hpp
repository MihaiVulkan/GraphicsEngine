#ifndef GRAPHICS_RENDERING_BACKENDS_OPENGL_RESOURCES_OPENGL_MATERIAL_HPP
#define GRAPHICS_RENDERING_BACKENDS_OPENGL_RESOURCES_OPENGL_MATERIAL_HPP

#if defined(OPENGL_RENDERER)
#include "Graphics/Rendering/Backends/OpenGL/Resources/OpenGLResource.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Renderer;

		class Material;

		// OpenGL implementation of the Graphics API Dependent Resource
		// INFO : basic default material
		class GADRMaterial : public GADRResource
		{
			GE_RTTI(GraphicsEngine::Graphics::GADRMaterial)

		public:
			GADRMaterial();
			explicit GADRMaterial(Renderer* pRenderer, Material* pMaterial);
			virtual ~GADRMaterial();

			void Bind(uint32_t currentBufferIdx);


		private:
			void Create();
			void Destroy();

			Material* mpMaterial;
		};
	}
}
#endif // OPENGL_RENDERER

#endif // GRAPHICS_RENDERING_BACKENDS_OPENGL_RESOURCES_OPENGL_MATERIAL_HPP