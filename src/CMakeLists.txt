cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# current source directory can be treated as "private" include directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
# add run-time library
add_subdirectory(pdcpl)
# add Bison-generated C declaration parser library
add_subdirectory(pdcpl_bcdp)

pdcpl_add_standalone(1.1)
pdcpl_add_standalone(1.2)
pdcpl_add_standalone(1.3)
pdcpl_add_standalone(1.4)
pdcpl_add_standalone(1.5)
pdcpl_add_standalone(1.6)
pdcpl_add_standalone(1.7)
pdcpl_add_standalone(1.8)
pdcpl_add_standalone(1.9)
pdcpl_add_standalone(1.10)
pdcpl_add_standalone(1.12)
pdcpl_add_standalone(1.16 REQUIRES pdcpl)
pdcpl_add_standalone(1.17 REQUIRES pdcpl)
pdcpl_add_standalone(1.19 REQUIRES pdcpl)
pdcpl_add_standalone(2.1)
pdcpl_add_standalone(2.2)
pdcpl_add_standalone(2.9 REQUIRES pdcpl)
pdcpl_add_standalone(3.4 REQUIRES pdcpl)
pdcpl_add_standalone(4.14)
pdcpl_add_standalone(5.13 REQUIRES pdcpl)
pdcpl_add_standalone(5.16 REQUIRES pdcpl)
# 5.20++ uses pdcpl_bcdp, so Flex and Bison need to be available
# TODO: create pdcpl_add_cc_standalone function to simplify this?
if(PDCPL_FLEX_FOUND AND PDCPL_BISON_FOUND)
    add_executable(5.20++ 5.20++.cc)
    target_link_libraries(5.20++ PRIVATE pdcpl pdcpl_bcdp)
    if(WIN32)
        add_custom_command(
            TARGET 5.20++ POST_BUILD
            COMMAND
                ${CMAKE_COMMAND} -E copy_if_different
                    $<TARGET_RUNTIME_DLLS:5.20++> $<TARGET_FILE_DIR:5.20++>
            # copy on Windows as link creation requires privilege escalation
            COMMAND
                ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:5.20++>
                    $<TARGET_FILE_DIR:5.20++>/bdcl.exe
            DEPENDS 5.20++
            # need to expand CMake list into separate args when copying DLLs
            COMMAND_EXPAND_LISTS
            COMMENT "Creating 5.20++.exe copy bdcl.exe + copying dependent DLLs"
        )
    else()
        add_custom_command(
            TARGET 5.20++ POST_BUILD
            # use ln directly as create_symlink creates link with absolute paths
            COMMAND ln -frs $<TARGET_FILE:5.20++> $<TARGET_FILE_DIR:5.20++>/bdcl
            DEPENDS 5.20++
            COMMENT "Creating 5.20++ relative symlink bdcl"
        )
    endif()
endif()
pdcpl_add_standalone(6.1 REQUIRES pdcpl)
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
        DEPENDS 7.1
        COMMENT "Creating 7.1.exe copies lower.exe, upper.exe"
    )
else()
    add_custom_command(
        TARGET 7.1 POST_BUILD WORKING_DIRECTORY $<TARGET_FILE_DIR:7.1>
        # use ln directly as create_symlink creates link with absolute paths
        COMMAND ln -frs $<TARGET_FILE:7.1> $<TARGET_FILE_DIR:7.1>/lower
        COMMAND ln -frs $<TARGET_FILE:7.1> $<TARGET_FILE_DIR:7.1>/upper
        DEPENDS 7.1
        COMMENT "Creating 7.1 relative symlinks lower, upper"
    )
endif()
