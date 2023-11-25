/**
 * @file 5.20++.cc
 * @author Derek Huang
 * @brief The C Programming Language exercise 5-20 in C++ using pdcpl_bcdp
 * @copyright MIT License
 */

#include <cstdio>
#include <filesystem>
#include <iostream>

#define PDCPL_HAS_PROGRAM_USAGE
#define PDCPL_HAS_PROGRAM_OPTIONS
#include "pdcpl/cliopts.h"
#include "pdcpl/core.h"

#include "pdcpl/cdcl_parser.hh"

/**
 * Static globals set during program option parsing.
 */
static std::filesystem::path input_path;
static bool trace_lexer = false;
static bool trace_parser = false;

/**
 * Action to get the specified input path if provided.
 */
static
PDCPL_CLIOPT_ACTION(input_path_action)
{
  PDCPL_CLIOPT_ACTION_ARGI_GUARD
  // file must exist and be regular
  auto path = argv[argi + 1];
  if (!std::filesystem::exists(path))
    return PDCPL_CLIOPT_ERROR_NO_PATH_EXISTS;
  if (!std::filesystem::is_regular_file(path))
    return PDCPL_CLIOPT_ERROR_NOT_REGULAR_FILE;
  // other set the path and return
  input_path = path;
  return PDCPL_CLIOPT_PARSE_OK;
}

/**
 * Action to determine whether or not to enable lexer tracing.
 */
static
PDCPL_CLIOPT_ACTION(trace_lexer_action)
{
  PDCPL_CLIOPT_ACTION_NO_WARN_UNUSED
  trace_lexer = true;
  return PDCPL_CLIOPT_PARSE_OK;
}

/**
 * Action to determine whether or not to enable parser tracing.
 */
static
PDCPL_CLIOPT_ACTION(trace_parser_action)
{
  PDCPL_CLIOPT_ACTION_NO_WARN_UNUSED
  trace_parser = true;
  return PDCPL_CLIOPT_PARSE_OK;
}

/**
 * Acttion to determine whether or not to enable full parser tracing.
 */
static
PDCPL_CLIOPT_ACTION(trace_full_action)
{
  PDCPL_CLIOPT_ACTION_NO_WARN_UNUSED
  trace_lexer = true;
  trace_parser = true;
  return PDCPL_CLIOPT_PARSE_OK;
}

PDCPL_PROGRAM_USAGE_DEF
(
  "C++ program that prints word descriptions for ANSI C declarations.\n"
  "\n"
  "Uses the pdcpl_bcdp C++ library to parse incoming ANSI C declarations. Can\n"
  "parse standard variable and function declarations, accepting builtin, enum,\n"
  "struct, array, pointer and arbitrary types in the declarations, as well as\n"
  "cv-qualifiers and storage specifiers. Function parameters need not be named.\n"
  "\n"
  "Currently, the descriptions for each declaration are not printed in their\n"
  "order of appearance in the input. This may change in a later version."
)

PDCPL_PROGRAM_OPTIONS_DEF
{
  {
    "-i",
    "--input",
    "Input file to read from.\n"
    "If not specified, input is read from stdin.",
    1,
    input_path_action,
    NULL
  },
  {
    "-T=lexer",
    "--trace-lexer",
    "Enable tracing for the lexer only",
    0,
    trace_lexer_action,
    NULL
  },
  {
    "-T=parser",
    "--trace-parser",
    "Enable tracing for the parser only",
    0,
    trace_parser_action,
    NULL
  },
  {
    "-T", "--trace",
    "Enable tracing for both the lexer and parser",
    0,
    trace_full_action,
    NULL
  },
  PDCPL_PROGRAM_OPTIONS_END
};

PDCPL_ARG_MAIN
{
  PDCPL_PARSE_PROGRAM_OPTIONS();
  // create parser
  pdcpl::cdcl_parser parser;
  // parse + print error if failed
  if (!parser.parse(input_path, trace_lexer, trace_parser)) {
    std::cerr << PDCPL_PROGRAM_NAME << ": " << parser.last_error() << std::endl;
    return EXIT_FAILURE;
  }
  // loop through parsed declarations and print them to the screen
  // TODO: may want to ensure that parser prints these in parse order
  for ([[maybe_unused]] const auto& [iden, dcln] : parser.results())
    std::cout << dcln << std::endl;
  return EXIT_SUCCESS;
}
