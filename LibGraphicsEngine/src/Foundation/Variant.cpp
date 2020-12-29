#include "Variant.hpp"
#include <utility> //std::move(), std::swap()

using namespace GraphicsEngine;

std::unordered_map<std::size_t, Variant::VariantType> Variant::mTypeHashMap;

void Variant::SetTypeHashes()
{
	Variant::mTypeHashMap[HashUtils::GetTypeHash<bool_t>()] = VariantType::GE_VT_BOOL;
	Variant::mTypeHashMap[HashUtils::GetTypeHash<int32_t>()] = VariantType::GE_VT_INT32;
	Variant::mTypeHashMap[HashUtils::GetTypeHash<int64_t>()] = VariantType::GE_VT_INT64;
	Variant::mTypeHashMap[HashUtils::GetTypeHash<uint32_t>()] = VariantType::GE_VT_UINT32;
	Variant::mTypeHashMap[HashUtils::GetTypeHash<uint64_t>()] = VariantType::GE_VT_UINT64;
	Variant::mTypeHashMap[HashUtils::GetTypeHash<float32_t>()] = VariantType::GE_VT_FLOAT32;
	Variant::mTypeHashMap[HashUtils::GetTypeHash<float64_t>()] = VariantType::GE_VT_FLOAT64;
	Variant::mTypeHashMap[HashUtils::GetTypeHash<std::string>()] = VariantType::GE_VT_STRING;
	Variant::mTypeHashMap[HashUtils::GetTypeHash<glm::vec2>()] = VariantType::GE_VT_VEC2;
	Variant::mTypeHashMap[HashUtils::GetTypeHash<glm::vec3>()] = VariantType::GE_VT_VEC3;
	Variant::mTypeHashMap[HashUtils::GetTypeHash<glm::vec4>()] = VariantType::GE_VT_VEC4;
	Variant::mTypeHashMap[HashUtils::GetTypeHash<glm::mat2>()] = VariantType::GE_VT_MAT2;
	Variant::mTypeHashMap[HashUtils::GetTypeHash<glm::mat3>()] = VariantType::GE_VT_MAT3;
	Variant::mTypeHashMap[HashUtils::GetTypeHash<glm::mat4>()] = VariantType::GE_VT_MAT4;
}

Variant::Variant()
{
	Reset();

	SetTypeHashes();
}

Variant::Variant(const Variant& var)
{
	Copy(var);
}

Variant::Variant(Variant&& var)
{
	Move(var);
}

Variant::Variant(Variant::VariantType type)
	: Variant()
{
	mType = type;
}

Variant::Variant(bool_t val)
	: Variant()
{
	mBool = val;
}

Variant::Variant(int32_t val)
	: Variant()
{
	mInt32 = val;
}

Variant::Variant(int64_t val)
	: Variant()
{
	mInt64 = val;
}

Variant::Variant(uint32_t val)
	: Variant()
{
	mUInt32 = val;
}

Variant::Variant(uint64_t val)
	: Variant()
{
	mUInt64 = val;
}

Variant::Variant(float32_t val)
	: Variant()
{
	mFloat32 = val;
}

Variant::Variant(float64_t val)
	: Variant()
{
	mFloat64 = val;
}

Variant::Variant(const std::string& val)
	: Variant()
{
	mString = val;
}

Variant::Variant(const glm::vec2& val)
	: Variant()
{
	mVec2 = val;
}

Variant::Variant(const glm::vec3& val)
{
	mVec3 = val;
}

Variant::Variant(const glm::vec4& val)
	: Variant()
{
	mVec4 = val;
}

Variant::Variant(const glm::mat2& val)
	: Variant()
{
	mMat2 = val;
}

Variant::Variant(const glm::mat3& val)
	: Variant()
{
	mMat3 = val;
}

Variant::Variant(const glm::mat4& val)
	: Variant()
{ 
	mMat4 = val;

	static const std::size_t mat4Hash = HashUtils::GetTypeHash<glm::mat4>();
}

