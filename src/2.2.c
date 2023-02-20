/**
 * @file 2.2.c
 * @author Derek Huang
 * @brief The C Programming Language exercise 2-2
 * @copyright MIT License
 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PDCPL_HAS_PROGRAM_USAGE
#include "pdcpl/cliopts.h"
#include "pdcpl/core.h"

#define CHAR_LIMIT 10

PDCPL_PROGRAM_USAGE_DEF
(
  "Read " PDCPL_STRINGIFY(CHAR_LIMIT)
    " chars of a line from stdin and print to stdout.\n"
  "\n"
  "This is just a trivial exercise that rewrites the for loop used in the\n"
  "bools getline() function without using any && or || operators. If the\n"
  "top-level CMakeLists.txt is used as input, the output would be\n"
  "\n"
  "  Read cmake_mini (10 chars, maximum 10)"
)

PDCPL_ARG_MAIN
{
  PDCPL_HANDLE_INFO_OPTS();
  // loop index, buffer size, getchar() output, buffer to receive chars
  size_t i, lim = CHAR_LIMIT + 1;
  int c;
  char *s = calloc(lim, sizeof *s);
  // read a line of input, lim - 1 chars max.
  //
  // the original for loop (pg. 41, section 2.6):
  //
  // for (i=0; i<lim-1 && (c=getchar()) != '\n' && c != EOF; ++i)
  //   s[i] = c;
  //
  // as per the exercise, no && or || are used. don't do this in real code.
  for (i = 0; i < lim - 1; ++i) {
    c = getchar();
    if (c == '\n')
      break;
    if (c == EOF)
      break;
    s[i] = (char) c;
  }
  // use %d specifier since CHAR_LIMIT is an int
  printf("Read %s (%zu chars, maximum %d)\n", s, strlen(s), CHAR_LIMIT);
  free(s);
  // check for stream error before exiting
  return (ferror(stdin)) ? EXIT_FAILURE : EXIT_SUCCESS;
}
