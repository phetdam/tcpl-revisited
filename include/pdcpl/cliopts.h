/**
 * @file cliopts.h
 * @author Derek Huang
 * @brief C/C++ header for CLI option handling
 * @copyright MIT License
 */

#ifndef PDCPL_CLIOPTS_H_
#define PDCPL_CLIOPTS_H_

#include <ctype.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pdcpl/common.h"
#include "pdcpl/core.h"
#include "pdcpl/version.h"

PDCPL_EXTERN_C_BEGIN

// name for static global holding the program usage (before option description)
#define PDCPL_PROGRAM_USAGE pdcpl_main_program_usage
// name for static global holding general program options description
#define PDCPL_PROGRAM_OPTIONS pdcpl_main_program_options
// name for static global holding the program epilog (after option description)
#define PDCPL_PROGRAM_EPILOG pdcpl_main_program_epilog

/**
 * Enum for predefined `pdcpl` program option parsing status values.
 */
typedef enum {
  PDCPL_CLIOPT_PARSE_OK,
  PDCPL_CLIOPT_ERROR_CANT_CONVERT = 1,
  PDCPL_CLIOPT_STATUS_MAX
} pdcpl_cliopt_status;

/**
 * Typedef for a function to perform an action given `argc` and `argv`.
 *
 * Each `pdcpl_clioption` has an `action` slot that should be bound to a
 * function or set to `NULL` to not take any action.
 *
 * @param argc `argc` from `main()`
 * @param argv `argv` from `main()`
 * @param argi Index of `argv` we are currently at
 * @returns 0 on success, with negative errno values, `pdcpl_cliopt_status`
 *  values, and positive values > `PDCPL_CLIOPT_ERROR_MAX` indicating failure.
 */
typedef int (*pdcpl_cliopt_action)(int argc, char **argv, int argi);

/**
 * Macro for declaring a `pdcpl_cliopt_action`.
 *
 * @param name Name of the function
 */
#define PDCPL_CLIOPT_ACTION(name) int name(int argc, char **argv, int argi)

// forward declaration for the pdcpl_clioption type
typedef struct pdcpl_clioption pdcpl_clioption;

/**
 * Typedef for a function to handle any errors indicated by the action.
 *
 * @param argc `argc` from `main()`
 * @param argv `argv` from `main()`
 * @param opt Address to `pdcpl_clioption` to read information from
 * @param argi Index of `argv` we are currently at
 * @param err Status value returned by associated action slot
 * @returns 0 on success, nonzero values for failure
 */
typedef int (*pdcpl_cliopt_errhandler)(
  int argc, char **argv, pdcpl_clioption *opt, int argi, int err);

/**
 * Macro for declaring a `pdcpl_cliopt_errhandler`.
 *
 * @param name Name of the function.
 */
#define PDCPL_CLIOPT_ERRHANDLER(name) \
  int name(int argc, char **argv, pdcpl_clioption *opt, int argi, int err)

/**
 * Struct to hold information about a CLI option.
 *
 * @param name Option name, is option short name if long_name is provided
 * @param long_name Option long name, can be left as `NULL`
 * @param help Option help text, can be left as `NULL`
 * @param nargs Number of args consumed by option (not including itself)
 * @param action Action handler
 * @param errhandler Error handler
 */
typedef struct pdcpl_clioption {
  const char *name;
  const char *long_name;
  const char *help;
  unsigned int nargs;
  pdcpl_cliopt_action action;
  pdcpl_cliopt_errhandler errhandler;
} pdcpl_clioption;

/**
 * Print an error message to stderr preceded by the program name.
 *
 * @param message String literal message
 */
#define PDCPL_PRINT_ERROR(message) \
  fprintf(stderr, "%s: " message, PDCPL_PROGRAM_NAME)

/**
 * Print an error message to stderr preceded by the program name.
 *
 * Allows `printf`-style formatting.
 *
 * @param message `printf` format literal message
 * @param ... Variadic list of arguments for format specifiers
 */
#define PDCPL_PRINT_ERROR_EX(message, ...) \
  fprintf(stderr, "%s: " message, PDCPL_PROGRAM_NAME, __VA_ARGS__)

