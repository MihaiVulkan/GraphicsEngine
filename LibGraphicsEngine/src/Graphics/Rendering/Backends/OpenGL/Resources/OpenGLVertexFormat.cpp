#if defined(OPENGL_RENDERER)
#include "Graphics/Rendering/Backends/OpenGL/Resources/OpenGLVertexFormat.hpp"
#include <cassert>


using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;


GADRVertexFormat::GADRVertexFormat()
	: mpVertexFormat(nullptr)
{}

GADRVertexFormat::GADRVertexFormat(Renderer* pRenderer, VertexFormat* pVertexFormat)
	: mpVertexFormat(pVertexFormat)
{
	Create();
}

GADRVertexFormat::~GADRVertexFormat()
{
	Destroy();
}

void GADRVertexFormat::Create()
{
	assert(mpVertexFormat != nullptr);

	auto& attributes = mpVertexFormat->GetVertexAttributes();

	assert(attributes.empty() == false);

	size_t index = 0;
	for (auto iter = attributes.begin(); iter != attributes.end(); ++iter)
	{
		GADRVertexFormat::VertexAttributeDescription attributeDesc{};
		attributeDesc.location = 0; // NOTE! Has to be updated later based on vertex shader input data~
		attributeDesc.type = GL_FLOAT;
		attributeDesc.stride = mpVertexFormat->GetVertexTotalStride();
		attributeDesc.offset = mpVertexFormat->GetVertexAttributeOffset(iter->first);
		attributeDesc.size = iter->second;

		mInputAttributeMap[iter->first] = attributeDesc;
	}
}

void GADRVertexFormat::Destroy()
{
	if (mpVertexFormat)
	{
		mpVertexFormat = nullptr;
	}

	mInputAttributeMap.clear();
}

const GADRVertexFormat::InputAttributeMap& GADRVertexFormat::GetInputAttributes() const
{
	return mInputAttributeMap;
}

const VertexFormat::VertexInputRate& GADRVertexFormat::GetVertexInputRate() const
{
	assert(mpVertexFormat != nullptr);

	return mpVertexFormat->GetVertexInputRate();
}
#endif // OPENGL_RENDERER