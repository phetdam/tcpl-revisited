/**
 * @file 1.7.c
 * @author Derek Huang
 * @brief The C Programming Language exercise 1-7
 * @copyright MIT License
 */

#include <stdio.h>
#include <stdlib.h>

#define PDCPL_HAS_PROGRAM_USAGE
#include "pdcpl/cliopts.h"
#include "pdcpl/core.h"

PDCPL_PROGRAM_USAGE_DEF("Prints the value of EOF as an integer.")

PDCPL_ARG_MAIN
{
  PDCPL_PARSE_PROGRAM_OPTIONS();
  printf("EOF = %d\n", EOF);
  return EXIT_SUCCESS;
}
