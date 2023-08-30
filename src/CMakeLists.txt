cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# add run-time library first
add_subdirectory(pdcpl)

pdcpl_add_standalone(1.1)
add_test(NAME 1.1 COMMAND 1.1)
pdcpl_add_standalone(1.2)
add_test(NAME 1.2 COMMAND 1.2)
pdcpl_add_standalone(1.3)
add_test(NAME 1.3 COMMAND 1.3)
pdcpl_add_standalone(1.4)
add_test(NAME 1.4 COMMAND 1.4)
pdcpl_add_standalone(1.5)
add_test(NAME 1.5 COMMAND 1.5)
pdcpl_add_standalone(1.6)
# no printf known to CMD
if(WIN32)
    message(WARNING "1.6: test addition skipped")
else()
    add_test(NAME 1.6 COMMAND sh -c "printf 'hello\\n' | ./1.6")
endif()
pdcpl_add_standalone(1.7)
add_test(NAME 1.7 COMMAND 1.7)
pdcpl_add_standalone(1.8)
if(WIN32)
    message(WARNING "1.8: test addition skipped")
else()
    add_test(NAME 1.8 COMMAND sh -c "./1.8 < ${CMAKE_SOURCE_DIR}/CMakeLists.txt")
endif()
pdcpl_add_standalone(1.9)
pdcpl_add_standalone(1.10)
pdcpl_add_standalone(1.12)
pdcpl_add_standalone(1.16 pdcpl)
pdcpl_add_standalone(1.17 pdcpl)
pdcpl_add_standalone(1.19 pdcpl)
pdcpl_add_standalone(2.1)
pdcpl_add_standalone(2.2)
pdcpl_add_standalone(2.9 pdcpl)
pdcpl_add_standalone(3.4 pdcpl)
pdcpl_add_standalone(4.14)
pdcpl_add_standalone(5.13 pdcpl)
pdcpl_add_standalone(5.16 pdcpl)
pdcpl_add_standalone(6.1 pdcpl)
pdcpl_add_standalone(7.1)
# create the appropriate copies/symbolic links of 7.1
if(WIN32)
    add_custom_command(
        TARGET 7.1 POST_BUILD
        # copy on Windows as link creation requires privilege escalation
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:7.1>
                $<TARGET_FILE_DIR:7.1>/lower.exe
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:7.1>
                $<TARGET_FILE_DIR:7.1>/upper.exe
    )
else()
    add_custom_command(
        TARGET 7.1 POST_BUILD WORKING_DIRECTORY $<TARGET_FILE_DIR:7.1>
        # use ln directly as create_symlink creates link with absolute paths
        COMMAND ln -frs $<TARGET_FILE:7.1> $<TARGET_FILE_DIR:7.1>/lower
        COMMAND ln -frs $<TARGET_FILE:7.1> $<TARGET_FILE_DIR:7.1>/upper
    )
endif()
