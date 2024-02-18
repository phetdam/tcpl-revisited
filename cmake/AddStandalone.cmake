cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

##
# Add an entry point for a "standalone" executable.
#
# The main .c source file has the same name as the output executable. On
# Windows, any DLLs created that this executable depends on will be copied to
# its output directory so the executable can find the DLLs at runtime.
#
# Arguments:
#   target
#       Name of the output target
#   REQUIRES ...
#       Names of targets to link against using the PRIVATE interface
#
function(pdcpl_add_standalone target)
    # parse REQUIRES multi-value argument after target. ARGN contains all the
    # arguments that are after the first required argument target
    set(MULTI_VALUE_ARGS REQUIRES)
    cmake_parse_arguments(TARGET "" "" "${MULTI_VALUE_ARGS}" ${ARGN})
    # add executable for the exercise
    add_executable(${target} ${target}.c)
    # link against any specified library targets. note list is expanded
    if(DEFINED TARGET_REQUIRES)
        target_link_libraries(${target} PRIVATE ${TARGET_REQUIRES})
        # on Windows, copy DLLs to the executable location so they can be found
        # during runtime. do this only if TARGET_REQUIRES is non-empty
        if(WIN32 AND TARGET_REQUIRES)
            add_custom_command(
                TARGET ${target} POST_BUILD
                COMMAND
                    ${CMAKE_COMMAND} -E copy_if_different
                        $<TARGET_RUNTIME_DLLS:${target}>
                        $<TARGET_FILE_DIR:${target}>
                COMMENT "Copying ${target} dependencies to output directory"
                VERBATIM
                COMMAND_EXPAND_LISTS
            )
        endif()
    endif()
endfunction()
