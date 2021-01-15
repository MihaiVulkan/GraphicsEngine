cmake_minimum_required(VERSION 3.14)

#project
project(LibGraphicsEngine LANGUAGES CXX C)

# disable ZERO_CHECK proj generation
set(CMAKE_SUPPRESS_REGENERATION true)

# source code
file(GLOB_RECURSE SOURCE_LIST
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
	${PROJECT_SOURCE_DIR}/src/Graphics/Rendering/Backends/Vulkan/*.hpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Rendering/Backends/Vulkan/*.cpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Rendering/PipelineStates/*.hpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Rendering/PipelineStates/*.cpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Rendering/RenderPasses/*.hpp
	${PROJECT_SOURCE_DIR}/src/Graphics/Rendering/RenderPasses/*.cpp
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
	${PROJECT_SOURCE_DIR}/src/Graphics/*.hpp
	${PROJECT_SOURCE_DIR}/src/Graphics/*.cpp
	${PROJECT_SOURCE_DIR}/src/Input/*.hpp
	${PROJECT_SOURCE_DIR}/src/Input/*.cpp
)

# our target is a static lib
add_library(LibGraphicsEngine STATIC ${SOURCE_LIST})

# this allows for us to have file filters in IDEs like MS Visual Studio
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_LIST})

# includes
target_include_directories(LibGraphicsEngine PRIVATE
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/dep/include
)

#defines
# when compiling with MSVC(cl.exe) we must choose explicitely if we want UNICODE or not
if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	add_definitions(-DUNICODE -D_UNICODE)
endif()