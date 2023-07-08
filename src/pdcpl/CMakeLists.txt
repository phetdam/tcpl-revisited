cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

add_library(pdcpl SHARED bitwise.c histogram.c memory.c string.c variant.c)
# we could use lazy Windows __declspec(dllexport), __declspec(dllimport)
# handling by using set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON), but we opt not to
# do this to make our code build-system neutral.
if(WIN32)
    target_compile_definitions(pdcpl PRIVATE PDCPL_BUILD_DLL)
endif()
# need math library for *nix-like systems. note that when inline functions
# that use math functions are used in C++ code, -lm is not needed, as the C++
# standard library already has the math functions included.
if(UNIX)
    target_link_libraries(pdcpl PRIVATE m)
endif()
