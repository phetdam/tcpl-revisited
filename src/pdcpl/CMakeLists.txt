cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# add pdcpl support library
add_library(
    pdcpl
    bitwise.c file.c histogram.c memory.c string.c variant.c
)
# we could use lazy Windows __declspec(dllexport), __declspec(dllimport)
# handling by using set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON), but we opt not to
# do this to make our code build-system neutral. also, if we want to use
# -fvisibility=hidden later on, then this is also helpful for *nix platforms.
set_target_properties(pdcpl PROPERTIES DEFINE_SYMBOL PDCPL_BUILD_DLL)
if(WIN32)
    # CMake adds this by default, but it's good to be explicit
    target_link_libraries(pdcpl PRIVATE kernel32)
endif()
# need math library for *nix-like systems. note that when inline functions
# that use math functions are used in C++ code, -lm is not needed, as the C++
# standard library already has the math functions included.
if(UNIX)
    target_link_libraries(pdcpl PRIVATE m)
endif()
