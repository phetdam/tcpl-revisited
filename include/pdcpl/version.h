/**
 * @file version.h
 * @author Derek Huang
 * @brief C/C++ header for project version macros
 */

#ifndef PDCPL_VERSION_H_
#define PDCPL_VERSION_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pdcpl/core.h"

// major, minor, patch versions
#ifndef PDCPL_VERSION_MAJOR
#define PDCPL_VERSION_MAJOR 0
#endif  // PDCPL_VERSION_MAJOR
#ifndef PDCPL_VERSION_MINOR
#define PDCPL_VERSION_MINOR 0
#endif  // PDCPL_VERSION_MINOR
#ifndef PDCPL_VERSION_PATCH
#define PDCPL_VERSION_PATCH 0
#endif  // PDCPL_VERSION_PATCH

// version concatenation macros
#define PDCPL_VERSION_CONCAT_I(major, minor, patch) \
  major ## . ## minor ## . ## patch
#define PDCPL_VERSION_CONCAT(major, minor, patch) \
  PDCPL_VERSION_CONCAT_I(major, minor, patch)

// version token, version string
#define PDCPL_VERSION \
  PDCPL_VERSION_CONCAT( \
    PDCPL_VERSION_MAJOR, PDCPL_VERSION_MINOR, PDCPL_VERSION_PATCH \
  )
#define PDCPL_VERSION_STRING PDCPL_STRINGIFY(PDCPL_VERSION)

/**
 * Print version information from within a `PDCPL_ARG_MAIN`.
 *
 * If `PDCPL_GET_PROGNAME()` has not been called yet, program name is `NULL`.
 */
#define PDCPL_PRINT_VERSION_INFO() \
  printf("%s %s\n", PDCPL_PROGNAME, PDCPL_VERSION_STRING)

/**
 * Print version information and exit a `PDCPL_ARG_MAIN` if CLI option is seen.
 *
 * If `-V` or `--version` are passed to a program with a `PDCPL_ARG_MAIN`, this
 * macro can be used to print the version info and exit.
 */
#define PDCPL_HANDLE_OPT_VERSION \
  PDCPL_GET_PROGNAME(); \
  for (int i = 1; i < PDCPL_ARGC; i++) { \
    if (!strcmp(PDCPL_ARGV[i], "-V") || !strcmp(PDCPL_ARGV[i], "--version")) { \
      PDCPL_PRINT_VERSION_INFO(); \
      return EXIT_SUCCESS; \
    } \
  }

#endif  // PDCPL_VERSION_H_
