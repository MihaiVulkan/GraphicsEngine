#include "VertexFormat.hpp"
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
		mAttributes[Attribute::POSITION] = position;
	}

	if (normal > 0)
	{
		mAttributes[Attribute::NORMAL] = normal;
	}

	if (tangent > 0)
	{
		mAttributes[Attribute::TANGENT] = tangent;
	}

	if (color > 0)
	{
		mAttributes[Attribute::COLOR] = color;
	}

	if (texCoord > 0)
	{
		mAttributes[Attribute::TEXTURE_COORD] = texCoord;
	}
}

void VertexFormat::Copy(const VertexFormat& format)
{
	if (this != &format)
	{
		mAttributes = format.GetAttributes();
	}
}

void VertexFormat::Move(VertexFormat&& format)
{
	if (this != &format)
	{
		mAttributes = format.GetAttributes();
	}
}

void VertexFormat::Destroy()
{
	mAttributes.clear();
}

// TODO - for now we work wih float data
const uint8_t& VertexFormat::GetAttributeStride(const VertexFormat::Attribute& att) const
{
	assert((att >= VertexFormat::Attribute::POSITION) && (att < VertexFormat::Attribute::COUNT));

	return (HasAttribute(att) ? (mAttributes.at(att) * sizeof(bfloat32_t)) : 0);
}

bool_t VertexFormat::HasAttribute(const VertexFormat::Attribute& att) const
{
	assert((att >= VertexFormat::Attribute::POSITION) && (att < VertexFormat::Attribute::COUNT));

	auto it = mAttributes.find(att);

	return (it != mAttributes.end());
}


uint32_t VertexFormat::GetAttributeOffset(const VertexFormat::Attribute& att) const
{
	uint32_t offset = 0;

	switch (att)
	{
	case Attribute::POSITION:
		offset = 0;
		break;
	case Attribute::NORMAL:
		offset = (HasAttribute(Attribute::POSITION) ? GetAttributeOffset(Attribute::POSITION) + GetAttributeStride(Attribute::POSITION) : GetAttributeOffset(Attribute::POSITION));
		break;
	case Attribute::TANGENT:
		offset = (HasAttribute(Attribute::NORMAL) ? GetAttributeOffset(Attribute::NORMAL) + GetAttributeStride(Attribute::NORMAL) : GetAttributeOffset(Attribute::NORMAL));
		break;
	case Attribute::COLOR:
		offset = (HasAttribute(Attribute::TANGENT) ? GetAttributeOffset(Attribute::TANGENT) + GetAttributeStride(Attribute::TANGENT) : GetAttributeOffset(Attribute::TANGENT));
		break;
	case Attribute::TEXTURE_COORD:
		offset = (HasAttribute(Attribute::COLOR) ? GetAttributeOffset(Attribute::COLOR) + GetAttributeStride(Attribute::COLOR) : GetAttributeOffset(Attribute::COLOR));
		break;
	case Attribute::COUNT:
	default:
		LOG_ERROR("Invalid vertex format attribute!");
	}

	return offset;
}

uint32_t VertexFormat::GetTotalStride() const
{
	uint32_t stride = 
		GetAttributeStride(Attribute::POSITION) +
	    GetAttributeStride(Attribute::NORMAL) +
		GetAttributeStride(Attribute::TANGENT) +
		GetAttributeStride(Attribute::COLOR) +
		GetAttributeStride(Attribute::TEXTURE_COORD);

	return stride;
}

const VertexFormat::AttributeMap& VertexFormat::GetAttributes() const
{
	return mAttributes;
}