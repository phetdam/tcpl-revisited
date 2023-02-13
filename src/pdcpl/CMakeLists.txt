cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# double quote reserved identifiers
add_library(pdcpl SHARED histogram.c "string.c")
# we could use lazy Windows __declspec(dllexport), __declspec(dllimport)
# handling by using set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON), but we opt not to
# do this to make our code build-system neutral.
target_compile_definitions(pdcpl PRIVATE PDCPL_BUILD_DLL)
