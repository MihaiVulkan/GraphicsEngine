#ifndef CORE_APP_CONFIG_HPP
#define CORE_APP_CONFIG_HPP

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
//#define ENABLE_ALLOCATOR
#define ENABLE_LOG

#define RIGHT_HAND_COORDINATES //default
//#define LEFT_HAND_COORDINATES
//#define USE_GUI
#elif defined(__APPLE__) || defined(__MACOSX__)
#define ENABLE_LOG
#define USE_GUI
#elif defined(linux) || defined(__linux) || defined(__linux__) || defined(__LINUX__)
#define ENABLE_LOG
#define USE_GUI
#endif

// Vulkan Config //
#if defined(VULKAN_RENDERER)
#ifdef _WIN32
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif // VK_USE_PLATFORM_WIN32_KHR
#else
// other platforms
#endif // _WIN32

#define VULKAN_POOL_ALLOCATOR
#define VULKAN_DEBUG true

#endif // VULKAN_RENDERER

// OpenGL Config //
#if defined(OPENGL_RENDERER)

// REQUEST OPENGL 4.6 !!!
#define OPENGL_MAJOR_VER 4
#define OPENGL_MINOR_VER 6 // needed to load SPIR-V shader binaries!

// Pixel Attribs
#define OPENGL_COLOR_BITS 32
#define OPENGL_ALPHA_BITS 8
#define OPENGL_DEPTH_BITS 24
#define OPENGL_STENCIL_BITS 8
#define OPENGL_SAMPLE_COUNT 1

// Core profile
#define OPENGL_CORE_PROFILE 0
#define OPENGL_COMPATIBILITY_PROFILE 1

#define OPENGL_PROFILE OPENGL_COMPATIBILITY_PROFILE
//#define OPENGL_PROFILE OPENGL_CORE_PROFILE

#define OPENGL_DEBUG // enables OpenGL Debug Context
// this allows us to use better debug mecahnism like glDebugMessageCallback()/glGetDebugMessageLog()
// instead of the old glGetError() for each gl API call

#endif // OPENGL_RENDERER

// GLM config //
#define GLM_FORCE_MESSAGES // see GLM message output
#define GLM_FORCE_RADIANS // we use radians instead of degrees for angles
// DEPTH CLIP SPACE RANGE: GLM DEFAULT = [-1.0f, 1.0f] // GL Convention
// COORDIANTE SYSTEM HANDINESS: GLM DEFAULT = RIGHT HAND
#if defined(VULKAN_RENDERER)
//NOTE! To allign the Vulkan viewport to GL one there is also the VK_KHR_MAINTENANCE1_EXTENSION_NAME supported since Vulkan 1.1
// more info: https://www.saschawillems.de/blog/2019/03/29/flipping-the-vulkan-viewport/
#define GLM_FORCE_DEPTH_ZERO_TO_ONE //correct clip space range (depth) in Vulkan is (0.0f, 1.0f) compared to OpenGL (-1.0f, 1.0f)
#endif // VULKAN_RENDERER

#if defined(RIGHT_HAND_COORDINATES)
#undef LEFT_HAND_COORDINATES
#endif // RIGHT_HAND_COORDINATES

#if defined(LEFT_HAND_COORDINATES)
#define GLM_FORCE_LEFT_HANDED
#undef RIGHT_HAND_COORDINATES
#endif // LEFT_HAND_COORDINATES

#endif /* CORE_APP_CONFIG_HPP */