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
#   ...
#       Names of targets to link against using the PRIVATE interface
#
function(pdcpl_add_standalone target)
    foreach(ARG ${ARGV})
        # add executable for the exercise
        if(ARG STREQUAL ${target})
            add_executable(${target} ${target}.c)
        # treat all other args as libraries to link against
        else()
            target_link_libraries(${target} PRIVATE ${ARG})
        endif()
    endforeach()
    # on Windows, we copy DLLs to the executable location so they can be found.
    # if there were any targets to link against, ARGN > 1.
    if(WIN32 AND (ARGC GREATER 1))
        add_custom_command(
            TARGET ${target} POST_BUILD
            COMMAND
                ${CMAKE_COMMAND} -E copy_if_different
                    $<TARGET_RUNTIME_DLLS:${target}>
                    $<TARGET_FILE_DIR:${target}>
            COMMAND_EXPAND_LISTS
        )
    endif()
endfunction()
