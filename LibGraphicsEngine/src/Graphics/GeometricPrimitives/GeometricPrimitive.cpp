#include "Graphics/GeometricPrimitives/GeometricPrimitive.hpp"
#include "Graphics/Rendering/Resources/VertexFormat.hpp"
#include "Graphics/Rendering/Resources/VertexBuffer.hpp"
#include "Graphics/Rendering/Resources/IndexBuffer.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

GeometricPrimitive::GeometricPrimitive()
	: mTopology(PrimitiveTopology::GE_PT_COUNT)
	, mFaceWinding(FaceWinding::GE_FW_COUNT)
	, mPolygonMode(PolygonMode::GE_PM_COUNT)
	, mpVertexBuffer(nullptr)
	, mpIndexBuffer(nullptr)
{}

GeometricPrimitive::GeometricPrimitive(PrimitiveTopology topology, FaceWinding faceWinding, PolygonMode polygonMode)
	: mTopology(topology)
	, mFaceWinding(faceWinding)
	, mPolygonMode(polygonMode)
	, mpVertexBuffer(nullptr)
	, mpIndexBuffer(nullptr)
{}

GeometricPrimitive::~GeometricPrimitive()
{
	if (mpIndexBuffer)
	{
		mpIndexBuffer = nullptr;
	}

	if (mpVertexBuffer)
	{
		mpVertexBuffer = nullptr;
	}

	mTopology = PrimitiveTopology::GE_PT_COUNT;
	mFaceWinding = FaceWinding::GE_FW_COUNT;
	mPolygonMode = PolygonMode::GE_PM_COUNT;
}

const GeometricPrimitive::PrimitiveTopology& GeometricPrimitive::GetTopology() const
{
	return mTopology;
}

void GeometricPrimitive::SetTopology(const GeometricPrimitive::PrimitiveTopology& topology)
{
	mTopology = topology;
}

const GeometricPrimitive::FaceWinding& GeometricPrimitive::GetFaceWinding() const
{
	return mFaceWinding;
}

void GeometricPrimitive::SetFaceWinding(const GeometricPrimitive::FaceWinding& faceWinding)
{
	mFaceWinding = faceWinding;
}

const GeometricPrimitive::PolygonMode& GeometricPrimitive::GetPolygonMode() const
{
	return mPolygonMode;
}

void GeometricPrimitive::SetPolygonMode(const GeometricPrimitive::PolygonMode& polygonMode)
{
	mPolygonMode = polygonMode;
}

VertexFormat* GeometricPrimitive::GetVertexFormat() const
{
	assert(mpVertexBuffer != nullptr);

	return mpVertexBuffer->GetFormat();
}

VertexBuffer* GeometricPrimitive::GetVertexBuffer() const
{
	return mpVertexBuffer;
}

IndexBuffer* GeometricPrimitive::GetIndexBuffer() const
{
	return mpIndexBuffer;
}

void GeometricPrimitive::SetVertexBuffer(VertexBuffer* pVertexBuffer)
{
	assert(pVertexBuffer != nullptr);

	mpVertexBuffer = pVertexBuffer;
}

void GeometricPrimitive::SetIndexBuffer(IndexBuffer* pIndexBuffer)
{
	assert(pIndexBuffer != nullptr);

	mpIndexBuffer = pIndexBuffer;
}

bool_t GeometricPrimitive::IsIndexed() const
{
	return (mpIndexBuffer != nullptr);
}