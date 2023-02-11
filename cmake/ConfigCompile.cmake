cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

if(WIN32)
    # always use release VC++ C runtime for ease of dependency management
    set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreadedDLL)
    add_compile_options(
        /Wall
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
