#include "Graphics/Rendering/Resources/VertexFormat.hpp"
#include "Foundation/Logger.hpp"
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;

//const VertexFormat VertexFormat::VF_P3(3, 0, 0, 0, 0);
//const VertexFormat VertexFormat::VF_P3_N3(3, 3, 0, 0, 0);
//const VertexFormat VertexFormat::VF_P3_C3(3, 0, 0, 3, 0);
//const VertexFormat VertexFormat::VF_P3_UV2(3, 0, 0, 0, 2);
//const VertexFormat VertexFormat::VF_P3_N3_UV2(3, 3, 0, 0, 2);
//const VertexFormat VertexFormat::VF_P3_C3_UV2(3, 0, 0, 3, 2);
//const VertexFormat VertexFormat::VF_P3_N3_C3_UV2(3, 3, 0, 3, 2);
//const VertexFormat VertexFormat::VF_P3_N3_TG3(3, 3, 3, 0, 0);
//const VertexFormat VertexFormat::VF_P3_N3_TG3_C3(3, 3, 3, 3, 0);
//const VertexFormat VertexFormat::VF_P3_N3_TG3_UV2(3, 3, 3, 0, 2);
//const VertexFormat VertexFormat::VF_P3_N3_TG3_C3_UV2(3, 3, 3, 3, 2);


VertexFormat::VertexFormat()
{}

VertexFormat::VertexFormat(uint8_t position, uint8_t normal, uint8_t tangent, uint8_t color, uint8_t texCoord)
{
	Create(position, normal, tangent, color, texCoord);
}

VertexFormat::VertexFormat(const VertexFormat& format)
{
	Copy(format);
}

VertexFormat::VertexFormat(VertexFormat&& format)
{
	//Move(format); //TODO
}

VertexFormat& VertexFormat::operator =(const VertexFormat& format)
{
	Copy(format);

	return *this;
}

VertexFormat& VertexFormat::operator =(VertexFormat&& format)
{
	//Move(format); //TODO

	return *this;
}

VertexFormat::~VertexFormat()
{
	Destroy();
}

void VertexFormat::Create(uint8_t position, uint8_t normal, uint8_t tangent, uint8_t color, uint8_t texCoord)
{
	if (position > 0)
	{
		mVertexAttributes[VertexAttribute::GE_VA_POSITION] = position;
	}

	if (normal > 0)
	{
		mVertexAttributes[VertexAttribute::GE_VA_NORMAL] = normal;
	}

	if (tangent > 0)
	{
		mVertexAttributes[VertexAttribute::GE_VA_TANGENT] = tangent;
	}

	if (color > 0)
	{
		mVertexAttributes[VertexAttribute::GE_VA_COLOR] = color;
	}

	if (texCoord > 0)
	{
		mVertexAttributes[VertexAttribute::GE_VA_TEXTURE_COORD] = texCoord;
	}
}

void VertexFormat::Copy(const VertexFormat& format)
{
	if (this != &format)
	{
		mVertexAttributes = format.GetVertexAttributes();
	}
}

void VertexFormat::Move(VertexFormat&& format)
{
	if (this != &format)
	{
		mVertexAttributes = format.GetVertexAttributes();
	}
}

void VertexFormat::Destroy()
{
	mVertexAttributes.clear();
}

// TODO - for now we work wih float data as iit is the most inclusive attribute type
uint32_t VertexFormat::GetVertexAttributeStride(const VertexFormat::VertexAttribute& att) const
{
	assert((att >= VertexFormat::VertexAttribute::GE_VA_POSITION) && (att < VertexFormat::VertexAttribute::GE_VA_COUNT));

	uint32_t attributeStride = (HasVertexAttribute(att) ? (mVertexAttributes.at(att) * sizeof(float32_t)) : 0);

	return attributeStride;
}

bool_t VertexFormat::HasVertexAttribute(const VertexFormat::VertexAttribute& att) const
{
	assert((att >= VertexFormat::VertexAttribute::GE_VA_POSITION) && (att < VertexFormat::VertexAttribute::GE_VA_COUNT));

	auto it = mVertexAttributes.find(att);

	return (it != mVertexAttributes.end());
}


uint32_t VertexFormat::GetVertexAttributeOffset(const VertexFormat::VertexAttribute& att) const
{
	uint32_t offset = 0;

	switch (att)
	{
	case VertexAttribute::GE_VA_POSITION:
		offset = 0;
		break;
	case VertexAttribute::GE_VA_NORMAL:
		offset = (HasVertexAttribute(VertexAttribute::GE_VA_POSITION) ? GetVertexAttributeOffset(VertexAttribute::GE_VA_POSITION) + GetVertexAttributeStride(VertexAttribute::GE_VA_POSITION) : GetVertexAttributeOffset(VertexAttribute::GE_VA_POSITION));
		break;
	case VertexAttribute::GE_VA_TANGENT:
		offset = (HasVertexAttribute(VertexAttribute::GE_VA_NORMAL) ? GetVertexAttributeOffset(VertexAttribute::GE_VA_NORMAL) + GetVertexAttributeStride(VertexAttribute::GE_VA_NORMAL) : GetVertexAttributeOffset(VertexAttribute::GE_VA_NORMAL));
		break;
	case VertexAttribute::GE_VA_COLOR:
		offset = (HasVertexAttribute(VertexAttribute::GE_VA_TANGENT) ? GetVertexAttributeOffset(VertexAttribute::GE_VA_TANGENT) + GetVertexAttributeStride(VertexAttribute::GE_VA_TANGENT) : GetVertexAttributeOffset(VertexAttribute::GE_VA_TANGENT));
		break;
	case VertexAttribute::GE_VA_TEXTURE_COORD:
		offset = (HasVertexAttribute(VertexAttribute::GE_VA_COLOR) ? GetVertexAttributeOffset(VertexAttribute::GE_VA_COLOR) + GetVertexAttributeStride(VertexAttribute::GE_VA_COLOR) : GetVertexAttributeOffset(VertexAttribute::GE_VA_COLOR));
		break;
	case VertexAttribute::GE_VA_COUNT:
	default:
		LOG_ERROR("Invalid vertex format attribute!");
	}

	return offset;
}

uint32_t VertexFormat::GetVertexTotalStride() const
{
	uint32_t stride = 
		GetVertexAttributeStride(VertexAttribute::GE_VA_POSITION) +
	    GetVertexAttributeStride(VertexAttribute::GE_VA_NORMAL) +
		GetVertexAttributeStride(VertexAttribute::GE_VA_TANGENT) +
		GetVertexAttributeStride(VertexAttribute::GE_VA_COLOR) +
		GetVertexAttributeStride(VertexAttribute::GE_VA_TEXTURE_COORD);

	return stride;
}

const VertexFormat::VertexAttributeMap& VertexFormat::GetVertexAttributes() const
{
	return mVertexAttributes;
}