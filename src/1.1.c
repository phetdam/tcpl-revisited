/**
 * @file 1.1.c
 * @author Derek Huang
 * @brief The C Programming Language exercise 1-1
 * @copyright MIT License
 */

#include <stdio.h>
#include <stdlib.h>

#define PDCPL_HAS_PROGRAM_USAGE
#include "pdcpl/cliopts.h"
#include "pdcpl/core.h"

PDCPL_PROGRAM_USAGE_DEF("Prints \"hello, world\" to the screen.")

PDCPL_ARG_MAIN
{
  PDCPL_HANDLE_INFO_OPTS();
  printf("hello, world\n");
  return EXIT_SUCCESS;
}
