cmake_minimum_required(VERSION 3.21)

##
# How to build:
#
# *nix:
#   cmake -S . -B build && cmake --build build -j
#
# Windows:
#   cmake -S . -B build_windows -G Ninja && cmake --build build_windows -j
#

# use new DOWNLOAD_EXTRACT_TIMESTAMP behavior for newer CMake versions
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.24)
    cmake_policy(SET CMP0135 NEW)
endif()

# project version values and full version
set(PDCPL_VERSION_MAJOR 0)
set(PDCPL_VERSION_MINOR 1)
set(PDCPL_VERSION_PATCH 0)
set(
    PDCPL_VERSION
    ${PDCPL_VERSION_MAJOR}.${PDCPL_VERSION_MINOR}.${PDCPL_VERSION_PATCH}
)

project(
    tcpl-revisited
    VERSION ${PDCPL_VERSION}
    DESCRIPTION "Exercises from The C Programming Language, 2nd ed."
    HOMEPAGE_URL https://github.com/phetdam/tcpl-revisited
    # C++ is here since we use Google Test for testing
    LANGUAGES C CXX
)

# set some system information variables used for the version info. note the
# relevant CMake variables used will be empty if used before project()
set(PDCPL_SYSTEM_NAME ${CMAKE_SYSTEM_NAME})
set(PDCPL_SYSTEM_VERSION ${CMAKE_SYSTEM_VERSION})
set(PDCPL_SYSTEM_ARCH ${CMAKE_SYSTEM_PROCESSOR})

# only Debug and Release builds available
set(PDCPL_BUILD_CONFIGS Debug Release)
# use Debug as the default build
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "CMAKE_BUILD_TYPE not set, using Debug")
    set(CMAKE_BUILD_TYPE Debug)
endif()
# check that user didn't specify something unrecognized
if(CMAKE_BUILD_TYPE IN_LIST PDCPL_BUILD_CONFIGS)
else()
    message(
        FATAL_ERROR
        "Error: CMAKE_BUILD_TYPE must be one of ${PDCPL_BUILD_CONFIGS}"
    )
endif()

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add version definitions as macro definitions
add_compile_definitions(
    PDCPL_VERSION_MAJOR=${PDCPL_VERSION_MAJOR}
    PDCPL_VERSION_MINOR=${PDCPL_VERSION_MINOR}
    PDCPL_VERSION_PATCH=${PDCPL_VERSION_PATCH}
    # need double quotes for CMake to pass as a string
    PDCPL_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
    PDCPL_SYSTEM_NAME="${PDCPL_SYSTEM_NAME}"
    PDCPL_SYSTEM_VERSION="${PDCPL_SYSTEM_VERSION}"
    PDCPL_SYSTEM_ARCH="${PDCPL_SYSTEM_ARCH}"
)

# eliminate per-configuration subdirs from being appended to build artifact
# output dirs for multi-config generators, e.g. Visual Studio
set(
    CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}$<$<CONFIG:Debug>:>
)
set(
    CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}$<$<CONFIG:Debug>:>
)
set(
    CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}$<$<CONFIG:Debug>:>
)

# update CMake module path for our CMake scripts
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# add compiler options and function for "standalone" executables
include(ConfigCompile)
include(AddStandalone)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Google Test [minimum] version to use. allow user override
if(NOT PDCPL_GTEST_VERSION)
    set(PDCPL_GTEST_VERSION 1.10.0)
endif()
# require major.minor.patch semantic version
if(NOT PDCPL_GTEST_VERSION MATCHES ^[0-9]+\.[0-9]+\.[0-9]+$)
    message(
        FATAL_ERROR
        "Google Test version ${PDCPL_GTEST_VERSION} invalid; must be of the \
format [major].[minor].[patch]"
    )
endif()
# find Google Test to enable testing. also allow use of FetchContent, which is
# convenient for CI/CD workflows or when on a system like Windows where there's
# no real way to "install" Google Test anywhere and GTEST_ROOT needs to be set.
if(FETCH_GTEST)
    # URL prefix to the release archive
    set(
        GTEST_RELEASE_PREFIX
        https://github.com/google/googletest/archive/refs/tags
    )
    # Google changed release format from "release-x.y.z" to "vx.y.z" for 1.13.0+
    if(PDCPL_GTEST_VERSION VERSION_GREATER_EQUAL 1.13.0)
        set(GTEST_ARCHIVE_PREFIX "v")
    else()
        set(GTEST_ARCHIVE_PREFIX "release-")
    endif()
    # full path for convenience
    set(
        GTEST_ARCHIVE_PATH
        ${GTEST_RELEASE_PREFIX}/${GTEST_ARCHIVE_PREFIX}${PDCPL_GTEST_VERSION}.zip
    )
    # build Google Test from source as a subdirectory
    include(FetchContent)
    message(
        STATUS
        "Fetching Google Test ${PDCPL_GTEST_VERSION} from ${GTEST_ARCHIVE_PATH}"
    )
    FetchContent_Declare(googletest URL ${GTEST_ARCHIVE_PATH})
    # required on Windows to force linking against shared VC++ C runtime
    if(WIN32)
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif()
    FetchContent_MakeAvailable(googletest)
    # indicate that Google Test was found
    set(PDCPL_GTEST_FOUND TRUE)
else()
    find_package(GTest ${PDCPL_GTEST_VERSION})
    # for consistency when using FetchContent
    set(PDCPL_GTEST_FOUND GTest_FOUND)
endif()

add_subdirectory(src)

# only add tests if Google Test was found
if(PDCPL_GTEST_FOUND)
    enable_testing()
    add_subdirectory(test)
else()
    message(
        WARNING
        "Google Test >=${PDCPL_GTEST_VERSION} not found. No tests will be built."
    )
endif()
