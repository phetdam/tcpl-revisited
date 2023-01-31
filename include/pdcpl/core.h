/**
 * @file core.h
 * @author Derek Huang
 * @brief C/C++ core project header
 * @copyright MIT License
 */

#ifndef PDCPL_MACROS_H_
#define PDCPL_MACROS_H_

#include <string.h>

// extern "C" scoping macros
#ifdef __cplusplus
#define PDCPL_EXTERN_C_BEGIN extern "C" {
#define PDCPL_EXTERN_C_END }
#else
#define PDCPL_EXTERN_C_BEGIN
#define PDCPL_EXTERN_C_END
#endif  // __cplusplus

// stringification macros
#define PDCPL_STRINGIFY_I(x) #x
#define PDCPL_STRINGIFY(x) PDCPL_STRINGIFY_I(x)

// concatenation macros
#define PDCPL_CONCAT_I(x, y) x ## y
#define PDCPL_CONCAT(x, y) PDCPL_CONCAT_I(x, y)

// macro for the program name
#define PDCPL_PROGNAME pdcpl_main_program_name

// macros for common main() signatures
#define PDCPL_MAIN int main()
#define PDCPL_ARGC argc
#define PDCPL_ARGV argv
#define PDCPL_ARG_MAIN \
  static const char *PDCPL_PROGNAME; \
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
 * Macro to get program name from `PDCPL_ARGC` and initialize `PDCPL_PROGNAME`.
 *
 * The body is not run again if `PDCPL_PROGNAME` has already been set.
 */
#define PDCPL_GET_PROGNAME() \
  do { \
    if (PDCPL_PROGNAME) \
      break; \
    PDCPL_PROGNAME = strrchr(PDCPL_ARGV[0], PDCPL_PATH_SEP_CHAR); \
    if (!PDCPL_PROGNAME) \
      PDCPL_PROGNAME = PDCPL_ARGV[0]; \
    else \
      PDCPL_PROGNAME++; \
  } \
  while (0)

#endif  // PDCPL_MACROS_H_