/**
 * The generic error handler that is used on action error.
 *
 * If `errhandler` is `NULL` for a `pdcpl_clioption`, this is used instead.
 *
 * TODO: Handle some `pdcpl_cliopt_status` values and errno errors.
 *
 * @param argc `argc` from `main()`
 * @param argv `argv` from `main()`
 * @param opt Address to `pdcpl_clioption` to read information from
 * @param argi Index of `argv` we are currently at
 * @param err Status value returned by associated action slot (unused)
 * @returns 0 on success, nonzero values for failure
 */
static
PDCPL_CLIOPT_ERRHANDLER(pdcpl_default_opterrhandler)
{
  // set program name if not set + handle some fatal errors
  PDCPL_SET_PROGRAM_NAME();
  if (!argv) {
    PDCPL_PRINT_ERROR("fatal error: argv is NULL\n");
    return 2;
  }
  if (argi < 0 || argi >= argc) {
    PDCPL_PRINT_ERROR_EX("fatal error: invalid argv index %d\n", argi);
    return 2;
  }
  if (!opt) {
    PDCPL_PRINT_ERROR("fatal error: cannot handle NULL option\n");
    return 2;
  }
  // doesn't actually do anything with err, so suppress compiler warning
  (void) err;
  // report error based on information from opt
  PDCPL_PRINT_ERROR_EX("error: %s", opt->name);
  if (opt->long_name)
    fprintf(stderr, ", %s", opt->long_name);
  fprintf(stderr, " parsing error, expected %u arguments.", opt->nargs);
  // prints next argument, unless there is no next argument
  if (argi + 1 < argc)
    fprintf(stderr, " current arg: %s\n", argv[argi + 1]);
  else
    fprintf(stderr, " no args provided\n");
  return EXIT_FAILURE;
}

/**
 * Macro defining `PDCPL_PROGRAM_USAGE`, the program usage static global.
 *
 * To use this macro, `PDCPL_HAS_PROGRAM_USAGE` must be defined before
 * including the `cliopts.h` header in application code.
 *
 * @param text String literal for the program usage
 */
#define PDCPL_PROGRAM_USAGE_DEF(text) \
  static const char *PDCPL_PROGRAM_USAGE = text;

/**
 * Macro defining `PDCPL_PROGRAM_EPILOG`, the program epilog static global.
 *
 * To use this macro, `PDCPL_HAS_PROGRAM_EPILOG` must be defined before
 * including the `cliopts.h` header in application code.
 *
 * @param text String literal for the program epilog
 */
#define PDCPL_PROGRAM_EPILOG_DEF(text) \
  static const char *PDCPL_PROGRAM_EPILOG = text;

/**
 * Macro used for defining the static array of program option structs.
 *
 * To use this macro, `PDCPL_HAS_PROGRAM_OPTIONS` must be defined before
 * including the `cliopts.h` header in application code. The last member of the
 * struct array must be the `PDCPL_PROGRAM_OPTIONS_END` sentinel member.
 */
#define PDCPL_PROGRAM_OPTIONS_DEF \
  static pdcpl_clioption PDCPL_PROGRAM_OPTIONS[] =

/**
 * Sentinel `pdcpl_clioption` member for `PDCPL_PROGRAM_OPTIONS`.
 */
#define PDCPL_PROGRAM_OPTIONS_END {NULL, NULL, NULL, 0, NULL, NULL}

// define PDCPL_HAS_PROGRAM_USAGE to provide your own program help usage
#ifndef PDCPL_HAS_PROGRAM_USAGE
PDCPL_PROGRAM_USAGE_DEF("")
#endif  // PDCPL_HAS_PROGRAM_USAGE

// define PDCPL_HAS_PROGRAM_OPTIONS to provide your own program options
#ifndef PDCPL_HAS_PROGRAM_OPTIONS
PDCPL_PROGRAM_OPTIONS_DEF {PDCPL_PROGRAM_OPTIONS_END};
#endif  // PDCPL_HAS_PROGRAM_OPTIONS

