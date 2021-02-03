cmake_minimum_required(VERSION 3.14)

set(PROJECT_NAME Triangle)

#project
project(${PROJECT_NAME} LANGUAGES CXX C)

# if MSVC compiler set the current proj as the default startup one
if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endif()

# disable ZERO_CHECK proj generation
set(CMAKE_SUPPRESS_REGENERATION true)

# source code
file(GLOB_RECURSE SOURCE_LIST
	${PROJECT_SOURCE_DIR}/*.hpp
	${PROJECT_SOURCE_DIR}/*.cpp
	${PROJECT_SOURCE_DIR}/res/shaders/*.vert
	${PROJECT_SOURCE_DIR}/res/shaders/*.frag
)

# our target is an executable
add_executable(${PROJECT_NAME} ${SOURCE_LIST})

# this allows for us to have file filters in IDEs like MS Visual Studio
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_LIST})

# includes
target_include_directories(${PROJECT_NAME} PRIVATE
	${Lib_Dir}/src
	${Lib_Dir}/src/Core
	${Lib_Dir}/dep/include
)

# the libs we need for our project
# make LibGraphicsEngine dependency for ${PROJECT_NAME} target so that it is rebuild automatically if it's the case
target_link_libraries(${PROJECT_NAME} PRIVATE LibGraphicsEngine)
			
# as cmake can't link static libs to other static libs when generating VS projs we link those to the app directly
if (CMAKE_BUILD_TYPE AND CMAKE_BUILD_TYPE MATCHES Debug)
target_link_libraries(${PROJECT_NAME} PRIVATE		
	${Lib_Dir}/dep/lib/vulkan-1.lib
	${Lib_Dir}/dep/lib/Debug/glslangd.lib
	${Lib_Dir}/dep/lib/Debug/OGLCompilerd.lib
	${Lib_Dir}/dep/lib/Debug/OSDependentd.lib
	${Lib_Dir}/dep/lib/Debug/SPIRVd.lib
	${Lib_Dir}/dep/lib/Debug/HLSLd.lib
)
elseif (CMAKE_BUILD_TYPE AND CMAKE_BUILD_TYPE MATCHES Release)
target_link_libraries(${PROJECT_NAME} PRIVATE		
	${Lib_Dir}/dep/lib/vulkan-1.lib
	${Lib_Dir}/dep/lib/Release/glslang.lib
	${Lib_Dir}/dep/lib/Release/OGLCompiler.lib
	${Lib_Dir}/dep/lib/Release/OSDependent.lib
	${Lib_Dir}/dep/lib/Release/SPIRV.lib
	${Lib_Dir}/dep/lib/Release/HLSL.lib
)
endif()
#defines
# Asset path
add_definitions(-DGE_ASSET_PATH=\"${PROJECT_SOURCE_DIR}/res/\")