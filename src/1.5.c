/**
 * @file 1.5.c
 * @author Derek Huang
 * @brief The C Programming Language exercise 1-5
 * @copyright MIT License
 */

#include <stdio.h>
#include <stdlib.h>

#define PDCPL_HAS_PROGRAM_USAGE
#include "pdcpl/cliopts.h"
#include "pdcpl/core.h"
#include "pdcpl/misc.h"

// lower, upper, and step for Fahrenheit values
#define FAHR_LOWER 0
#define FAHR_UPPER 300
#define FAHR_STEP 20

PDCPL_PROGRAM_USAGE_DEF
(
  "Prints a Fahrenheit to Celsius conversion table with column headers."
  "\n\n"
  "Fahrenheit values range from " PDCPL_STRINGIFY(FAHR_UPPER) " to "
  PDCPL_STRINGIFY(FAHR_LOWER) " inclusive, spaced by "
  PDCPL_STRINGIFY(FAHR_STEP) "."
)

PDCPL_ARG_MAIN
{
  PDCPL_HANDLE_INFO_OPTS();
  printf(
    "+--------+--------+\n"
    "|     F. |     C. |\n"
    "+--------+--------+\n"
  );
  for (int fahr = FAHR_UPPER; fahr >= FAHR_LOWER; fahr -= FAHR_STEP)
    printf("| %6.0f | %6.1f |\n", (double) fahr, pdcpl_f2ctemp(fahr));
  printf("+--------+--------+\n");
  return EXIT_SUCCESS;
}
