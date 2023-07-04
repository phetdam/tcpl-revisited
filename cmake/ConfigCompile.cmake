cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

if(MSVC)
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
        # enum value not explicitly handled in by case label in switch
        /wd4061
        # implicit definition of copy, move ctor/operator= as deleted. MSVC
        # tends to emit this whenenever one uses Google Test.
        /wd4625 /wd4626 /wd5026 /wd5027
        # unreferenced inline function removed
        /wd4514
        # assignment within conditional expression
        /wd4706
        # /Wall enables excessive warnings about automatic inline expansion
        /wd4710 /wd4711
        # silence warnings on padding emitted when compiling x64 binaries
        /wd4820
        # compiler may not enforce left-to-right eval in braced init list
        /wd4868
        # Spectre mitigation, winbase.h macro expansion issue
        /wd5045 /wd5105
        # const value not used
        /wd5264
        # /Od applied by default when using Debug config, /O2 for Release
        $<$<NOT:$<CONFIG:Release>>:/DEBUG>
    )
    # note: does not work correctly for Windows for the test runner, which will
    # hang indefinitely with CPU constantly spinning. it is possible that
    # Google Test and AddressSanitizer simply don't mesh well.
    # enable AddressSanitizer use
    if(ENABLE_ASAN)
        message(STATUS "AddressSanitizer (/fsanitize=address) enabled")
        # must remove run-time check options from C++ flags. CMake adds these
        # by default in CMAKE_C(XX)_FLAGS_DEBUG, so remove them
        string(
            REGEX REPLACE
            "/RTC[1csu]" "" CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG}
        )
        message(STATUS "Removed /RTC[1csu] from CMAKE_C_FLAGS_DEBUG")
        string(
            REGEX REPLACE
            "/RTC[1csu]" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG}
        )
        message(STATUS "Removed /RTC[1csu] from CMAKE_CXX_FLAGS_DEBUG")
        add_compile_options(/fsanitize=address)
        # need to disable MSVC std::vector annotation for static Google Test
        add_compile_definitions(_DISABLE_VECTOR_ANNOTATION)
        # AddressSanitizer cannot be used with incremental linking
        add_link_options(/INCREMENTAL:NO)
    endif()
# Clang/GCC can accept most of the same options
else()
    add_compile_options(
        -Wall
        # -O0 is default optimization level anyways
        $<IF:$<CONFIG:Release>,-O3,-O0> $<$<NOT:$<CONFIG:Release>>:-ggdb>
    )
    # enable AddressSanitizer use
    if(ENABLE_ASAN)
        message(STATUS "AddressSanitizer (-fsanitize=address) enabled")
        # must specifiy for both compile and link
        add_compile_options(-fsanitize=address)
        add_link_options(-fsanitize=address)
    endif()
endif()
