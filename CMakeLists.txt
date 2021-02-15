cmake_minimum_required(VERSION 3.14)

#TODO - add logic for searhing the dependency libs: vulkan, etc.

set(PROJECT_NAME GraphicsEngine)

#project
project(${PROJECT_NAME} LANGUAGES CXX C)

#if MSVC compiler ALL_BUILD is the default project in this case

# disable ZERO_CHECK proj generation
set(CMAKE_SUPPRESS_REGENERATION true)

#set envs for useful paths
set(Root_Dir ${CMAKE_CURRENT_SOURCE_DIR})
set(Lib_Dir ${Root_Dir}/LibGraphicsEngine)
set(Samples_Dir ${Root_Dir}/SampleApplications)

# Build type, supported; Debug, Release
# Defult conig is Default
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
	message(STATUS "Build type not specified: Use Debug by default")
endif()

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)


#chooce Vulkan lib according to toolchain architecture: 32bit or 64bit
set(Vulkan_Lib_Dir ${Lib_Dir}/dep/lib/$ENV{ARCH}/vulkan-1.lib)

# subdirectories
add_subdirectory(${Lib_Dir})
add_subdirectory(${Samples_Dir})