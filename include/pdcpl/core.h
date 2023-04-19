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

PDCPL_EXTERN_C_BEGIN

// macro for the program name
#define PDCPL_PROGRAM_NAME pdcpl_main_program_name

// program name. although this is translation-unit local, one really only needs
// this header in the translation unit that defines main().
static const char *PDCPL_PROGRAM_NAME = "";

// macros for common main() signatures
#define PDCPL_MAIN int main()
#define PDCPL_ARGC argc
#define PDCPL_ARGV argv
#define PDCPL_ARG_MAIN int main(int PDCPL_ARGC, char **PDCPL_ARGV)

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
 * Return `EXIT_FAILURE` from `main()` if `expr` evaluates to an `errno` value.
 *
 * Prints a message to stderr using `strerror`, where `expr` is expected to
 * resolve to an `errno` value or the negation of an `errno` value.
 *
 * @param expr Expression resolving to an `errno` value or its negation
 */
#define PDCPL_MAIN_ERRNO_EXIT(expr) \
  do { \
    int res; \
    if ((res = expr)) { \
      PDCPL_PRINT_ERROR_EX("%s\n", strerror((res < 0) ? -res : res)); \
      return EXIT_FAILURE; \
    } \
  } \
  while (0)

/**
 * Return `EXIT_FAILURE` from `main()` if `expr` evaluates to nonzero.
 *
 * @param expr Expression resolving to an integral value
 */
#define PDCPL_MAIN_EXIT(expr) if (expr) return EXIT_FAILURE

/**
 * Return `EXIT_FAILURE` from `main()` if `expr` evaluates to nonzero.
 *
 * Prints a user-defined message to stderr.
 *
 * @param expr Expression resolving to an integral value
 */
#define PDCPL_MAIN_EXIT_EX(expr, message) \
  do { \
    if (expr) { \
      PDCPL_PRINT_ERROR_EX("%s\n", message); \
      return EXIT_FAILURE; \
    } \
  } \
  while (0)

PDCPL_EXTERN_C_END

#endif  // PDCPL_CORE_H_
