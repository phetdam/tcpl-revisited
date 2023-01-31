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

# project version values and full version
set(PDCPL_VERSION_MAJOR 0)
set(PDCPL_VERSION_MINOR 1)
set(PDCPL_VERSION_PATCH 0)
set(
    PDCPL_VERSION
    ${PDCPL_VERSION_MAJOR}.${PDCPL_VERSION_MINOR}.${PDCPL_VERSION_PATCH}
)

# set some system information variables used for the version info
set(PDCPL_SYSTEM_NAME ${CMAKE_SYSTEM_NAME})
set(PDCPL_SYSTEM_VERSION ${CMAKE_SYSTEM_VERSION})
set(PDCPL_SYSTEM_ARCH ${CMAKE_SYSTEM_PROCESSOR})

project(
    tcpl-revisited
    VERSION ${PDCPL_VERSION}
    DESCRIPTION "Exercises from The C Programming Language, 2nd ed."
    HOMEPAGE_URL https://github.com/phetdam/tcpl-revisited
    # C++ is here preemptively since Google Test might be used for testing
    LANGUAGES C
)

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
    PDCPL_BUILD_TYPE="${PDCPL_BUILD_TYPE}"
    PDCPL_SYSTEM_NAME="${PDCPL_SYSTEM_NAME}"
    PDCPL_SYSTEM_VERSION="${PDCPL_SYSTEM_VERSION}"
    PDCPL_SYSTEM_ARCH="${PDCPL_SYSTEM_ARCH}"
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(src)
