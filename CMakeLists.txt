cmake_minimum_required(VERSION 3.22)

project(chordVst3)

set(CMAKE_CXX_STANDARD 23)

if(NOT DEFINED LIB_DIR)
    set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs")
endif()

set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)
include(cmake/cpm.cmake)

CPMAddPackage(
    NAME JUCE
    GITHUB_REPOSITORY juce-framework/JUCE
    GIT_TAG 8.0.6
    VERSION 8.0.6
    SOURCE_DIR ${LIB_DIR}/juce    
)

if (MSVC)
    add_compile_options(/Wall /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_subdirectory(plugin)    