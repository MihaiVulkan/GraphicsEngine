#ifndef GRAPHICS_GEOMETRIC_PRIMITIVES_GEOMETRIC_PRIMITIVE_HPP
#define GRAPHICS_GEOMETRIC_PRIMITIVES_GEOMETRIC_PRIMITIVE_HPP

#include "Foundation/TypeDefs.hpp"
#include "Foundation/RTTI.hpp"

namespace GraphicsEngine
{
	namespace Graphics
	{
		// common Rendering Resources
		class VertexFormat;
		class VertexBuffer;
		class IndexBuffer;

		class GeometricPrimitive : public RTTI
		{
			GE_RTTI(GraphicsEngine::Graphics::GeometricPrimitive)

		public:
			enum class PrimitiveTopology : uint8_t
			{
				PT_POINT_LIST = 0,
				PT_LINE_LIST,
				PT_LINE_LOOP,
				PT_LINE_STRIP,
				PT_TRIANGLE_LIST,
				PT_TRIANGLE_STRIP,
				PT_TRIANGLE_FAN,
				PT_PATCH_LIST,
				PT_COUNT
			};

			GeometricPrimitive();
			explicit GeometricPrimitive(PrimitiveTopology topology);
			virtual ~GeometricPrimitive();

			GeometricPrimitive::PrimitiveTopology GetTopology() const;

			VertexFormat* GetVertexFormat() const;
			VertexBuffer* GetVertexBuffer() const;
			IndexBuffer* GetIndexBuffer() const;

			void SetVertexBuffer(VertexBuffer* pVertexBuffer);
			void SetIndexBuffer(IndexBuffer* pIndexBuffer);

			bool_t IsIndexed() const;

		private:
			PrimitiveTopology mTopology;

			VertexBuffer* mpVertexBuffer;
			IndexBuffer* mpIndexBuffer;
		};
	}
}
#endif // GRAPHICS_GEOMETRIC_PRIMITIVES_GEOMETRIC_PRIMITIVE_HPP