// define PDCPL_HAS_PROGRAM_EPILOG to provide your own program help epilog
#ifndef PDCPL_HAS_PROGRAM_EPILOG
PDCPL_PROGRAM_EPILOG_DEF("")
#endif  // PDCPL_NO_PROGRAM_EPILOG

/**
 * Return number of non-sentinel members in a `pdcpl_clioption` array.
 *
 * Behavior obviously undefined if `PDCPL_PROGRAM_OPTIONS_END` is missing from
 * the array of `pdcpl_clioption` structs or if the array is `NULL`.
 *
 * @param opts Options array terminated with `PDCPL_PROGRAM_OPTIONS_END`
 */
PDCPL_INLINE unsigned int
pdcpl_program_options_count(const pdcpl_clioption *opts)
{
  unsigned int n_opts;
  for (n_opts = 0; opts->name; opts++)
    n_opts++;
  return n_opts;
}

/**
 * Return number of non-sentinel members in `PDCPL_PROGRAM_OPTIONS`.
 *
 * Behavior obviously undefined if `PDCPL_PROGRAM_OPTIONS_END` is missing from
 * the `PDCPL_PROGRAM_OPTIONS` static array of `pdcpl_clioption` structs.
 *
 * @returns `unsigned int` number of program options
 */
#define PDCPL_PROGRAM_OPTIONS_COUNT() \
  pdcpl_program_options_count(PDCPL_PROGRAM_OPTIONS)

/**
 * Return program option print offset.
 *
 * The print offset for a program option `opt` is defined as:
 *
 *  2 + strlen(opt.name) + ((opt.long_name) ? 2 + strlen(opt.long_name) : 0)
 *
 * The first 2 is to provide indentation, the second 2 accommodates the extra
 * ", " that is needed before printing the long option name.
 *
 * @param opt Address to `pdcpl_clioption` to get print offset for
 * @returns 0 on error, 2 or greater for any success
 */
PDCPL_INLINE size_t
pdcpl_program_option_print_offset(const pdcpl_clioption *opt)
{
  size_t offset = 2;
  // malformed option or sentinel
  if (!opt || !opt->name)
    return 0;
  offset += strlen(opt->name);
  // need an extra 2 for space and comma
  if (opt->long_name)
    offset += strlen(opt->long_name) + 2;
  return offset;
}

/**
 * "Magic" program option target column offset value.
 *
 * Program options that have a print offset less than this will have help text
 * printed started on the same line from this column (starting from 0),
 * otherwise the help text will start on this column on the next line.
 */
#define PDCPL_PROGRAM_OPTION_COL_OFFSET 20

/**
 * Helper for printing a specified number of spaces
 *
 * @param n Number of spaces to print
 */
static void
pdcpl_print_spaces(size_t n)
{
  for (size_t i = 0; i < n; i++)
    putchar(' ');
}

/**
 * Helper for printing part of a string without checking its size.
 *
 * @param s String to print part of
 * @param si Index to start printing at (included)
 * @param ei Index to stop printing at (excluded)
 */
static void
pdcpl_print_substring_unchecked(const char *s, size_t si, size_t ei)
{
  for (size_t i = si; i < ei; i++)
    putchar(s[i]);
}

/**
 * Print the formatted help text for a program option wrapped to 80 cols.
 *
 * @param opt Address to `pdcpl_clioption` whose help will be printed
 * @returns 0 on success, -EINVAL if option is malformed
 */
