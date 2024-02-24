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
# public headers to install
set(
    PDCPL_PUBLIC_HEADERS
    ${PDCPL_INCLUDE_DIR}/pdcpl/bitwise.h
    ${PDCPL_INCLUDE_DIR}/pdcpl/cliopts.h
    ${PDCPL_INCLUDE_DIR}/pdcpl/common.h
    ${PDCPL_INCLUDE_DIR}/pdcpl/core.h
    ${PDCPL_INCLUDE_DIR}/pdcpl/dllexport.h
    ${PDCPL_INCLUDE_DIR}/pdcpl/features.h
    ${PDCPL_INCLUDE_DIR}/pdcpl/file.h
    ${PDCPL_INCLUDE_DIR}/pdcpl/histogram.h
    ${PDCPL_INCLUDE_DIR}/pdcpl/math.h
    ${PDCPL_INCLUDE_DIR}/pdcpl/memory.h
    ${PDCPL_INCLUDE_DIR}/pdcpl/misc.h
    ${PDCPL_INCLUDE_DIR}/pdcpl/platform.h
    ${PDCPL_INCLUDE_DIR}/pdcpl/sa.h
    ${PDCPL_INCLUDE_DIR}/pdcpl/string.h
    ${PDCPL_INCLUDE_DIR}/pdcpl/termcolors.h
    ${PDCPL_INCLUDE_DIR}/pdcpl/utility.h
    ${PDCPL_INCLUDE_DIR}/pdcpl/variant.h
    ${PDCPL_INCLUDE_DIR}/pdcpl/version.h
    ${PDCPL_INCLUDE_DIR}/pdcpl/warnings.h
)
# note: must quote the list of headers
set_target_properties(pdcpl PROPERTIES PUBLIC_HEADER "${PDCPL_PUBLIC_HEADERS}")
