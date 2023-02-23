cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

add_executable(pdcpl_test misc_test.cc string_test.cc)
target_link_libraries(pdcpl_test PRIVATE GTest::gtest_main pdcpl)
# on Windows, copy the library and other DLL dependencies to output dir
if(WIN32)
    add_custom_command(
        TARGET pdcpl_test POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:pdcpl_test> $<TARGET_FILE_DIR:pdcpl_test>
        COMMAND_EXPAND_LISTS
    )
endif()
