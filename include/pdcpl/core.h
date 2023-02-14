/**
 * @file core.h
 * @author Derek Huang
 * @brief C/C++ core project header
 * @copyright MIT License
 */

#ifndef PDCPL_CORE_H_
#define PDCPL_CORE_H_

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

// allow C++-like use of inline in C code
#ifdef __cplusplus
#define PDCPL_INLINE inline
#else
#define PDCPL_INLINE static inline
#endif  // __cplusplus

// macro to indicate extern inline, i.e. never static inline
#define PDCPL_EXTERN_INLINE inline

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
