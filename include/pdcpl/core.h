/**
 * @file core.h
 * @author Derek Huang
 * @brief C/C++ core project header
 * @copyright MIT License
 */

#ifndef PDCPL_CORE_H_
#define PDCPL_CORE_H_

#include <string.h>

#include "pdcpl/common.h"

// macro for the program name
#define PDCPL_PROGRAM_NAME pdcpl_main_program_name

// macros for common main() signatures
#define PDCPL_MAIN int main()
#define PDCPL_ARGC argc
#define PDCPL_ARGV argv
#define PDCPL_ARG_MAIN \
  static const char *PDCPL_PROGRAM_NAME = ""; \
  int main(int PDCPL_ARGC, char **PDCPL_ARGV)

// path separator macros
#ifdef _WIN32
#define PDCPL_PATH_SEP_CHAR '\\'
#define PDCPL_PATH_SEP_STRING "\\"
#else
#define PDCPL_PATH_SEP_CHAR '/'
#define PDCPL_PATH_SEP_STRING "/"
#endif  // _WIN32

/**
 * Macro to set `PDCPL_PROGRAM_NAME` from `PDCPL_ARGC`.
 *
 * The body is not run again if `PDCPL_PROGRAM_NAME` has already been set.
 */
#define PDCPL_SET_PROGRAM_NAME() \
  do { \
    if (strlen(PDCPL_PROGRAM_NAME)) \
      break; \
    PDCPL_PROGRAM_NAME = strrchr(PDCPL_ARGV[0], PDCPL_PATH_SEP_CHAR); \
    if (!PDCPL_PROGRAM_NAME) \
      PDCPL_PROGRAM_NAME = PDCPL_ARGV[0]; \
    else \
      PDCPL_PROGRAM_NAME++; \
  } \
  while (0)

#endif  // PDCPL_CORE_H_
