cmake_minimum_required(VERSION 3.14)

#project
project(Triangle LANGUAGES CXX C)

# disable ZERO_CHECK proj generation
set(CMAKE_SUPPRESS_REGENERATION true)

# our target is an executable
add_executable(Triangle
	${PROJECT_SOURCE_DIR}/Main.cpp
)

# includes
target_include_directories(Triangle PRIVATE
	${Lib_Dir}/src
	${Lib_Dir}/src/Core
	${Lib_Dir}/dep/include
)

# the libs we need for our project
# make LibGraphicsEngine dependency for Triangle target so that it is rebuild automatically if it's the case
target_link_libraries(Triangle PRIVATE LibGraphicsEngine)
			
# as cmake can't link static libs to other static libs when generating VS projs we link those to the app directly			
if (CMAKE_BUILD_TYPE AND CMAKE_BUILD_TYPE MATCHES Debug)			
target_link_libraries(Triangle PRIVATE
	${Lib_Dir}/dep/lib/vulkan-1.lib
	${Lib_Dir}/dep/lib/glslangd.lib
	${Lib_Dir}/dep/lib/OGLCompilerd.lib
	${Lib_Dir}/dep/lib/OSDependentd.lib
	${Lib_Dir}/dep/lib/SPIRVd.lib
	${Lib_Dir}/dep/lib/HLSLd.lib	
)
endif()

#defines
# Asset path
add_definitions(-DGE_ASSET_PATH=\"${PROJECT_SOURCE_DIR}/res/\")