/**
 * @file 4.14.c
 * @author Derek Huang
 * @brief The C Programming Language exercise 4-14
 * @copyright MIT License
 */

#include <stdio.h>
#include <stdlib.h>

#define PDCPL_HAS_PROGRAM_USAGE
#include "pdcpl/cliopts.h"
#include "pdcpl/common.h"
#include "pdcpl/core.h"
#include "pdcpl/utility.h"

/**
 * Demonstrate the `PDCPL_SWAP` macro on different types.
 *
 * Note that if `type` is a pointer type, the output printed by this macro is
 * no longer valid C code due to pointer initialization syntax.
 *
 * @param type Type of v1, v2
 * @param v1 First `type` value
 * @param v2 Second `type` value
 * @param fmt Format specifier for `printf` to print values correctly
 */
#define PDCPL_SWAP_DEMO(type, v1, v2, fmt) \
  do { \
    type x = v1; \
    type y = v2; \
    printf( \
      "%s x = " fmt ", y = " fmt ";\nPDCPL_SWAP(x, y)  ", \
      PDCPL_STRINGIFY(type), x, y \
    ); \
    PDCPL_SWAP(type, x, y); \
    printf("// x = " fmt ", y = " fmt "\n", x, y); \
  } \
  while (0)

/**
 * Typedef of `const char *` into a type.
 *
 * Added just for the sake of making the `PDCPL_SWAP_DEMO` output "correct" C,
 * as the macro's print implementation means that if `type` is `const char *`,
 * the corresponding print for the assignments would be something like
 *
 * const char * x = "first", y = "second";
 *
 * This is not correct C, as we should instead have `*y = "second"`.
 */
typedef const char *cstring;

PDCPL_PROGRAM_USAGE_DEF
(
  "Demonstrate the PDCPL_SWAP swap macro.\n"
  "\n"
  "Showcases the PDCPL_SWAP macro on a few different types. Note that the\n"
  "cstring type is really a typedef for const char *, added only so that the\n"
  "program will output syntactically correct C code.\n"
  "\n"
  "Of course, in C++ we can just use std::swap and not write our own macro."
)

PDCPL_ARG_MAIN
{
  PDCPL_HANDLE_INFO_OPTS();
  PDCPL_SWAP_DEMO(int, 4, 15, "%d");
  PDCPL_SWAP_DEMO(double, 14.4, 78.113, "%g");
  PDCPL_SWAP_DEMO(cstring, "first", "second", "\"%s\"");
  return EXIT_SUCCESS;
}
