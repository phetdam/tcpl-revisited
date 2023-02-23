cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

if(WIN32)
    # always use release VC++ C runtime for ease of dependency management.
    # note: turned this off since this makes Google Test integration difficult.
    # Google Test uses debug VC++ C runtime for debug, release for release.
    # set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreadedDLL)
    add_compile_options(
        /Wall
        # implicit definition of copy, move ctor/operator= as deleted. MSVC
        # tends to emit this whenenever one uses Google Test.
        /wd4625 /wd4626 /wd5026 /wd5027
        # Spectre mitigation, winbase.h macro expansion issue
        /wd5045 /wd5105
    )
    if(NOT CMAKE_BUILD_TYPE STREQUAL Release)
        add_compile_options(/Od /DEBUG)
    else()
        add_compile_options(/O2)
    endif()
# Clang/GCC can accept most of the same options
else()
    add_compile_options(-Wall)
    if(NOT CMAKE_BUILD_TYPE STREQUAL Release)
        add_compile_options(-O0 -ggdb)
    else()
        add_compile_options(-O3)
    endif()
endif()
