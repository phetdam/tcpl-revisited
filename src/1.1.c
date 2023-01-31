/**
 * @file 1.1.c
 * @author Derek Huang
 * @brief The C Programming Language exercise 1-1
 * @copyright MIT License
 */

#include <stdio.h>
#include <stdlib.h>

#include "pdcpl/core.h"
#include "pdcpl/version.h"

PDCPL_ARG_MAIN
{
  PDCPL_HANDLE_OPT_VERSION
  printf("hello, world\n");
  return EXIT_SUCCESS;
}
