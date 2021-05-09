#include "Graphics/Rendering/Resources/UniformBuffer.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"
#include <cstring> // ::memcpy()
#include <cassert>

using namespace GraphicsEngine;
using namespace GraphicsEngine::Graphics;


UniformBuffer::UniformBuffer()
	: Buffer()
	, mIsUniformMapSizeChanged(false)
	, mIsUniformMapUpdated(false)
{
	Create();
}


UniformBuffer::~UniformBuffer()
{
	Destroy();
}

void UniformBuffer::Create()
{

}

void UniformBuffer::Destroy()
{
	mUniformMap.clear();

	Buffer::Destroy();
}

void UniformBuffer::AddUniform(GLSLShaderTypes::UniformType type)
{
	assert(type < GLSLShaderTypes::UniformType::GE_UT_COUNT);

	std::size_t oldMapSize = mUniformMap.size();

	auto& ref = mUniformMap[type];

	mIsUniformMapSizeChanged = (oldMapSize != mUniformMap.size());

	switch (type)
	{
	case GLSLShaderTypes::UniformType::GE_UT_MODEL_MATRIX3:
	case GLSLShaderTypes::UniformType::GE_UT_NORMAL_MATRIX3:
		ref = Variant(Variant::VariantType::GE_VT_MAT3);
		break;
	case GLSLShaderTypes::UniformType::GE_UT_MODEL_MATRIX4:
	case GLSLShaderTypes::UniformType::GE_UT_VIEW_MATRIX4:
	case GLSLShaderTypes::UniformType::GE_UT_PROJECTION_MATRIX4:
	case GLSLShaderTypes::UniformType::GE_UT_PVM_MATRIX4:
		ref = Variant(Variant::VariantType::GE_VT_MAT4);
		break;
	case GLSLShaderTypes::UniformType::GE_UT_CAMERA_POS:
		ref = Variant(Variant::VariantType::GE_VT_VEC3);
		break;
	case GLSLShaderTypes::UniformType::GE_UT_CRR_TIME:
		ref = Variant(Variant::VariantType::GE_VT_FLOAT32);
		break;
	case GLSLShaderTypes::UniformType::GE_UT_LIGHT_DIR:
	case GLSLShaderTypes::UniformType::GE_UT_LIGHT_COLOR:
		ref = Variant(Variant::VariantType::GE_VT_VEC3);
		break;
	case GLSLShaderTypes::UniformType::GE_UT_COUNT:
	default:
		LOG_ERROR("Invalid uniform type!");
		return;
	}

	mSize += ref.Size();
}

Variant UniformBuffer::GetUniform(GLSLShaderTypes::UniformType type) const
{
	assert(type < GLSLShaderTypes::UniformType::GE_UT_COUNT);

	auto iter = mUniformMap.find(type);
	if (iter != mUniformMap.end())
	{
		return iter->second;
	}

	LOG_ERROR("Uniform type not found!");
	return Variant();
}

bool_t UniformBuffer::HasUniform(GLSLShaderTypes::UniformType type) const
{
	assert(type < GLSLShaderTypes::UniformType::GE_UT_COUNT);

	auto iter = mUniformMap.find(type);
	if (iter != mUniformMap.end())
	{
		return true;
	}

	return false;
}

void* UniformBuffer::GetData()
{
	// create a contiguos memory zone of concatenated data - as the pipeline expects it to be!
	// allocate a new zone only if the uniform map has changed
	if (mpData && mIsUniformMapSizeChanged)
	{
		GE_FREE_ARRAY(mpData);
	}

	if (mpData == nullptr)
	{
		assert(mSize > 0);

		mpData = GE_ALLOC_ARRAY(uint8_t, mSize);
		assert(mpData != nullptr);
	}

	// repopulate mpData only if needed
	if (mpData && (mIsUniformMapSizeChanged || mIsUniformMapUpdated))
	{
		std::size_t offset = 0;
		for (auto iter = mUniformMap.begin(); iter != mUniformMap.end(); ++iter)
		{
			auto& type = iter->first;
			auto& variant = iter->second;

			switch (type)
			{
			case GLSLShaderTypes::UniformType::GE_UT_MODEL_MATRIX3:
			case GLSLShaderTypes::UniformType::GE_UT_NORMAL_MATRIX3:
			{
				auto& ref = variant.Value<glm::mat3>();
				::memcpy(mpData + offset, &ref, variant.Size());

				offset += variant.Size();
			}
			break;
			case GLSLShaderTypes::UniformType::GE_UT_MODEL_MATRIX4:
			case GLSLShaderTypes::UniformType::GE_UT_VIEW_MATRIX4:
			case GLSLShaderTypes::UniformType::GE_UT_PROJECTION_MATRIX4:
			case GLSLShaderTypes::UniformType::GE_UT_PVM_MATRIX4:
			{
				auto& ref = variant.Value<glm::mat4>();
				::memcpy(mpData + offset, &ref, variant.Size());

				offset += variant.Size();
			}
			break;
			case GLSLShaderTypes::UniformType::GE_UT_LIGHT_DIR:
			case GLSLShaderTypes::UniformType::GE_UT_LIGHT_COLOR:
			case GLSLShaderTypes::UniformType::GE_UT_CAMERA_POS:
			{
				auto& ref = variant.Value<glm::vec3>();
				::memcpy(mpData + offset, &ref, variant.Size());

				offset += variant.Size();
			}
			break;
			case GLSLShaderTypes::UniformType::GE_UT_CRR_TIME:
			{
				auto& ref = variant.Value<float32_t>();
				::memcpy(mpData + offset, &ref, variant.Size());

				offset += variant.Size();
			}
			break;
			case GLSLShaderTypes::UniformType::GE_UT_COUNT:
			default:
				LOG_ERROR("Invalid uniform type! Failed to get data!");
				return nullptr;
			}
		}

		if (mIsUniformMapUpdated)
			mIsUniformMapUpdated = false;
		if (mIsUniformMapSizeChanged)
			mIsUniformMapSizeChanged = false;
	}

	return mpData;
}