cmake_minimum_required(VERSION 3.22.0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_XCODE_GENERATE_SCHEME ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# No reason to set CMAKE_CONFIGURATION_TYPES if it's not a multiconfig generator.
get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if (IS_MULTI_CONFIG)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "" FORCE)
else()
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release"  CACHE STRING "Build type" FORCE)
    endif()
endif()

project(metasonic VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# set(CMAKE_CXX_FLAGS "-fno-exceptions -fno-rtti" CACHE STRING "C++ standard to be used" FORCE)
# set(CMAKE_CXX_FLAGS_DEBUG "-fno-exceptions -fno-rtti" CACHE STRING "C++ standard to be used" FORCE)
# set(CMAKE_OBJCXX_FLAGS "-fno-exceptions -fno-rtti" CACHE STRING "C++ standard to be used" FORCE)
# set(CMAKE_OBJCXX_FLAGS_DEBUG "-fno-exceptions -fno-rtti" CACHE STRING "C++ standard to be used" FORCE)

# User options.
option(MTS_BUILD_TESTS "Build and run tests." ON) 
option(MTS_BUILD_BENCHS "Build and run benchmarks." ON)

include(mts_fetch_library)
include(mts_compile_options)
include(mts_add_subdirectories)

set(MTS_MODULES_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/modules")
set(MTS_BUILD_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/output")
set(MTS_MODULES_OUTPUT_DIRECTORY "${MTS_BUILD_OUTPUT_DIRECTORY}/modules")
set(MTS_TESTS_OUTPUT_DIRECTORY "${MTS_BUILD_OUTPUT_DIRECTORY}/tests")
set(MTS_BENCHS_OUTPUT_DIRECTORY "${MTS_BUILD_OUTPUT_DIRECTORY}/benchs")
set(MTS_EXAMPLES_OUTPUT_DIRECTORY "${MTS_BUILD_OUTPUT_DIRECTORY}/examples")

set(MTS_MODULES_FOLDER_NAME "modules")
set(MTS_TESTS_FOLDER_NAME "tests")
set(MTS_BENCHS_FOLDER_NAME "benchs")
set(MTS_EXAMPLES_FOLDER_NAME "examples")

# Fetch dependencies.
set(MTS_EXTERN_REPO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/extern")
set(MTS_EXTERN_PREFIX "extern")

# Fetch googltest.
if (MTS_BUILD_TESTS OR MTS_BUILD_BENCHS)
    mts_fetch_googletest(${MTS_EXTERN_REPO_DIR} ${MTS_EXTERN_PREFIX})

    if (MTS_BUILD_BENCHS)
        mts_fetch_benchmark(${MTS_EXTERN_REPO_DIR} ${MTS_EXTERN_PREFIX})
    endif()
endif()

mts_add_subdirectories(${MTS_MODULES_DIRECTORY})
mts_add_subdirectories(${CMAKE_CURRENT_SOURCE_DIR}/examples)
