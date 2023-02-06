/**
 * @file 1.8.c
 * @author Derek Huang
 * @brief The C Programming Language exercise 1-8
 * @copyright MIT License
 */

#include <stdio.h>
#include <stdlib.h>

#define PDCPL_HAS_PROGRAM_USAGE
#include "pdcpl/cliopts.h"
#include "pdcpl/core.h"

PDCPL_PROGRAM_USAGE_DEF
(
  "Prints the number of blanks, tabs, and newlines read from stdin.\n"
  "\n"
  "Results are displayed with the total character count in wc style, except\n"
  "with column headers. E.g. if `printf \"\\t\\nhello my name is dan\"' were\n"
  "piped into this program, one would get output like\n"
  "\n"
  "   blanks      tabs  newlines     total\n"
  "        4         1         1        22"
)

PDCPL_ARG_MAIN
{
  PDCPL_HANDLE_INFO_OPTS();
  // getchar() output + number of blanks, tabs, newlines, total characters
  int c;
  size_t nb = 0, nt = 0, nn = 0, nc = 0;
  // loop for each character
  while ((c = getchar()) != EOF) {
    switch (c) {
      case ' ':
        nb++;
        break;
      case '\t':
        nt++;
        break;
      case '\n':
        nn++;
        break;
    }
    nc++;
  }
  // print out wc-style summary (but with headers)
  printf(
    "   blanks      tabs  newlines     total\n%9zu %9zu %9zu %9zu\n",
    nb, nt, nn, nc
  );
  return EXIT_SUCCESS;
}
