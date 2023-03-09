/**
 * @file 1.2.c
 * @author Derek Huang
 * @brief The C Programming Language exercise 1-2
 * @copyright MIT License
 */

#include <stdio.h>
#include <stdlib.h>

#define PDCPL_HAS_PROGRAM_USAGE
#include "pdcpl/cliopts.h"
#include "pdcpl/core.h"
#include "pdcpl/termcolors.h"

PDCPL_PROGRAM_USAGE_DEF("Prints \"hello, world\" to the screen in color.")

PDCPL_ARG_MAIN
{
  PDCPL_PARSE_PROGRAM_OPTIONS();
  PDCPL_PRINTF(
    PDCPL_GREEN_STRING("hello")
    PDCPL_CYAN_STRING(", ")
    PDCPL_RED_STRING("world\n")
  );
  return EXIT_SUCCESS;
}
