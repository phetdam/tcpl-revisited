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

# build shared libraries by default
option(BUILD_SHARED_LIBS "Build pdcpl libraries as shared" ON)
# provide an extra $<CONFIG> subdirectory for lib and bin on Windows. this is
# on by default; we don't support flat install with "d" suffix on Windows yet
option(
    PDCPL_INSTALL_MULTI_CONFIG
    "Add per-config lib + bin subdirectories for Windows installations" ON
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

# put all build artifacts in top-level build dir. for multi-config generators
# like Visual Studio, each config's artifacts go into a subdirectory. it's not
# a good idea for multi-config generators to have different configs putting
# artifacts into the same directory since it no longer becomes possible to
# separately install debug and release artifacts.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# update CMake module path for our CMake scripts
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# add compiler options and function for "standalone" executables
include(ConfigCompile)
include(AddStandalone)
# find Flex + Bison if possible. PDCPL_FLEX_FOUND and PDCPL_BISON_FOUND will be
# set to either true or false depending on if they are found or not
include(FindFlexBison)

# add local include dir
set(PDCPL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${PDCPL_INCLUDE_DIR})

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
    set(PDCPL_GTEST_FOUND ${GTest_FOUND})
endif()

# create version.h header from version.h.in. note that we want the output file
# to have the same LF newlines as the source files, as otherwise on Windows the
# output file is automatically configured with CRLF newlines. do also note that
# since WSL Ubuntu and native Windows builds both use this file, running the
# WSL build and then the Windows one or vice versa causes the other to think
# version.h has been changed and retrigger mass recompilation of anything that
# uses version.h (almost all the programs as cliopts.h uses version.h).
set(PDCPL_VERSION_HEADER ${PDCPL_INCLUDE_DIR}/pdcpl/version.h)
configure_file(
    ${PDCPL_VERSION_HEADER}.in ${PDCPL_VERSION_HEADER}
    @ONLY NEWLINE_STYLE LF
)
cmake_path(
    RELATIVE_PATH PDCPL_VERSION_HEADER
    BASE_DIRECTORY ${PDCPL_INCLUDE_DIR}
    OUTPUT_VARIABLE PDCPL_VERSION_HEADER_RELATIVE
)
message(STATUS "Generated ${PDCPL_VERSION_HEADER_RELATIVE}")

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

# list of install targets
set(PDCPL_INSTALL_TARGETS pdcpl)
# if Flex/Bison was found, then pdcpl_bcdp is built, so add as well
if(PDCPL_FLEX_FOUND AND PDCPL_BISON_FOUND)
    set(PDCPL_INSTALL_TARGETS ${PDCPL_INSTALL_TARGETS} pdcpl_bcdp)
endif()

# install rule on Windows
if(WIN32)
    # multi-config installation
    if(PDCPL_INSTALL_MULTI_CONFIG)
        install(
            TARGETS ${PDCPL_INSTALL_TARGETS}
            ARCHIVE DESTINATION lib/$<CONFIG>
            LIBRARY DESTINATION lib/$<CONFIG>
            RUNTIME DESTINATION bin/$<CONFIG>
            PUBLIC_HEADER DESTINATION include/pdcpl
        )
    # flat installation. TODO: add "d" suffix to targets if doing flat install
    else()
        message(
            FATAL_ERROR
            "Flat installation with \"d\"-suffixed artifacts not supported yet"
        )
    endif()
# typical flat *nix install
else()
    install(
        TARGETS ${PDCPL_INSTALL_TARGETS}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        PUBLIC_HEADER DESTINATION include/pdcpl
    )
endif()