static int
pdcpl_program_option_print_help(const pdcpl_clioption *opt)
{
  // print nice errors for each case
  if (!opt) {
    PDCPL_PRINT_ERROR("fatal error: encountered NULL program option\n");
    return -EINVAL;
  }
  if (!opt->name) {
    PDCPL_PRINT_ERROR("fatal error: option with NULL name\n");
    return -EINVAL;
  }
  // print short and long name (if there is long name)
  printf("  %s", opt->name);
  if (opt->long_name)
    printf(", %s", opt->long_name);
  // if no help, just print newline and move on
  if (!opt->help) {
    putchar('\n');
    return 0;
  }
  // compute options print offset (no need to check, opt->name is not NULL)
  {
    size_t offset = pdcpl_program_option_print_offset(opt);
    // if offset exceeds PDCPL_PROGRAM_OPTION_COL_OFFSET - 2, print a newline
    // and PDCPL_PROGRAM_OPTION_COL_OFFSET spaces, otherwise just the diff
    if (offset + 2 > PDCPL_PROGRAM_OPTION_COL_OFFSET) {
      putchar('\n');
      offset = PDCPL_PROGRAM_OPTION_COL_OFFSET;
    }
    else
      offset = PDCPL_PROGRAM_OPTION_COL_OFFSET - offset;
    pdcpl_print_spaces(offset);
  }
  // help length, position of last whitespace
  size_t help_len = strlen(opt->help);
  size_t wprev_i = SIZE_MAX;
  // chars printed in the current line, current char, prev whitespace char
  size_t n_line_chars = PDCPL_PROGRAM_OPTION_COL_OFFSET;
  char c, wc;
  // print chars in segments, printing newline when necessary
  for (size_t i = 0; i < help_len; i++) {
    // for any whitespace, e.g. tab, newline, space
    if (c = opt->help[i], isspace(c)) {
      // offset is PDCPL_PROGRAM_OPTION_COL_OFFSET. if number of chars in line
      // plus the expected number of chars to print exceeds 80 cols and this
      // is not the first line, we go to next line and reset chars in line
      if (n_line_chars + (i - wprev_i) >= 80) {
        // if wprev_i not set, i.e. this is a long first line, treat as zero
        if (wprev_i == SIZE_MAX)
          wprev_i = 0;
        else {
          putchar('\n');
          pdcpl_print_spaces(PDCPL_PROGRAM_OPTION_COL_OFFSET);
          n_line_chars = PDCPL_PROGRAM_OPTION_COL_OFFSET;
        }
      }
      // otherwise, we print the whitespace and also increment n_line_chars
      else {
        wc = opt->help[wprev_i];
        putchar(wc);
        // if newline, also print the spaces and update n_line_chars instead
        if (wc == '\n') {
          pdcpl_print_spaces(PDCPL_PROGRAM_OPTION_COL_OFFSET);
          n_line_chars = PDCPL_PROGRAM_OPTION_COL_OFFSET;
        }
        else
          n_line_chars++;
      }
      // print chars between wprev_i, i exclusive (allows spill)
      pdcpl_print_substring_unchecked(opt->help, wprev_i + 1, i);
      // update n_line_chars and wprev_i
      n_line_chars += i - wprev_i - 1;
      wprev_i = i;
    }
  }
  // print last bit of chars (include last wprev_i) + newline and we are done
  pdcpl_print_substring_unchecked(opt->help, wprev_i, help_len);
  putchar('\n');
  return 0;
}

/**
 * Print the help text for all the program options.
 *
 * If any options are malformed, an appropriate error message is printed to
 * `stderr` and the return value of this function will be nonzero.
 *
 * @param opts Options array terminated with `PDCPL_PROGRAM_OPTIONS_END`
 * @returns 0 on success, -EINVAL if `opts` is `NULL` or if an error was
 *  encountered during the process of printing the program option help
 */
static int
pdcpl_program_options_printf(const pdcpl_clioption *opts)
{
  if (!opts)
    return -EINVAL;
  // number of options (no need to print if there are no options)
  unsigned int n_opts = pdcpl_program_options_count(opts);
  if (!n_opts)
    return 0;
  // print the header and print options
  printf("General options:\n");
  for (unsigned int i = 0; i < n_opts; i++) {
    // if there is an error, the function prints its own error message
    if (pdcpl_program_option_print_help(opts + i))
      return -EINVAL;
  }
  // need extra newline for spacing
  putchar('\n');
  return 0;
}

/**
 * Print the program usage info.
 *
 * If there is no user-provided usage, options, or epilog, they are left empty.
 * If user-provided program options are malformed, an error message will be
 * printed and the program will exit with `EXIT_FAILURE`.
 */
