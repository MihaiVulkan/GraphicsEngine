#ifndef FOUNDATION_TYPEDEFS_HPP
#define FOUNDATION_TYPEDEFS_HPP

// typedefs for boolean types
typedef bool		bool_t;

// typedefs for char types
typedef char		char_t;

// typedefs for integer types
#include <cstdint>

typedef long		long_t;

// typedefs for floating point types - binary types
#include <cfloat>

// https://en.wikipedia.org/wiki/Floating-point_arithmetic
//typedef half_float        bfloat16_t; //TODO - not yet support for half float
typedef float               bfloat32_t;
typedef double				bfloat64_t;

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

typedef glm::ivec3 UVec3;
typedef glm::vec3 Vec3;
typedef glm::ivec4 UVec4;
typedef glm::vec4 Vec4;
typedef Vec3 Color3f;
typedef Vec4 Color4f;

#endif // FOUNDATION_TYPEDEFS_HPP