Variant::~Variant()
{
	Reset();
}

Variant& Variant::operator = (const Variant& var)
{
	Copy(var);

	return *this;
}


Variant& Variant::operator = (Variant&& var)
{
	Move(var);

	return *this;
}

bool_t Variant::IsValid() const
{
	return (mType != VariantType::GE_VT_COUNT);
}

Variant::VariantType Variant::Type() const
{
	return mType;
}

void Variant::Swap(Variant& var)
{
	std::swap(mType, var.mType);

	switch (mType)
	{
	case VariantType::GE_VT_BOOL:
		std::swap(mBool, var.mBool);
		break;
	case VariantType::GE_VT_INT32:
		std::swap(mInt32, var.mInt32);
		break;
	case VariantType::GE_VT_INT64:
		std::swap(mInt64, var.mInt64);
		break;
	case VariantType::GE_VT_UINT32:
		std::swap(mUInt32, var.mUInt32);
		break;
	case VariantType::GE_VT_UINT64:
		std::swap(mUInt64, var.mUInt64);
		break;
	case VariantType::GE_VT_FLOAT32:
		std::swap(mFloat32, var.mFloat32);
		break;
	case VariantType::GE_VT_FLOAT64:
		std::swap(mFloat64, var.mFloat64);
		break;
	case VariantType::GE_VT_STRING:
		std::swap(mString, var.mString);
		break;
	case VariantType::GE_VT_VEC2:
		std::swap(mVec2, var.mVec2);
		break;
	case VariantType::GE_VT_VEC3:
		std::swap(mVec3, var.mVec3);
		break;
	case VariantType::GE_VT_VEC4:
		std::swap(mVec4, var.mVec4);
		break;
	case VariantType::GE_VT_MAT2:
		std::swap(mMat2, var.mMat2);
		break;
	case VariantType::GE_VT_MAT3:
		std::swap(mMat3, var.mMat3);
		break;
	case VariantType::GE_VT_MAT4:
		std::swap(mMat4, var.mMat4);
		break;
	case VariantType::GE_VT_COUNT:
	default:
		LOG_ERROR("Unsupported type!");
		break;
	}
}

void Variant::Reset()
{
	mType = Variant::VariantType::GE_VT_COUNT;
}

void Variant::Copy(const Variant& var)
{
	if (this != &var)
	{
		mType = var.mType;

		switch (mType)
		{
		case VariantType::GE_VT_BOOL:
			mBool = var.mBool;
			break;
		case VariantType::GE_VT_INT32:
			mInt32 = var.mInt32;
			break;
		case VariantType::GE_VT_INT64:
			mInt64 = var.mInt64;
			break;
		case VariantType::GE_VT_UINT32:
			mUInt32 = var.mUInt32;
			break;
		case VariantType::GE_VT_UINT64:
			mUInt64 = var.mUInt64;
			break;
		case VariantType::GE_VT_FLOAT32:
			mFloat32 = var.mFloat32;
			break;
		case VariantType::GE_VT_FLOAT64:
			mFloat64 = var.mFloat64;
			break;
		case VariantType::GE_VT_STRING:
			mString = var.mString;
			break;
		case VariantType::GE_VT_VEC2:
			mVec2 = var.mVec2;
			break;
		case VariantType::GE_VT_VEC3:
			mVec3 = var.mVec3;
			break;
		case VariantType::GE_VT_VEC4:
			mVec4 = var.mVec4;
			break;
		case VariantType::GE_VT_MAT2:
			mMat2 = var.mMat2;
			break;
		case VariantType::GE_VT_MAT3:
			mMat3 = var.mMat3;
			break;
		case VariantType::GE_VT_MAT4:
			mMat4 = var.mMat4;
			break;
		case VariantType::GE_VT_COUNT:
		default:
			LOG_ERROR("Unsupported type!");
			break;
		}
	}
}

void Variant::Move(Variant& var)
{
	if (this != &var)
	{
		Swap(var);
	}
}