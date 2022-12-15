cmake_minimum_required(VERSION 3.19)

project(LAYOUT_IDENTIFICATION_ALGH LANGUAGES C CXX)


enable_testing()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Initialization of common build parameters
set(BUILD_TYPE_LIST "Debug" "Release")
set(OUTPUT_DIR "${CMAKE_CURRENT_LIST_DIR}/Output")

set(OUTPUT_DEBUG_DIR "${CMAKE_CURRENT_LIST_DIR}/Debug")
set(OUTPUT_RELEASE_DIR "${CMAKE_CURRENT_LIST_DIR}/Release")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")
set(PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR})

include(UnitTestModule)
include(DoxygenModule)
include(CTest)

generate_doxygen(${PROJECT_DIR})

add_subdirectory(src)