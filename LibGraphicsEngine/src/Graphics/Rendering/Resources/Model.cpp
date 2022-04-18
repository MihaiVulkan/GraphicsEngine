#include "Graphics/Rendering/Resources/Model.hpp"
#include "Graphics/Rendering/Resources/VertexFormat.hpp"
#include "Graphics/Rendering/Resources/VertexBuffer.hpp"
#include "Graphics/Rendering/Resources/IndexBuffer.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;


Model::Model()
	: GeometricPrimitive()
	, mpLoader(nullptr)
{}

Model::Model(const std::string& filePath, uint32_t loadingFlags)
	: Model()
{
	Create(filePath, loadingFlags);
}

Model::~Model()
{
	Destroy();
}

void Model::Create(const std::string& filePath, uint32_t loadingFlags)
{
	mpLoader = GE_ALLOC(glTF2Loader)(filePath, loadingFlags);
	assert(mpLoader != nullptr);

	auto& vertexAttribs = mpLoader->GetVertexAttributes();

	auto& vertexBuf = mpLoader->GetVertexBuffer();
	assert(false == vertexBuf.empty());

	auto* pVertexFormat = GE_ALLOC(VertexFormat)(vertexAttribs.pos, vertexAttribs.normal,
		vertexAttribs.tangent, vertexAttribs.color, vertexAttribs.uv);
	assert(pVertexFormat != nullptr);

	auto* pVertexBuffer = GE_ALLOC(VertexBuffer)(pVertexFormat, Buffer::BufferUsage::GE_BU_STATIC, (void*)vertexBuf.data(), vertexBuf.size() * sizeof(float32_t));
	assert(pVertexBuffer != nullptr);
	SetVertexBuffer(pVertexBuffer);

	auto& indexBuff = mpLoader->GetIndexBuffer();
	assert(false == indexBuff.empty());

	auto* pIndexBuffer = GE_ALLOC(IndexBuffer)(IndexBuffer::BufferUsage::GE_BU_STATIC, IndexBuffer::IndexType::GE_IT_UINT32, (void*)indexBuff.data(), indexBuff.size() * sizeof(uint32_t));

	SetIndexBuffer(pIndexBuffer);

	// Default face winding
	// GL - CCW
	// Vulkan - CW

	SetIsModel(true); //TODO - for now used for rendering submeshes
}

void Model::Destroy()
{
	GE_FREE(mpLoader);
}

void Model::Draw(std::function<void(uint32_t indexCount, uint32_t firstIndex)> onDrawCB)
{
	assert(mpLoader != nullptr);

	mpLoader->Draw(onDrawCB);
}