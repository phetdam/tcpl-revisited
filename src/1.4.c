/**
 * @file 1.4.c
 * @author Derek Huang
 * @brief The C Programming Language exercise 1-4
 * @copyright MIT License
*/

#include <stdlib.h>
#include <stdio.h>

#define PDCPL_HAS_PROGRAM_USAGE
#include "pdcpl/core.h"
#include "pdcpl/cliopts.h"
#include "pdcpl/misc.h"

// lower, upper, and step for celenheit values
#define CEL_LOWER 0
#define CEL_UPPER 300
#define CEL_STEP 20

PDCPL_PROGRAM_USAGE_DEF
(
  "Prints a Celsius to Fahrenheit conversion table with column headers."
  "\n\n"
  "Celsius values range from " PDCPL_STRINGIFY(CEL_LOWER) " to "
  PDCPL_STRINGIFY(CEL_UPPER) " inclusive, spaced by "
  PDCPL_STRINGIFY(CEL_STEP) "."
)

PDCPL_ARG_MAIN
{
  PDCPL_HANDLE_INFO_OPTS();
  printf(
    "+--------+--------+\n"
    "|     C. |     F. |\n"
    "+--------+--------+\n"
  );
  for (int cel = CEL_LOWER; cel <= CEL_UPPER; cel += CEL_STEP)
    printf("| %6.0f | %6.1f |\n", (double) cel, pdcpl_c2ftemp(cel));
  printf("+--------+--------+\n");
  return EXIT_SUCCESS;
}
