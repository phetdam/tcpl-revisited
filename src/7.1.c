/**
 * @file 7.1.c
 * @author Derek Huang
 * @brief The C Programming Language exercise 7-1
 * @copyright MIT License
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PDCPL_HAS_PROGRAM_USAGE
#include "pdcpl/cliopts.h"
#include "pdcpl/core.h"

/**
 * Enum to indicate stream operation state.
 */
typedef enum {
  stream_state_success,
  stream_state_error_input,
  stream_state_error_output
} stream_state;

/**
 * Convert input read from `in` to lowercase and write to `out`.
 *
 * @param in Stream to read from
 * @param out Stream to write to
 */
static stream_state
convert_lower(FILE *in, FILE *out)
{
  int c;
  while ((c = fgetc(in)) != EOF)
    if (fputc(tolower(c), out) == EOF && ferror(out))
      return stream_state_error_output;
  return (ferror(in)) ? stream_state_error_input : stream_state_success;
}

/**
 * Convert input read from `in` to uppercase and write to `out`.
 *
 * @param in Stream to read from
 * @param out Stream to write to
 */
static stream_state
convert_upper(FILE *in, FILE *out)
{
  int c;
  while ((c = fgetc(in)) != EOF)
    if (fputc(toupper(c), out) == EOF && ferror(out))
      return stream_state_error_output;
  return (ferror(in)) ? stream_state_error_input : stream_state_success;
}

/**
 * Macro determining the extension for executables on a system.
 */
#ifdef _WIN32
#define EXE_SUFFIX ".exe"
#else
#define EXE_SUFFIX
#endif  // _WIN32

PDCPL_PROGRAM_USAGE_DEF
(
  "Convert text input from stdin to lower or upper case.\n"
  "\n"
  "This program's operation mode depends on its invocation name. For example,\n"
  "if symlinked, copied, or renamed with `lower" EXE_SUFFIX
    "' as its new name, it will\n"
  "convert text input to lowercase. But if the name is `upper" EXE_SUFFIX
    "', it will\n"
  "instead convert text to uppercase. Otherwise, the program will simply\n"
  "print an error message and terminate."
)

PDCPL_ARG_MAIN
{
  PDCPL_PARSE_PROGRAM_OPTIONS();
  // stream operation status
  stream_state status;
  // operate based on name. for Windows, we also check .exe extension
  if (
#ifdef _WIN32
    !strcmp(PDCPL_PROGRAM_NAME, "lower.exe") ||
#endif  // _WIN32
    !strcmp(PDCPL_PROGRAM_NAME, "lower")
  )
    status = convert_lower(stdin, stdout);
  else if (
#ifdef _WIN32
    !strcmp(PDCPL_PROGRAM_NAME, "upper.exe") ||
#endif  // _WIN32
    !strcmp(PDCPL_PROGRAM_NAME, "upper")
  )
    status = convert_upper(stdin, stdout);
  // exit case returns failure directly
  else {
#ifdef _WIN32
    PDCPL_PRINT_ERROR(
      "error: program base name; must be lower, upper, lower.exe, upper.exe\n"
    );
#else
    PDCPL_PRINT_ERROR("error: program base name must be lower, upper\n");
#endif  // _WIN32
    return EXIT_FAILURE;
  }
  // handle error status and exit
  switch (status) {
    case stream_state_success:
      return EXIT_SUCCESS;
    case stream_state_error_input:
      PDCPL_PRINT_ERROR("error: input stream error");
      break;
    case stream_state_error_output:
      PDCPL_PRINT_ERROR("error: output stream error");
      break;
  }
  return EXIT_FAILURE;
}
