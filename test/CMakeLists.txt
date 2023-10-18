cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# unit test runner
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
# if Flex and Bison are available, pdcpl_bcdp will be built, so add its tests,
# define path to the test data directory, link against pdcpl_bcdp library
if(PDCPL_FLEX_FOUND AND PDCPL_BISON_FOUND)
    target_sources(pdcpl_test PRIVATE cdcl_parser_test.cc)
    target_compile_definitions(
        pdcpl_test
        PRIVATE PDCPL_BCDP_TEST_DIR="${CMAKE_SOURCE_DIR}/data"
    )
    target_link_libraries(pdcpl_test PRIVATE pdcpl_bcdp)
endif()
# on Windows, there are additional link dependencies. also, we need to copy the
# library and other DLL dependencies to output dir
if(WIN32)
    # CMake adds this by default, but it's good to be explicit
    target_link_libraries(pdcpl_test PRIVATE kernel32)
    # only need to copy when building DLLs
    if(BUILD_SHARED_LIBS)
        add_custom_command(
            TARGET pdcpl_test POST_BUILD
            COMMAND
                ${CMAKE_COMMAND} -E copy_if_different
                    # sources
                    $<TARGET_RUNTIME_DLLS:pdcpl_test>
                    # destination
                    $<TARGET_FILE_DIR:pdcpl_test>
            COMMAND_EXPAND_LISTS
        )
    endif()
endif()
