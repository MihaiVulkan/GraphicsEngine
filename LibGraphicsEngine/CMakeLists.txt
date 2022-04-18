cmake_minimum_required(VERSION 3.14)

set(PROJECT_NAME LibGraphicsEngine)

#project
project(${PROJECT_NAME} LANGUAGES CXX C)

# if MSVC compiler set the current proj as the default startup one
if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endif()

# disable ZERO_CHECK proj generation
set(CMAKE_SUPPRESS_REGENERATION true)

file(GLOB_RECURSE SOURCE_LIST
	# source code
	${PROJECT_SOURCE_DIR}/src/Applications/*.hpp
	${PROJECT_SOURCE_DIR}/src/Applications/*.cpp
	${PROJECT_SOURCE_DIR}/src/Core/*.hpp
	${PROJECT_SOURCE_DIR}/src/Core/*.cpp
	${PROJECT_SOURCE_DIR}/src/Foundation/MemoryManagement/*.hpp
	${PROJECT_SOURCE_DIR}/src/Foundation/MemoryManagement/*.cpp
	${PROJECT_SOURCE_DIR}/src/Foundation/Platform/Win32/*.hpp
	${PROJECT_SOURCE_DIR}/src/Foundation/Platform/Win32/*.cpp
	${PROJECT_SOURCE_DIR}/src/Foundation/Platform/*.hpp
	${PROJECT_SOURCE_DIR}/src/Foundation/Platform/*.cpp
	${PROJECT_SOURCE_DIR}/src/Foundation/*.hpp
	${PROJECT_SOURCE_DIR}/src/Foundation/*.cpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Cameras/*.hpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Cameras/*.cpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Components/*.hpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Components/*.cpp
	${PROJECT_SOURCE_DIR}/src/Graphics/GeometricPrimitives/*.hpp
	${PROJECT_SOURCE_DIR}/src/Graphics/GeometricPrimitives/*.cpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Rendering/Backends/Vulkan/Common/*.hpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Rendering/Backends/Vulkan/Common/*.cpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Rendering/Backends/Vulkan/Internal/*.hpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Rendering/Backends/Vulkan/Internal/*.cpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Rendering/Backends/Vulkan/Resources/*.hpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Rendering/Backends/Vulkan/Resources/*.cpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Rendering/Backends/Vulkan/VisualPasses/*.hpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Rendering/Backends/Vulkan/VisualPasses/*.cpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Rendering/Backends/Vulkan/*.hpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Rendering/Backends/Vulkan/*.cpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Rendering/PipelineStates/*.hpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Rendering/PipelineStates/*.cpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Rendering/VisualEffects/*.hpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Rendering/VisualEffects/*.cpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Rendering/VisualEffects/LitEffects/*.hpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Rendering/VisualEffects/LitEffects/*.cpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Rendering/VisualEffects/UnlitEffects/*.hpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Rendering/VisualEffects/UnlitEffects/*.cpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Rendering/VisualPasses/*.hpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Rendering/VisualPasses/*.cpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Rendering/Resources/*.hpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Rendering/Resources/*.cpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Rendering/*.hpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Rendering/*.cpp
	${PROJECT_SOURCE_DIR}/src/Graphics/SceneGraph/Visitors/*.hpp
	${PROJECT_SOURCE_DIR}/src/Graphics/SceneGraph/Visitors/*.cpp
	${PROJECT_SOURCE_DIR}/src/Graphics/SceneGraph/*.hpp
	${PROJECT_SOURCE_DIR}/src/Graphics/SceneGraph/*.cpp
	${PROJECT_SOURCE_DIR}/src/Graphics/ShaderTools/GLSL/*.hpp
	${PROJECT_SOURCE_DIR}/src/Graphics/ShaderTools/GLSL/*.cpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Loaders/*.hpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Loaders/*.cpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Lights/*.hpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Lights/*.cpp
	${PROJECT_SOURCE_DIR}/src/Graphics/*.hpp
	${PROJECT_SOURCE_DIR}/src/Graphics/*.cpp
	${PROJECT_SOURCE_DIR}/src/Input/*.hpp
	${PROJECT_SOURCE_DIR}/src/Input/*.cpp
	
	# resources
	${PROJECT_SOURCE_DIR}/res/shaders/VisualEffects/LitEffects/*.vert
	${PROJECT_SOURCE_DIR}/res/shaders/VisualEffects/LitEffects/*.frag
	${PROJECT_SOURCE_DIR}/res/shaders/VisualEffects/UnlitEffects/*.vert
	${PROJECT_SOURCE_DIR}/res/shaders/VisualEffects/UnlitEffects/*.frag
	${PROJECT_SOURCE_DIR}/res/textures/*.ktx2
)

# our target is a static lib
add_library(${PROJECT_NAME} STATIC ${SOURCE_LIST})

# this allows for us to have file filters in IDEs like MS Visual Studio
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_LIST})

# includes
target_include_directories(${PROJECT_NAME} PRIVATE
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/dep/include
)

#defines
# when compiling with MSVC(cl.exe) we must choose explicitely if we want UNICODE or not
if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	add_definitions(-DUNICODE -D_UNICODE)
endif()

# Asset path
add_definitions(-DGE_ASSET_PATH=\"${PROJECT_SOURCE_DIR}/res/\")