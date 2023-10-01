cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# file prefixes and names for the lexer and parser
set(BDCL_LEXER_PREFIX lexer)
set(BDCL_LEXER_INPUT ${BDCL_LEXER_PREFIX}.l)
set(BDCL_LEXER_HEADER ${BDCL_LEXER_PREFIX}.yy.h)
set(BDCL_LEXER_SOURCE ${BDCL_LEXER_PREFIX}.yy.cc)
set(BDCL_PARSER_PREFIX parser)
set(BDCL_PARSER_INPUT ${BDCL_PARSER_PREFIX}.y)
set(BDCL_PARSER_HEADER ${BDCL_PARSER_PREFIX}.yy.h)
set(BDCL_PARSER_SOURCE ${BDCL_PARSER_PREFIX}.yy.cc)

# generate parser using Bison. header + source both go to source dir. more or
# less the same formula used in infix-calculator with some changes.
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${BDCL_PARSER_SOURCE}
    COMMAND
        ${PDCPL_BISON} -Wall
            # use historical --defines for Bison 3.5.1
            --defines=${CMAKE_CURRENT_SOURCE_DIR}/${BDCL_PARSER_HEADER}
            -o ${CMAKE_CURRENT_SOURCE_DIR}/${BDCL_PARSER_SOURCE}
            ${CMAKE_CURRENT_SOURCE_DIR}/${BDCL_PARSER_INPUT}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${BDCL_PARSER_INPUT}
    COMMENT "Running Bison to build parser ${BDCL_PARSER_SOURCE}"
    VERBATIM
)
# generate scanner using Flex. source goes to src dir
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${BDCL_LEXER_SOURCE}
    # don't define yywrap, just assume EOF is end of input. note Flex input
    # sets never-interactive as without it, isatty() is still used to handle
    # interactive input and will result in -Wimplicit-function-declaration.
    # also no need for bison-bridge if we are using the C++ Bison parser.
    COMMAND
        ${PDCPL_FLEX} -CFe --noyywrap --nounistd --warn
            -o ${CMAKE_CURRENT_SOURCE_DIR}/${BDCL_LEXER_SOURCE}
            ${CMAKE_CURRENT_SOURCE_DIR}/${BDCL_LEXER_INPUT}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${BDCL_LEXER_INPUT}
    COMMENT "Running Flex to build lexer ${BDCL_LEXER_SOURCE}"
    VERBATIM
)
# MSVC complains that a conditional expression in the Bison-generated parser
# source is constant, so we silence it just for this source file
if(MSVC)
    set_source_files_properties(
        ${BDCL_PARSER_SOURCE} PROPERTIES
        COMPILE_OPTIONS /wd4127
    )
endif()
# only build if both Flex and Bison are found
if(PDCPL_FLEX_FOUND AND PDCPL_BISON_FOUND)
    # MSVC complains that a conditional expression in the Bison-generated
    # parser source is constant, so we silence it just for this source file
    # Bison generated parser causes MSVC to emit some warnings
    if(MSVC)
        set_source_files_properties(
            ${BDCL_PARSER_SOURCE} PROPERTIES
            # C4065: switch statement with default but no case labels
            # C4127: conditional expression is constant
            COMPILE_OPTIONS "/wd4065;/wd4127"
        )
    endif()
    # build bdcl as a separate library. could be static or shared
    add_library(
        bdcl_lib
            ${BDCL_LEXER_SOURCE}
            ${BDCL_PARSER_SOURCE}
            dcl_parser.cc
            dcl_parser_impl.cc
    )
    set_target_properties(
        bdcl_lib PROPERTIES
            OUTPUT_NAME bdcl
            DEFINE_SYMBOL BDCL_BUILD_DLL
    )
else()
    # for formatted warning message, need leading spaces
    message(
        STATUS
        "Skipping bdcl\n"
        "    Found Flex: ${PDCPL_FLEX_FOUND}\n"
        "    Found Bison: ${PDCPL_BISON_FOUND}"
    )
endif()
# add main
# add_executable(BDCL main.cc ${BDCL_LEXER_SOURCE} ${BDCL_PARSER_SOURCE})
