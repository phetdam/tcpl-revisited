/**
 * @file 2.1.c
 * @author Derek Huang
 * @brief The C Programming Language exercise 2-1
 * @copyright MIT License
 */

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PDCPL_HAS_PROGRAM_USAGE
#include "pdcpl/cliopts.h"
#include "pdcpl/core.h"

/**
 * Return the min value of an unsigned integral type.
 *
 * @param type C type
 */
#define PDCPL_UNSIGNED_MIN(type) ((type) 0)

/**
 * Return the max value of an unsigned integral type.
 *
 * @param type C type
 */
#define PDCPL_UNSIGNED_MAX(type) ((type) -1)

/**
 * Return the max positive value of a signed integral type.
 *
 * @param type C type
 */
#define PDCPL_SIGNED_MAX(type) ((type) (exp2(8 * sizeof(type) - 1) - 1))

/**
 * Return the max negative value of a signed integral type.
 *
 * @param type C type
 */
#define PDCPL_SIGNED_MIN(type) ((type) -exp2(8 * sizeof(type) - 1))

/**
 * Print a message with an unsigned type name and its [min, max] range.
 *
 * The type is printed right-justified and separated from the range by " : ".
 * For formatting consistency, the type name should be 16 chars at most.
 */
#define PDCPL_PRINT_UNSIGNED_RANGE(type) \
  printf( \
    "%16s : [%ju, %ju]\n", \
    PDCPL_STRINGIFY(type), \
    (uintmax_t) PDCPL_UNSIGNED_MIN(type), \
    (uintmax_t) PDCPL_UNSIGNED_MAX(type) \
  )

/**
 * Print a message with a signed type name and its [0, max] range.
 *
 * The type is printed right-justified and separated from the range by " : ".
 * For formatting consistency, the type name should be 16 chars at most.
 */
#define PDCPL_PRINT_SIGNED_RANGE(type) \
  printf( \
    "%16s : [%jd, %jd]\n", \
    PDCPL_STRINGIFY(type), \
    (intmax_t) PDCPL_SIGNED_MIN(type), \
    (intmax_t) PDCPL_SIGNED_MAX(type) \
  )

PDCPL_PROGRAM_USAGE_DEF
(
  "Prints the [min, max] ranges of commonly used integral types.\n"
  "\n"
  "On a 64-bit system, the output might look something like\n"
  "\n"
  "            char : [-128, 127]\n"
  "   unsigned char : [0, 255]\n"
  "           short : [-32768, 32767]\n"
  "  unsigned short : [0, 65535]\n"
  "             int : [-2147483648, 2147483647]\n"
  "    unsigned int : [0, 4294967295]\n"
  "            long : [-9223372036854775808, 9223372036854775807]\n"
  "   unsigned long : [0, 18446744073709551615]\n"
  "       ptrdiff_t : [-9223372036854775808, 9223372036854775807]\n"
  "          size_t : [0, 18446744073709551615]\n"
  "        intmax_t : [-9223372036854775808, 9223372036854775807]\n"
  "       uintmax_t : [0, 18446744073709551615]"
)

PDCPL_ARG_MAIN
{
  PDCPL_HANDLE_INFO_OPTS();
  PDCPL_PRINT_SIGNED_RANGE(char);
  PDCPL_PRINT_UNSIGNED_RANGE(unsigned char);
  PDCPL_PRINT_SIGNED_RANGE(short);
  PDCPL_PRINT_UNSIGNED_RANGE(unsigned short);
  PDCPL_PRINT_SIGNED_RANGE(int);
  PDCPL_PRINT_UNSIGNED_RANGE(unsigned int);
  PDCPL_PRINT_SIGNED_RANGE(long);
  PDCPL_PRINT_UNSIGNED_RANGE(unsigned long);
  // other integral types that were not requested but of practical interest
  PDCPL_PRINT_SIGNED_RANGE(ptrdiff_t);
  PDCPL_PRINT_UNSIGNED_RANGE(size_t);
  PDCPL_PRINT_SIGNED_RANGE(intmax_t);
  PDCPL_PRINT_UNSIGNED_RANGE(uintmax_t);
  return EXIT_SUCCESS;
}
