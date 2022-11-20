#ifndef GRAPHICS_RENDERING_BACKENDS_OPENGL_RESOURCES_OPENGL_VERTEX_FORMAT_HPP
#define GRAPHICS_RENDERING_BACKENDS_OPENGL_RESOURCES_OPENGL_VERTEX_FORMAT_HPP

#if defined(OPENGL_RENDERER)
#include "Graphics/Rendering/Backends/OpenGL/Resources/OpenGLResource.hpp"
#include "Graphics/Rendering/Resources/VertexFormat.hpp"
#include <map>

namespace GraphicsEngine
{
	namespace Graphics
	{
		class Renderer;

		// OpenGL implementation of the Graphics API Dependent Resource
		class GADRVertexFormat : public GADRResource
		{
			GE_RTTI(GraphicsEngine::Graphics::GADRVertexFormat)

		public:
			typedef struct VertexAttributeDescription
			{
				GLuint location;
				GLint size; // 1, 2, 3, 4
				GLenum type;
				GLsizei stride;
				GLsizei offset;
			} VertexAttributeDescription;

			// NOTE! ordered_map as we want to maintain the order of attributes!
			typedef std::map<VertexFormat::VertexAttribute, GADRVertexFormat::VertexAttributeDescription> InputAttributeMap;

			GADRVertexFormat();
			explicit GADRVertexFormat(Renderer* pRenderer, VertexFormat* pVertexFormat);
			virtual ~GADRVertexFormat();

			const GADRVertexFormat::InputAttributeMap& GetInputAttributes() const;

			const VertexFormat::VertexInputRate& GetVertexInputRate() const;

		private:
			void Create();
			void Destroy();

			VertexFormat* mpVertexFormat;
			InputAttributeMap mInputAttributeMap;
		};
	}
}
#endif // OPENGL_RENDERER

#endif // GRAPHICS_RENDERING_BACKENDS_OPENGL_RESOURCES_OPENGL_VERTEX_FORMAT_HPP