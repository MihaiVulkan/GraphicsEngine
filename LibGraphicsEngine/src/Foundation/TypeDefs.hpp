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
//typedef half_float        float16_t; //TODO - not yet support for half float
typedef float               float32_t;
typedef double				float64_t;

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

typedef glm::vec3 Color3f;
typedef glm::vec4 Color4f;

#endif // FOUNDATION_TYPEDEFS_HPP