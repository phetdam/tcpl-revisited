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
  PDCPL_HANDLE_OPT_VERSION();
  PDCPL_PRINTF(
    PDCPL_GREEN_STRING("hello")
    PDCPL_CYAN_STRING(", ")
    PDCPL_RED_STRING("world\n")
  );
  return EXIT_SUCCESS;
}
