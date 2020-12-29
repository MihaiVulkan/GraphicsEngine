#include "UniformBuffer.hpp"
#include "Foundation/MemoryManagement/MemoryOperations.hpp"
#include "Foundation/Logger.hpp"
#include <cstring> // memcpy()
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
	{
		ref.data = Variant(Variant::VariantType::GE_VT_MAT3);
		ref.size = sizeof(glm::mat3);
	}
		break;
	case GLSLShaderTypes::UniformType::GE_UT_MODEL_MATRIX4:
	case GLSLShaderTypes::UniformType::GE_UT_VIEW_MATRIX4:
	case GLSLShaderTypes::UniformType::GE_UT_PROJECTION_MATRIX4:
	case GLSLShaderTypes::UniformType::GE_UT_PVM_MATRIX4:
	{
		ref.data = Variant(Variant::VariantType::GE_VT_MAT4);
		ref.size = sizeof(glm::mat4);
	}
		break;
	case GLSLShaderTypes::UniformType::GE_UT_COUNT:
	default:
		LOG_ERROR("Invalid uniform type!");
		return;
	}

	mSize += ref.size;
}

Variant UniformBuffer::GetUniform(GLSLShaderTypes::UniformType type) const
{
	assert(type < GLSLShaderTypes::UniformType::GE_UT_COUNT);

	auto iter = mUniformMap.find(type);
	if (iter != mUniformMap.end())
	{
		return iter->second.data;
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
		mpData = GE_ALLOC_ARRAY(char_t, mSize);
	}

	// repopulate mpData only if needed
	if (mpData && (mIsUniformMapSizeChanged || mIsUniformMapUpdated))
	{
		std::size_t offset = 0;
		for (auto iter = mUniformMap.begin(); iter != mUniformMap.end(); ++iter)
		{
			auto& type = iter->first;
			auto& uniform = iter->second;

			switch (type)
			{
			case GLSLShaderTypes::UniformType::GE_UT_MODEL_MATRIX3:
			case GLSLShaderTypes::UniformType::GE_UT_NORMAL_MATRIX3:
			{
				auto& ref = uniform.data.Value<glm::mat3>();
				::memcpy(mpData + offset, &ref, uniform.size);

				offset += uniform.size;
			}
			break;
			case GLSLShaderTypes::UniformType::GE_UT_MODEL_MATRIX4:
			case GLSLShaderTypes::UniformType::GE_UT_VIEW_MATRIX4:
			case GLSLShaderTypes::UniformType::GE_UT_PROJECTION_MATRIX4:
			case GLSLShaderTypes::UniformType::GE_UT_PVM_MATRIX4:
			{
				auto& ref = uniform.data.Value<glm::mat4>();
				::memcpy(mpData + offset, &ref, uniform.size);

				offset += uniform.size;
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