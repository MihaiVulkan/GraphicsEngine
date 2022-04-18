#ifndef GRAPHICS_GEOMETRIC_PRIMITIVES_GEOMETRIC_PRIMITIVE_HPP
#define GRAPHICS_GEOMETRIC_PRIMITIVES_GEOMETRIC_PRIMITIVE_HPP

#include "Foundation/Object.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		// common Rendering Resources
		class VertexFormat;
		class VertexBuffer;
		class IndexBuffer;

		/* Base class for gemetric primitives */
		class GeometricPrimitive : public Object
		{
			GE_RTTI(GraphicsEngine::Graphics::GeometricPrimitive)

		public:
			GeometricPrimitive();
			virtual ~GeometricPrimitive();

			VertexFormat* GetVertexFormat() const;
			VertexBuffer* GetVertexBuffer() const;
			IndexBuffer* GetIndexBuffer() const;

			void SetVertexBuffer(VertexBuffer* pVertexBuffer);
			void SetIndexBuffer(IndexBuffer* pIndexBuffer);

			bool_t IsIndexed() const;

			void SetIsModel(bool_t val);
			bool_t IsModel();

		private:
			NO_COPY_NO_MOVE_CLASS(GeometricPrimitive)

			VertexBuffer* mpVertexBuffer;
			IndexBuffer* mpIndexBuffer;

			bool_t mIsModel;
		};
	}
}
#endif // GRAPHICS_GEOMETRIC_PRIMITIVES_GEOMETRIC_PRIMITIVE_HPP