cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

##
# Add an entry point for a "standalone" executable.
#
# The main .c source file has the same name as the output executable.
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
endfunction()
