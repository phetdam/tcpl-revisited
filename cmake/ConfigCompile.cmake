cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

if(WIN32)
    # stop MSVC from warning about unsafe functions, C is unsafe by nature
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    # always use release VC++ C runtime for ease of dependency management.
    # note: turned this off since this makes Google Test integration difficult.
    # Google Test uses debug VC++ C runtime for debug, release for release.
    # set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreadedDLL)
    add_compile_options(
        /Wall
        # MSVC doesn't define __cplusplus correctly unless this is specified
        /Zc:__cplusplus
        # implicit definition of copy, move ctor/operator= as deleted. MSVC
        # tends to emit this whenenever one uses Google Test.
        /wd4625 /wd4626 /wd5026 /wd5027
        # Spectre mitigation, winbase.h macro expansion issue
        /wd5045 /wd5105
        # unreferenced inline function removed
        /wd4514
        # assignment within conditional expression
        /wd4706
        # enum value not explicitly handled in by case label in switch
        /wd4061
        # const value not used
        /wd5264
        # /Wall enables excessive warnings about automatic inline expansion
        /wd4710 /wd4711
        # /Od applied by default when using Debug config, /O2 for Release
        $<$<NOT:$<CONFIG:Release>>:/DEBUG>
    )
# Clang/GCC can accept most of the same options
else()
    add_compile_options(
        -Wall
        # -O0 is default optimization level anyways
        $<IF:$<CONFIG:Release>,-O3,-O0> $<$<NOT:$<CONFIG:Release>>:-ggdb>
    )
endif()
