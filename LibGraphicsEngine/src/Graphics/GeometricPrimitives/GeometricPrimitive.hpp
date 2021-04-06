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
			enum class PrimitiveTopology : uint8_t
			{
				GE_PT_POINT_LIST = 0,
				GE_PT_LINE_LIST,
				GE_PT_LINE_LOOP,
				GE_PT_LINE_STRIP,
				GE_PT_TRIANGLE_LIST,
				GE_PT_TRIANGLE_STRIP,
				GE_PT_TRIANGLE_FAN,
				GE_PT_PATCH_LIST,
				GE_PT_COUNT
			};

			// front face winding
			enum class FaceWinding : uint8_t
			{
				GE_FW_CLOCKWISE = 0,
				GE_FW_COUNTER_CLOCKWISE,
				GE_FW_COUNT
			};

			enum class PolygonMode : uint8_t
			{
				GE_PM_FILL = 0,
				GE_PM_LINE,
				GE_PM_POINT,
				GE_PM_COUNT
			};

			GeometricPrimitive();
			explicit GeometricPrimitive(PrimitiveTopology topology, FaceWinding faceWinding, PolygonMode polygonMode);
			virtual ~GeometricPrimitive();

			const GeometricPrimitive::PrimitiveTopology& GetTopology() const;
			void SetTopology(const GeometricPrimitive::PrimitiveTopology& topology);

			const GeometricPrimitive::FaceWinding& GetFaceWinding() const;
			void SetFaceWinding(const GeometricPrimitive::FaceWinding& faceWinding);

			const GeometricPrimitive::PolygonMode& GetPolygonMode() const;
			void SetPolygonMode(const GeometricPrimitive::PolygonMode& polygonMode);

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

			PrimitiveTopology mTopology;
			FaceWinding mFaceWinding;
			PolygonMode mPolygonMode;

			VertexBuffer* mpVertexBuffer;
			IndexBuffer* mpIndexBuffer;

			bool_t mIsModel;
		};
	}
}
#endif // GRAPHICS_GEOMETRIC_PRIMITIVES_GEOMETRIC_PRIMITIVE_HPP