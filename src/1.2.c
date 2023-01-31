/**
 * @file 1.2.c
 * @author Derek Huang
 * @brief The C Programming Language exercise 1-2
 * @copyright MIT License
 */

#include <stdio.h>
#include <stdlib.h>

#include "pdcpl/core.h"
#include "pdcpl/termcolors.h"
#include "pdcpl/version.h"

PDCPL_ARG_MAIN
{
  PDCPL_HANDLE_OPT_VERSION
  printf(PDCPL_GREEN "hello" PDCPL_CYAN ", " PDCPL_RED "world\n" PDCPL_NO_COLOR);
  return EXIT_SUCCESS;
}
