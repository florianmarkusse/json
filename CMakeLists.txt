cmake_minimum_required(VERSION 3.18)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

### Options ###
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (Debug, Release, Profiling, Fuzzing)" FORCE)
endif()
set(VALID_BUILD_TYPES "Debug" "Release" "Profiling" "Fuzzing")
list(FIND VALID_BUILD_TYPES ${CMAKE_BUILD_TYPE} VALID_BUILD_TYPE_INDEX)
if(VALID_BUILD_TYPE_INDEX EQUAL -1)
    message(FATAL_ERROR "Invalid build type specified. Please choose one of: ${VALID_BUILD_TYPES}")
endif()

option(BUILD_TESTS "Build tests" OFF)
option(BUILD_FUZZ "Build fuzz" OFF)
option(BUILD_EXAMPLE "Build example" OFF)

### Flags + Definitions ###
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wconversion -Wno-sign-conversion -Wno-gnu-alignof-expression -Wdouble-promotion -Wvla -W")
if (CMAKE_BUILD_TYPE STREQUAL "Fuzzing" OR CMAKE_BUILD_TYPE STREQUAL "Debug")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g3")

    add_compile_definitions("DEBUG")
endif()
if (CMAKE_BUILD_TYPE STREQUAL "Profiling")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg -O2 -pg")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")
endif()
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
endif()

add_compile_definitions("_GNU_SOURCE")

project(mono LANGUAGES C)

message(STATUS "=== Configuration Settings ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "Linker: ${CMAKE_LINKER}")
message(STATUS "C flags: ${CMAKE_C_FLAGS}")
message(STATUS "sanitize: ${SANITIZE_OPTIONS}")
get_directory_property(compile_definitions DIRECTORY ${CMAKE_SOURCE_DIR} COMPILE_DEFINITIONS)
message(STATUS "Compile Definitions: ${compile_definitions}")
message(STATUS "Build tests: ${BUILD_TESTS}")
message(STATUS "Build benchmarks: ${BUILD_BENCHMARKS}")
message(STATUS "Build fuzz: ${BUILD_FUZZ}")
message(STATUS "=== End Configuration ===")

add_subdirectory(util)
add_subdirectory(parse-framework)
if("${BUILD_TESTS}")
    add_subdirectory(test-framework)
endif()
add_subdirectory(json)