#define PDCPL_PRINT_USAGE_INFO() \
  do { \
    PDCPL_SET_PROGRAM_NAME(); \
    printf("Usage: %s [OPTIONS...]\n\n", PDCPL_PROGRAM_NAME); \
    if (strlen(PDCPL_PROGRAM_USAGE)) \
      printf("%s\n\n", PDCPL_PROGRAM_USAGE); \
    if (pdcpl_program_options_printf(PDCPL_PROGRAM_OPTIONS)) \
      return EXIT_FAILURE; \
    printf( \
      "Info options:\n" \
      "  -h, --help        Print this help output\n" \
      "  -V, --version     Print program version info\n" \
    ); \
    if (strlen(PDCPL_PROGRAM_EPILOG)) \
      printf("\n%s\n", PDCPL_PROGRAM_EPILOG); \
  } \
  while (0)

/**
 * Print help or version info and exit a `PDCPL_ARG_MAIN` as appropriate.
 *
 * If `-h`, `--help` is seen on the command line, the program usage is printed
 * before exit, while if `-V`, `--version` is seen on the command line, the
 * program version info is printed before exit.
 */
#define PDCPL_HANDLE_INFO_OPTS() \
  do { \
    PDCPL_SET_PROGRAM_NAME(); \
    for (int i = 1; i < PDCPL_ARGC; i++) { \
      if (!strcmp(PDCPL_ARGV[i], "-h") || !strcmp(PDCPL_ARGV[i], "--help")) { \
        PDCPL_PRINT_USAGE_INFO(); \
        return EXIT_SUCCESS; \
      } \
      if ( \
        !strcmp(PDCPL_ARGV[i], "-V") || !strcmp(PDCPL_ARGV[i], "--version") \
      ) { \
        PDCPL_PRINT_VERSION_INFO(); \
        return EXIT_SUCCESS; \
      } \
    } \
  } \
  while (0)

/**
 * Parse optional arguments passed to a `PDCPL_ARG_MAIN` appropriately.
 *
 * TODO: Remove conditional compile later, see `pdcpl_default_opterrhandler`
 *
 * Prints program usage and exits if `-h`, `--help` is specified and prints
 * program version info and exits if `-V`, `--version` is specified. If there
 * are user-defined program options, i.e. `PDCPL_HAS_PROGRAM_OPTIONS` is
 * defined, these options are parsed and their actions are executed. Any errors
 * are handled per option using an option-specific error handler or the
 * default error handler if none is provided for the option.
 *
 * If the option is not associated with an action, i.e. action is `NULL`, then
 * the option and its required args are silently skipped.
 */
#define PDCPL_PARSE_PROGRAM_OPTIONS() \
  do { \
    PDCPL_HANDLE_INFO_OPTS(); \
    pdcpl_clioption *prog_options = PDCPL_PROGRAM_OPTIONS; \
    pdcpl_clioption *cur_opt = NULL; \
    int opt_status; \
    for (int i = 1; i < PDCPL_ARGC; i++) { \
      while (prog_options->name) { \
        if ( \
          !strcmp(PDCPL_ARGV[i], prog_options->name) || \
          ( \
            prog_options->long_name && \
            !strcmp(PDCPL_ARGV[i], prog_options->long_name) \
          ) \
        ) { \
          cur_opt = prog_options; \
          break; \
        } \
        prog_options++; \
      } \
      if (!cur_opt) { \
        PDCPL_PRINT_ERROR_EX("error: unknown option %s\n", PDCPL_ARGV[i]); \
        return EXIT_FAILURE; \
      } \
      if (cur_opt->action) \
        opt_status = cur_opt->action(argc, argv, i); \
      else \
        opt_status = 0; \
      if (opt_status) { \
        if (cur_opt->errhandler) \
          return cur_opt->errhandler(argc, argv, cur_opt, i, opt_status); \
        return pdcpl_default_opterrhandler(argc, argv, cur_opt, i, opt_status); \
      } \
      i += (int) cur_opt->nargs; \
      cur_opt = NULL; \
    } \
  } \
  while (0)

PDCPL_EXTERN_C_END

#endif  // PDCPL_CLIOPTS_H_
