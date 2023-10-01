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
        # during runtime. we also copy the target itself if different in order
        # to ensure that even if there are no DLL dependencies, the
        # copy_if_different will still succeed instead of erroring.
        if(WIN32)
            add_custom_command(
                TARGET ${target} POST_BUILD
                COMMAND
                    ${CMAKE_COMMAND} -E copy_if_different
                        # sources
                        $<TARGET_RUNTIME_DLLS:${target}> $<TARGET_FILE:${target}>
                        # destination
                        $<TARGET_FILE_DIR:${target}>
                COMMAND_EXPAND_LISTS
            )
        endif()
    endif()
endfunction()
