cmake_minimum_required(VERSION 3.14)

set(PROJECT_NAME TexturedCubes)

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
	${PROJECT_SOURCE_DIR}/res/textures/*.ktx2
)

# our target is an executable
add_executable(${PROJECT_NAME} ${SOURCE_LIST})

# includes
target_include_directories(${PROJECT_NAME} PRIVATE
	${Lib_Dir}/src
	${Lib_Dir}/src/Core
	${Lib_Dir}/dep/include
)

# this allows for us to have file filters in IDEs like MS Visual Studio
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_LIST})

# the libs we need for our project
# make LibGraphicsEngine dependency for ${PROJECT_NAME} target so that it is rebuild automatically if it's the case
target_link_libraries(${PROJECT_NAME} PRIVATE LibGraphicsEngine)

#defines
# Asset path
add_definitions(-DGE_ASSET_PATH=\"${PROJECT_SOURCE_DIR}/res/\")