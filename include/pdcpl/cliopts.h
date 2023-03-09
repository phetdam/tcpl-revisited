/**
 * @file cliopts.h
 * @author Derek Huang
 * @brief C/C++ header for CLI option handling
 * @copyright MIT License
 */

#ifndef PDCPL_CLIOPTS_H_
#define PDCPL_CLIOPTS_H_

#include <errno.h>
#include <stddef.h>
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
 * Typedef for a function to perform an action given `argc` and `argv`.
 *
 * Each `pdcpl_clioption` has an `action` slot that should be bound to a
 * function or set to `NULL` to not take any action.
 *
 * On action success, the action should return 0, otherwise failure.
 *
 * @param argc `argc` from `main()`
 * @param argv `argv` from `main()`
 * @param argi Index of `argv` we are currently at
 */
typedef int (*pdcpl_optaction)(int argc, char **argv, int argi);

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
 */
typedef int (*pdcpl_opterrhandler)(
  int argc, char **argv, pdcpl_clioption *opt, int argi, int err);

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
  pdcpl_optaction action;
  pdcpl_opterrhandler errhandler;
} pdcpl_clioption;

/**
 * The generic error handler that is used on action error.
 *
 * If `errhandler` is `NULL` for a `pdcpl_clioption`, this is used instead.
 *
 * @param argc `argc` from `main()`
 * @param argv `argv` from `main()`
 * @param opt Address to `pdcpl_clioption` to read information from
 * @param argi Index of `argv` we are currently at
 * @param err Status value returned by associated action slot (unused)
 */
static int
pdcpl_default_opterrhandler(
  int argc, char **argv, pdcpl_clioption *opt, int argi, int err)
{
  // set program name if not set + handle some fatal errors
  PDCPL_SET_PROGRAM_NAME();
  if (!argv) {
    fprintf(stderr, "%s: fatal error: argv is NULL\n", PDCPL_PROGRAM_NAME);
    return 2;
  }
  if (argi < 0 || argi >= argc) {
    fprintf(
      stderr,
      "%s: fatal error: invalid argv index %d", PDCPL_PROGRAM_NAME, argi
    );
    return 2;
  }
  if (!opt) {
    fprintf(
      stderr,
      "%s: fatal error: cannot handle error for NULL option\n",
      PDCPL_PROGRAM_NAME
    );
    return 2;
  }
  // doesn't actually do anything with err, so suppress compiler warning
  (void) err;
  // report error based on information from opt
  fprintf(stderr, "%s: error: %s", PDCPL_PROGRAM_NAME, opt->name);
  if (opt->long_name)
    fprintf(stderr, ", %s", opt->long_name);
  fprintf(
    stderr,
    " parsing error, expected %u arguments. current arg: %s\n",
    opt->nargs,
    argv[argi]
  );
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
 * Sentinel `pdcpl_clioption` member
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
 * Print the formatted help text for a program option wrapped to 80 cols.
 *
 * TODO: Currently just prints the help text verbatim, no wrapping
 *
 * @param opt Address to `pdcpl_clioption` whose help will be printed
 * @returns 0 on success, -EINVAL if option is malformed
 */
static int
pdcpl_program_option_print_help(const pdcpl_clioption *opt)
{
  // print nice errors for each case
  if (!opt) {
    fprintf(
      stderr,
      "%s: fatal error: encountered NULL program option\n",
      PDCPL_PROGRAM_NAME
    );
    return -EINVAL;
  }
  if (!opt->name) {
    fprintf(
      stderr, "%s: fatal error: option with NULL name\n", PDCPL_PROGRAM_NAME
    );
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
    for (size_t i = 0; i < offset; i++)
      putchar(' ');
  }
  // now print the help. we first make a copy so we can edit contents and then
  // replace single spaces with newlines as appropriate to meet 80 - col_offset
  // column width per line, although long words that exceed this length are
  // allowed to continue unbroken (makes implementation easy and is natural).
  // char *help = malloc((strlen(opt->help) + 1) * sizeof *help);
  // if (!help)
  //   return -ENOMEM;
  // free(help);
  printf("%s\n", opt->help);
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
        fprintf( \
          stderr, \
          "%s: error: unknown option %s\n", \
          PDCPL_PROGRAM_NAME, \
          PDCPL_ARGV[i] \
        ); \
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
