cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

add_executable(
    pdcpl_test
    bitwise_test.cc
    file_test.cc
    math_test.cc
    memory_test.cc
    misc_test.cc
    string_test_1.cc
    string_test_2.cc
    variant_test.cc
)
target_link_libraries(pdcpl_test PRIVATE GTest::gtest_main pdcpl)
# on Windows, there are additional link dependencies. also, we need to copy the
# library and other DLL dependencies to output dir
if(WIN32)
    # CMake adds this by default, but it's good to be explicit
    target_link_libraries(pdcpl_test PRIVATE kernel32)
    add_custom_command(
        TARGET pdcpl_test POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different
                # sources. copy target itself so that in case DLL list is empty
                # we will not get some ugly CMake error
                $<TARGET_RUNTIME_DLLS:pdcpl_test> $<TARGET_FILE:pdcpl_test>
                # destination
                $<TARGET_FILE_DIR:pdcpl_test>
        COMMAND_EXPAND_LISTS
    )
endif()
