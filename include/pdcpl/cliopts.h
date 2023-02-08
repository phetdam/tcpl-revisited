/**
 * @file cliopts.h
 * @author Derek Huang
 * @brief C/C++ header for CLI option handling
 * @copyright MIT License
 */

#ifndef PDCPL_CLIOPTS_H_
#define PDCPL_CLIOPTS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pdcpl/core.h"
#include "pdcpl/version.h"

// name for static global holding the program usage (before option description)
#define PDCPL_PROGRAM_USAGE pdcpl_main_program_usage
// name for static global holding general program options description
#define PDPCL_PROGRAM_OPTIONS pdcpl_main_program_options
// name for static global holding the program epilog (after option description)
#define PDCPL_PROGRAM_EPILOG pdcpl_main_program_epilog

/**
 * Macro defining `PDCPL_PROGRAM_USAGE`, the program usage static global.
 *
 * To use this macro, `PDCPL_HAS_PROGRAM_USAGE` must be defined before
 * including the `cliopts.h` header in application code.
 *
 * @param text String literal for the program usage
 */
#define PDCPL_PROGRAM_USAGE_DEF(text) \
  static const char* PDCPL_PROGRAM_USAGE = text;

/**
 * Macro defining `PDCPL_PROGRAM_OPTIONS`, the program options static global.
 *
 * To use this macro, `PDCPL_HAS_PROGRAM_OPTIONS` must be defined before
 * including the `cliopts.h` header in application code.
 *
 * @param text String literal for the program options description
 */
#define PDCPL_PROGRAM_OPTIONS_DEF(text) \
  static const char* PDCPL_PROGRAM_OPTIONS = text;

/**
 * Macro defining `PDCPL_PROGRAM_EPILOG`, the program epilog static global.
 *
 * To use this macro, `PDCPL_HAS_PROGRAM_EPILOG` must be defined before
 * including the `cliopts.h` header in application code.
 *
 * @param text String literal for the program epilog
 */
#define PDCPL_PROGRAM_EPILOG_DEF(text) \
  static const char* PDCPL_PROGRAM_EPILOG = text;

// define PDCPL_HAS_PROGRAM_USAGE to provide your own program help usage
#ifndef PDCPL_HAS_PROGRAM_USAGE
PDCPL_PROGRAM_USAGE_DEF("")
#endif  // PDCPL_HAS_PROGRAM_USAGE

// define PDCPL_HAS_PROGRAM_OPTIONS to provide your own program options help
#ifndef PDCPL_HAS_PROGRAM_OPTIONS
PDCPL_PROGRAM_OPTIONS_DEF("")
#endif  // PDCPL_HAS_PROGRAM_OPTIONS

// define PDCPL_HAS_PROGRAM_EPILOG to provide your own program help epilog
#ifndef PDCPL_HAS_PROGRAM_EPILOG
PDCPL_PROGRAM_EPILOG_DEF("")
#endif  // PDCPL_NO_PROGRAM_EPILOG

/**
 * Print the program usage info.
 *
 * If there is no user-provided usage, options, or epilog, they are left empty.
 */
#define PDCPL_PRINT_USAGE_INFO() \
  do { \
    PDCPL_SET_PROGRAM_NAME(); \
    printf("Usage: %s [OPTIONS...]\n\n", PDCPL_PROGRAM_NAME); \
    if (strlen(PDCPL_PROGRAM_USAGE)) \
      printf("%s\n\n", PDCPL_PROGRAM_USAGE); \
    if (strlen(PDCPL_PROGRAM_OPTIONS)) \
      printf("General options:\n%s\n\n", PDCPL_PROGRAM_OPTIONS); \
    printf( \
      "Info options:\n" \
      "  -h, --help       Print this help output\n" \
      "  -V, --version    Print program version info\n" \
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
        return EXIT_SUCCESS;\
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

#endif  // PDCPL_CLIOPTS_H_
