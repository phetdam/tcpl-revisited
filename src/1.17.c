/**
 * @file 1.17.c
 * @author Derek Huang
 * @brief The C Programming Language exercise 1-17
 * @copyright MIT License
 */

#include <stdio.h>
#include <stdlib.h>

#define PDCPL_HAS_PROGRAM_USAGE
#include "pdcpl/cliopts.h"
#include "pdcpl/core.h"
#include "pdcpl/string.h"

PDCPL_PROGRAM_USAGE_DEF
(
  "Prints all lines read from stdin longer than 80 chars to stdout.\n"
  "\n"
  "Lines up to SIZE_MAX - 1 will be correctly printed while any lines that\n"
  "are SIZE_MAX or longer will trigger appropriate error handling and exit."
)

PDCPL_ARG_MAIN
{
  PDCPL_PARSE_PROGRAM_OPTIONS();
  // pointer to current line buffer and its size
  char *line;
  size_t len;
  // keep going until we reach EOF
  while (!feof(stdin)) {
    // read line from stdin, setting buffer address and line length
    if (pdcpl_getline(stdin, &line, &len))
      return EXIT_FAILURE;
    // print if greater than 80 chars in length + free
    if (len > 80)
      printf("%s\n", line);
    free(line);
  }
  return EXIT_SUCCESS;
}
