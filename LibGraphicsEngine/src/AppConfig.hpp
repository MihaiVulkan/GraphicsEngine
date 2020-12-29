/* Author: BAIRAC MIHAI */

#ifndef APP_CONFIG_HPP
#define APP_CONFIG_HPP

#define USE_GLSLANG //TODO - using glsltospriv lib for shader introspection, maybe create our own code for introspection

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
//#define ENABLE_ALLOCATOR
#define ENABLE_LOG
#define VULKAN_RENDERER
#define VULKAN_POOL_ALLOCATOR
#define VULKAN_DEBUG true

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

// VULKAN Condig //
#ifdef VULKAN_RENDERER
#ifdef _WIN32
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif // VK_USE_PLATFORM_WIN32_KHR
#else
// other platforms
#endif // _WIN32
#endif // VULKAN_RENDERER

// GLM config //
#define GLM_FORCE_MESSAGES
#define GLM_FORCE_RADIANS
// DEPTH RANGE: GLM DEFAULT = [-1.0f, 1.0f]
// COORDIANTE SYSTEM HANDINESS: GLM DEFAULT = RIGHT HAND
#if defined(VULKAN_RENDERER)
#define GLM_FORCE_DEPTH_ZERO_TO_ONE //correct depth range in Vulkan (0.0f, 1.0f) compared to OpenGL (-1.0f, 1.0f)
#endif // VULKAN_RENDERER

#if defined(RIGHT_HAND_COORDINATES)
#undef LEFT_HAND_COORDINATES
#endif // RIGHT_HAND_COORDINATES

#if defined(LEFT_HAND_COORDINATES)
#define GLM_FORCE_LEFT_HANDED
#undef RIGHT_HAND_COORDINATES
#endif // LEFT_HAND_COORDINATES

#endif /* APP_CONFIG_HPP */