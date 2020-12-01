#include "GeometricPrimitive.hpp"
#include "Graphics/Rendering/Resources/VertexFormat.hpp"
#include "Graphics/Rendering/Resources/VertexBuffer.hpp"
#include "Graphics/Rendering/Resources/IndexBuffer.hpp"

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

GeometricPrimitive::GeometricPrimitive()
	: mTopology(PrimitiveTopology::PT_COUNT)
	, mpVertexBuffer(nullptr)
	, mpIndexBuffer(nullptr)
{}

GeometricPrimitive::GeometricPrimitive(PrimitiveTopology topology)
	: mTopology(topology)
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

	mTopology = PrimitiveTopology::PT_COUNT;
}

GeometricPrimitive::PrimitiveTopology GeometricPrimitive::GetTopology() const
{
	return mTopology;
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