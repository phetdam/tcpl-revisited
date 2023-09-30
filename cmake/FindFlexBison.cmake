cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

##
# Find Flex + Bison paths given path(s) to the Flex + Bison binary directories.
#
# These can also be set as environment variables.
#
# Adapted from the infix-calculator project's original with the added ability
# to skip building anything requiring Flex + Bison if they are not found.
#

# set directory containing Flex/Bison executables. for example, if using
# WinFlexBison on Windows, set this to the path of the release directory
if(DEFINED ENV{PDCPL_FLEX_BISON_DIR})
    message(
        STATUS
        "Using PDCPL_FLEX_BISON_DIR $ENV{PDCPL_FLEX_BISON_DIR} from env"
    )
    set(PDCPL_FLEX_BISON_DIR $ENV{PDCPL_FLEX_BISON_DIR})
endif()
# can be left undefined if Flex + Bison are on system PATH
if(NOT DEFINED PDCPL_FLEX_BISON_DIR)
    message(STATUS "PDCPL_FLEX_BISON_DIR not defined, treating as empty")
    set(PDCPL_FLEX_BISON_DIR "")
# make the messages nicer by indicating when variable is empty
elseif(PDCPL_FLEX_BISON_DIR STREQUAL "")
    message(STATUS "PDCPL_FLEX_BISON_DIR: (empty)")
else()
    message(STATUS "PDCPL_FLEX_BISON_DIR: ${PDCPL_FLEX_BISON_DIR}")
endif()
# set independent directory locations for Flex/Bison. these take the value of
# PDCPL_FLEX_BISON_DIR if it is not empty and they are not explicitly defined.
if(DEFINED ENV{PDCPL_FLEX_DIR})
    message(STATUS "Using PDCPL_FLEX_DIR $ENV{PDCPL_FLEX_DIR} from env")
    set(PDCPL_FLEX_DIR $ENV{PDCPL_FLEX_DIR})
endif()
# can be left undefined and use PDCPL_FLEX_BISON_DIR or be left empty
if(NOT DEFINED PDCPL_FLEX_DIR)
    if(DEFINED PDCPL_FLEX_BISON_DIR)
        message(
            STATUS
            "PDCPL_FLEX_DIR not defined, using PDCPL_FLEX_BISON_DIR "
"${PDCPL_FLEX_BISON_DIR}"
        )
        set(PDCPL_FLEX_DIR ${PDCPL_FLEX_BISON_DIR})
    else()
        message(STATUS "PDCPL_FLEX_DIR not defined, treating as empty")
        set(PDCPL_FLEX_DIR "")
    endif()
# make the messages nicer by indicating when variable is empty
elseif(PDCPL_FLEX_DIR STREQUAL "")
    message(STATUS "PDCPL_FLEX_DIR: (empty)")
else()
    message(STATUS "PDCPL_FLEX_DIR detected: ${PDCPL_FLEX_DIR}")
endif()
if(DEFINED ENV{PDCPL_BISON_DIR})
    message(STATUS "Using PDCPL_BISON_DIR $ENV{PDCPL_BISON_DIR} from env")
    set(PDCPL_BISON_DIR $ENV{PDCPL_BISON_DIR})
endif()
# can be left undefined and use PDCPL_FLEX_BISON_DIR or be left empty
if(NOT DEFINED PDCPL_BISON_DIR)
    if(DEFINED PDCPL_FLEX_BISON_DIR)
        message(
            STATUS
            "PDCPL_BISON_DIR not defined, using PDCPL_FLEX_BISON_DIR "
"${PDCPL_FLEX_BISON_DIR}"
        )
        set(PDCPL_BISON_DIR ${PDCPL_FLEX_BISON_DIR})
    else()
        message(STATUS "PDCPL_BISON_DIR not defined, treating as empty")
        set(PDCPL_BISON_DIR "")
    endif()
# make the messages nicer by indicating when variable is empty
elseif(PDCPL_BISON_DIR STREQUAL "")
    message(STATUS "PDCPL_BISON_DIR: (empty)")
else()
    message(STATUS "PDCPL_BISON_DIR detected: ${PDCPL_BISON_DIR}")
endif()
# find Flex and Bison. if failed, just print a warning, but continue
find_program(PDCPL_FLEX NAMES flex win_flex HINTS ${PDCPL_FLEX_DIR})
if(PDCPL_FLEX STREQUAL "PDCPL_FLEX-NOTFOUND")
    set(PDCPL_FLEX_FOUND FALSE)
    message(
        STATUS
        "Flex not found. Please set PDCPL_FLEX_BISON_DIR or PDCPL_FLEX_DIR"
    )
else()
    set(PDCPL_FLEX_FOUND TRUE)
    message(STATUS "Found Flex: ${PDCPL_FLEX}")
endif()
find_program(PDCPL_BISON NAMES bison win_bison HINTS ${PDCPL_BISON_DIR})
if(PDCPL_BISON STREQUAL "PDCPL_BISON-NOTFOUND")
    set(PDCPL_BISON_FOUND FALSE)
    message(
        STATUS
        "Bison not found. Please set PDCPL_FLEX_BISON_DIR or PDCPL_BISON_DIR"
    )
else()
    set(PDCPL_BISON_FOUND TRUE)
    message(STATUS "Found Bison: ${PDCPL_BISON}")
endif()
