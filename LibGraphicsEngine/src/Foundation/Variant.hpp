#ifndef FOUNDATION_VARIANT_HPP
#define FOUNDATION_VARIANT_HPP

#include "Foundation/Object.hpp"
#include "Foundation/HashUtils.hpp"
#include "Foundation/Logger.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat2x2.hpp"
#include "glm/mat3x3.hpp"
#include "glm/mat4x4.hpp"
#include <unordered_map>
#include <string>

namespace GraphicsEngine
{
	/* Variant is an object which can store a multitude of types, but the types it can store are known, it's not any type. */
	class Variant : public Object
	{
		GE_RTTI(GraphicsEngine::Variant)

	public:
		enum class VariantType : uint8_t
		{
			GE_VT_BOOL = 0,
			GE_VT_INT32,
			GE_VT_INT64,
			GE_VT_UINT32,
			GE_VT_UINT64,
			GE_VT_FLOAT32,
			GE_VT_FLOAT64,
			GE_VT_STRING,
			GE_VT_VEC2,
			GE_VT_VEC3,
			GE_VT_VEC4,
			GE_VT_MAT2,
			GE_VT_MAT3,
			GE_VT_MAT4,
			// others
			GE_VT_COUNT
		};


		Variant();
		Variant(const Variant& var);
		Variant(Variant&& var);
		explicit Variant(Variant::VariantType type);
		explicit Variant(bool_t val);
		explicit Variant(int32_t val);
		explicit Variant(int64_t val);
		explicit Variant(uint32_t val);
		explicit Variant(uint64_t val);
		explicit Variant(float32_t val);
		explicit Variant(float64_t val);
		explicit Variant(const std::string& val);
		explicit Variant(const glm::vec2& val);
		explicit Variant(const glm::vec3& val);
		explicit Variant(const glm::vec4& val);
		explicit Variant(const glm::mat2& val);
		explicit Variant(const glm::mat3& val);
		explicit Variant(const glm::mat4& val);

		virtual ~Variant();

		Variant& operator = (const Variant& var);
		Variant& operator = (Variant&& var);

		bool_t IsValid() const;

		Variant::VariantType Type() const;

		void Swap(Variant& var);

		template <typename T>
		const T& Value() const
		{
			std::size_t typeHash = HashUtils::GetTypeHash<T>();

			auto iter = Variant::mTypeHashMap.find(typeHash);
			if (iter != Variant::mTypeHashMap.end())
			{
				switch (iter->second)
				{
				case VariantType::GE_VT_BOOL:
					return *(T*)&mBool;
				case VariantType::GE_VT_INT32:
					return *(T*)&mInt32;
				case VariantType::GE_VT_INT64:
					return *(T*)&mInt64;
				case VariantType::GE_VT_UINT32:
					return *(T*)&mUInt32;
				case VariantType::GE_VT_UINT64:
					return *(T*)&mUInt64;
				case VariantType::GE_VT_FLOAT32:
					return *(T*)&mFloat32;
				case VariantType::GE_VT_FLOAT64:
					return *(T*)&mFloat64;
				case VariantType::GE_VT_STRING:
					return *(T*)&mString;
				case VariantType::GE_VT_VEC2:
					return *(T*)&mVec2;
				case VariantType::GE_VT_VEC3:
					return *(T*)&mVec3;
				case VariantType::GE_VT_VEC4:
					return *(T*)&mVec4;
				case VariantType::GE_VT_MAT2:
					return *(T*)&mMat2;
				case VariantType::GE_VT_MAT3:
					return *(T*)&mMat3;
				case VariantType::GE_VT_MAT4:
					return *(T*)&mMat4;
				case VariantType::GE_VT_COUNT:
				default:
					LOG_ERROR("Unsupported type!");
					break;
				}
			}
			else
			{
				LOG_ERROR("Unsupported type!");
			}

			//NOTE! Shouldn't get here!
			return *(T*)&mBool;
		}

		template <typename T>
		bool_t SetValue(const T& val)
		{
			std::size_t typeHash = HashUtils::GetTypeHash<T>();

			auto iter = Variant::mTypeHashMap.find(typeHash);
			if (iter != Variant::mTypeHashMap.end())
			{
				switch (iter->second)
				{
				case VariantType::GE_VT_BOOL:
					mBool = *(bool_t*)&val;
					break;
				case VariantType::GE_VT_INT32:
					mInt32 = *(int32_t*)&val;
					break;
				case VariantType::GE_VT_INT64:
					mInt64 = *(int64_t*)&val;
					break;
				case VariantType::GE_VT_UINT32:
					mUInt32 = *(uint32_t*)&val;
					break;
				case VariantType::GE_VT_UINT64:
					mUInt64 = *(uint64_t*)&val;
					break;
				case VariantType::GE_VT_FLOAT32:
					mFloat32 = *(float32_t*)&val;
					break;
				case VariantType::GE_VT_FLOAT64:
					mFloat64 = *(float64_t*)&val;
					break;
				case VariantType::GE_VT_STRING:
					mString = *(std::string*)&val;
					break;
				case VariantType::GE_VT_VEC2:
					mVec2 = *(glm::vec2*)&val;
					break;
				case VariantType::GE_VT_VEC3:
					mVec3 = *(glm::vec3*)&val;
					break;
				case VariantType::GE_VT_VEC4:
					mVec4 = *(glm::vec4*)&val;
					break;
				case VariantType::GE_VT_MAT2:
					mMat2 = *(glm::mat2*)&val;
					break;
				case VariantType::GE_VT_MAT3:
					mMat3 = *(glm::mat3*)&val;
					break;
				case VariantType::GE_VT_MAT4:
					mMat4 = *(glm::mat4*)&val;
					break;
				case VariantType::GE_VT_COUNT:
				default:
					LOG_ERROR("Unsupported type!");
					return false;
				}
			}
			else
			{
				LOG_ERROR("Unsupported type!");
				return false;
			}

			return true;
		}


	private:
		VariantType mType;
		union
		{
			bool_t mBool;
			int32_t mInt32;
			int64_t mInt64;
			uint32_t mUInt32;
			uint64_t mUInt64;
			float32_t mFloat32;
			float64_t mFloat64;
			std::string mString;
			glm::vec2 mVec2;
			glm::vec3 mVec3;
			glm::vec4 mVec4;
			glm::mat2 mMat2;
			glm::mat3 mMat3;
			glm::mat4 mMat4;
		};

		static std::unordered_map<std::size_t, Variant::VariantType> mTypeHashMap;

		static void SetTypeHashes();

		void Reset();
		void Copy(const Variant& var);
		void Move(Variant& var);

	public:
		friend bool_t operator != (const Variant& v1, const Variant& v2) = delete;
		friend bool_t operator == (const Variant& v1, const Variant& v2) = delete;
	};
}

#endif // FOUNDATION_VARIANT_HPP